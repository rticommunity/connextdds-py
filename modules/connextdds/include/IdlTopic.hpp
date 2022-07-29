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
#include "IdlTypeSupport.hpp"

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

inline PyIdlTopic create_idl_py_topic(
        PyDomainParticipant& participant,
        const ::std::string& topic_name,
        py::object& type,
        const dds::topic::qos::TopicQos* qos = nullptr,
        PyTopicListenerPtr<rti::topic::cdr::CSampleWrapper> listener = nullptr,
        const dds::core::status::StatusMask& mask =
                dds::core::status::StatusMask::all())
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
                    listener,
                    mask));

    if (listener != nullptr) {
        py::gil_scoped_acquire acquire;
        py::cast(listener).inc_ref();
    }

    // Store the type support in the user data; increase its reference count
    // while the topic exists, and decrease it upon destruction.
    type_support.inc_ref();
    topic->set_user_data_(type_support.ptr(), [](void* ptr) {
        py::gil_scoped_acquire acquire;
        auto py_object = py::handle(static_cast<PyObject*>(ptr));
        py_object.dec_ref();
    });

    return topic;
}

}  // namespace pyrti