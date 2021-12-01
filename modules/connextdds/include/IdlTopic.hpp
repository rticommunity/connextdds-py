/*
 * (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */

#pragma once

#include "PyConnext.hpp"
#include "PyTopic.hpp"
#include "PyGenericTypePluginFactory.hpp"

namespace pyrti {

using PyIdlTopic = PyTopic<rti::topic::cdr::CSampleWrapper>;

using IdlTopicPyClass = py::class_<
        PyIdlTopic,
        PyITopicDescription<rti::topic::cdr::CSampleWrapper>,
        PyIAnyTopic,
        std::unique_ptr<PyIdlTopic, no_gil_delete<PyIdlTopic>>>;

using IdlTopicDescriptionPyClass = py::class_<
        PyTopicDescription<rti::topic::cdr::CSampleWrapper>,
        PyITopicDescription<rti::topic::cdr::CSampleWrapper>,
        std::unique_ptr<
                PyTopicDescription<rti::topic::cdr::CSampleWrapper>,
                no_gil_delete<
                        PyTopicDescription<rti::topic::cdr::CSampleWrapper>>>>;

using IdlITopicDescriptionPyClass = py::class_<
        PyITopicDescription<rti::topic::cdr::CSampleWrapper>,
        PyIEntity,
        std::unique_ptr<
                PyITopicDescription<rti::topic::cdr::CSampleWrapper>,
                no_gil_delete<
                        PyITopicDescription<rti::topic::cdr::CSampleWrapper>>>>;


// From an @idl_type-decorated python dataclass we get its DynamicType,
// which must be cached in the associated type_support
//
// @pre The GIL must be held
// @throw TypeError if idl_type is not a valid @idl_type-annotated type
inline
static py::object get_type_support_from_idl_type(py::object& type)
{
    // type must be a type
    if (!py::isinstance<py::type>(type)) {
        throw py::type_error(
                "Incompatible 'type' argument: an @idl.struct or @idl.union is "
                "required");
    }

    // type must have a type_support attribute (which is added by the decorator)
    if (!py::hasattr(type, "type_support")) {
        throw py::type_error(
                "Incompatible 'type' argument: an @idl.struct or @idl.union is "
                "required");
    }

    // the type_support must have a dynamic_type attribute
    auto type_support = type.attr("type_support");
    if (!py::hasattr(type_support, "_plugin_dynamic_type")) {
        throw py::type_error(
                "Incompatible 'type' argument: not a valid an @idl.struct or "
                "@idl.union");
    }

    return type_support;
}

inline
static rti::topic::cdr::CTypePlugin* get_type_plugin_from_type_support(
        py::object& type_support)
{
    // get the struct holding the DynamicType and the associated Type
    // Plugin
    auto py_type_plugin_holder = type_support.attr("_plugin_dynamic_type");

    // pybind11 magic allows casting the python object to the C++ one
    PluginDynamicTypeHolder type_plugin_holder =
            py::cast<PluginDynamicTypeHolder>(py_type_plugin_holder);

    return type_plugin_holder.type_plugin;
}

// Gets the python TypeSupport from a C++ Topic object, which is stored in its
// user data.
inline
static py::handle get_py_type_support_from_topic(PyIdlTopic& topic)
{
    return py::handle(static_cast<PyObject*>(topic->get_user_data_()));
}

inline
PyIdlTopic create_idl_py_topic(
        PyDomainParticipant& participant,
        const ::std::string& topic_name,
        py::object& type,
        const dds::topic::qos::TopicQos* qos = nullptr)
{
    // Get the type support created in python
    py::object type_support = get_type_support_from_idl_type(type);

    // Get the Type Plugin based on a DynamicType that was created
    // reflectively in python by inspecting the IDL-derived
    // dataclass
    rti::topic::cdr::CTypePlugin* plugin =
            get_type_plugin_from_type_support(type_support);

    // Register the plugin with this Topic's participant
    plugin->register_type(participant);

    auto topic = PyIdlTopic(
            dds::core::construct_from_native_tag_t(),
            new rti::topic::TopicImpl<rti::topic::cdr::CSampleWrapper>(
                    rti::topic::detail::no_register_tag_t(),
                    participant,
                    topic_name,
                    plugin->type_name().c_str(),
                    qos,
                    nullptr,
                    dds::core::status::StatusMask::none()));

    topic->set_user_data_(type_support.ptr());

    return topic;
}

}  // namespace pyrti