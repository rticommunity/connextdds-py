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

#include <pybind11/numpy.h>

#include "PyConnext.hpp"
#include "PySeq.hpp"
#include "PyGenericTypePluginFactory.hpp"
#include "PyInitType.hpp"
#include "PyDynamicTypeMap.hpp"
#include "PyInitOpaqueTypeContainers.hpp"

#include <dds/core/xtypes/DynamicData.hpp>
#include <dds/core/QosProvider.hpp>

using namespace dds::core::xtypes;
using namespace dds::topic;
using namespace rti::topic::cdr;

namespace pyrti {

// TODO PY-16: rename to get_buffer
void* get_buffer(const py::object& wrapper)
{
    PyObject* ptr = wrapper.ptr();

    // TODO PY-16: return buffer must be called
    Py_buffer pybuffer;
    int result = PyObject_GetBuffer(ptr, &pybuffer, 0);
    return pybuffer.buf;
}

// From an @idl_type-decorated python dataclass we get its DynamicType,
// which must be cached in the associated type_support
//
// @pre The GIL must be held
// @throw TypeError if idl_type is not a valid @idl_type-annotated type
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

static CTypePlugin* get_type_plugin_from_type_support(py::object& type_support)
{
    // get the struct holding the DynamicType and the associated Type Plugin
    auto py_type_plugin_holder = type_support.attr("_plugin_dynamic_type");

    // pybind11 magic allows casting the python object to the C++ one
    PluginDynamicTypeHolder type_plugin_holder =
            py::cast<PluginDynamicTypeHolder>(py_type_plugin_holder);

    return type_plugin_holder.type_plugin;
}


using rti::topic::cdr::CTypePlugin;

using IdlGenericTopicPyClass = py::class_<
        PyTopic<CSampleWrapper>,
        PyITopicDescription<CSampleWrapper>,
        PyIAnyTopic,
        std::unique_ptr<
                PyTopic<CSampleWrapper>,
                no_gil_delete<PyTopic<CSampleWrapper>>>>;

using IdlGenericTopicDescriptionPyClass = py::class_<
        PyTopicDescription<CSampleWrapper>,
        PyITopicDescription<CSampleWrapper>,
        std::unique_ptr<
                PyTopicDescription<CSampleWrapper>,
                no_gil_delete<PyTopicDescription<CSampleWrapper>>>>;

using IdlGenericITopicDescriptionPyClass = py::class_<
        PyITopicDescription<CSampleWrapper>,
        PyIEntity,
        std::unique_ptr<
                PyITopicDescription<CSampleWrapper>,
                no_gil_delete<PyITopicDescription<CSampleWrapper>>>>;

template<>
void init_dds_typed_topic_template(IdlGenericTopicPyClass &cls)
{
    init_dds_typed_topic_base_template(cls);
    cls.def(py::init([](PyDomainParticipant &participant,
                        const ::std::string &topic_name,
                        py::object &type) {

                // Get the type support created in python
                py::object type_support = get_type_support_from_idl_type(type);

                // Get the Type Plugin based on a DynamicType that was created 
                // reflectively in python by inspecting the IDL-derived dataclass
                CTypePlugin *plugin =
                        get_type_plugin_from_type_support(type_support);

                // Register the plugin with this Topic's participant 
                plugin->register_type(participant);

                auto topic = PyTopic<CSampleWrapper>(
                        dds::core::construct_from_native_tag_t(),
                        new rti::topic::TopicImpl<CSampleWrapper>(
                                rti::topic::detail::no_register_tag_t(),
                                participant,
                                topic_name,
                                plugin->type_name().c_str(),
                                NULL,
                                nullptr,
                                dds::core::status::StatusMask::none()));

                topic->set_user_data_(type_support.ptr());

                return topic;
            }),
            py::arg("participant"),
            py::arg("topic_name"),
            py::arg("type"),
            "Create a Topic for an @idl_type-decorated type.")
            .def_property_readonly(
                    "type_support",
                    [](PyTopic<CSampleWrapper> &self) {
                        return py::handle(static_cast<PyObject *>(
                                self->get_user_data_()));
                    },
                    "Get the type_support object associated with the topic.");
}

// TODO PY-16: all that follows is temporary

using IdlDataWriterPyClass = py::class_<
        PyDataWriter<CSampleWrapper>,
        PyIEntity,
        PyIAnyDataWriter,
        std::unique_ptr<
                PyDataWriter<CSampleWrapper>,
                no_gil_delete<PyDataWriter<CSampleWrapper>>>>;

template<>
void init_dds_typed_datawriter_template(IdlDataWriterPyClass &cls)
{
    init_dds_typed_datawriter_base_template(cls);
    cls.def(
            "write",
            [](PyDataWriter<CSampleWrapper> &dw, py::object &sample)
            {

                auto type_support = py::handle(
                    static_cast<PyObject *>(dw.topic()->get_user_data_()));

                auto c_sample = type_support.attr("_create_c_sample")(sample);

                CSampleWrapper wrapped_c_sample { get_buffer(c_sample) };

                py::gil_scoped_release release;
                dw.write(wrapped_c_sample);
            },
            py::arg("sample_data"),
            "Write a data sample.");
    // .def(
    //         "write_async",
    //         [](PyDataWriter<dds::core::xtypes::DynamicData> &dw,
    //            py::dict &dict) {
    //             return PyAsyncioExecutor::run<void>(
    //                     std::function<void()>([&dw, &dict]() {
    //                         py::gil_scoped_acquire acquire;
    //                         auto dt = PyDynamicTypeMap::get(
    //                                 dw->type_name());
    //                         dds::core::xtypes::DynamicData sample(dt);
    //                         update_dynamicdata_object(sample, dict);
    //                         {
    //                             py::gil_scoped_release release;
    //                             dw.write(sample);
    //                         }
    //                     }));
    //         },
    //         py::arg("sample_data"),
    //         py::keep_alive<0, 1>(),
    //         py::keep_alive<0, 2>(),
    //         "Create a DynamicData object and write it with the given "
    //         "dictionary containing field names as keys. This method is "
    //         "awaitable and is only for use with asyncio.")
    // .def(
    //         "create_data",
    //         [](PyDataWriter<dds::core::xtypes::DynamicData> &dw) {
    //             return dds::core::xtypes::DynamicData(
    //                     PyDynamicTypeMap::get(dw->type_name()));
    //         },
    //         py::call_guard<py::gil_scoped_release>(),
    //         "Create data of the writer's associated type and "
    //         "initialize it.")
    // .def(
    //         "key_value",
    //         [](PyDataWriter<dds::core::xtypes::DynamicData> &dw,
    //            const dds::core::InstanceHandle &handle) {
    //             dds::core::xtypes::DynamicData d(
    //                     PyDynamicTypeMap::get(dw->type_name()));
    //             dw.key_value(d, handle);
    //             return d;
    //         },
    //         py::arg("handle"),
    //         py::call_guard<py::gil_scoped_release>(),
    //         "Retrieve the instance key that corresponds to an instance "
    //         "handle.")
    // .def(
    //         "topic_instance_key_value",
    //         [](PyDataWriter<dds::core::xtypes::DynamicData> &dw,
    //            const dds::core::InstanceHandle &handle) {
    //             dds::core::xtypes::DynamicData d(
    //                     PyDynamicTypeMap::get(dw->type_name()));
    //             dds::topic::TopicInstance<
    //                     dds::core::xtypes::DynamicData>
    //                     ti(handle, d);
    //             dw.key_value(ti, handle);
    //             return ti;
    //         },
    //         py::arg("handle"),
    //         py::call_guard<py::gil_scoped_release>(),
    //         "Retrieve the instance key that corresponds to an instance "
    //         "handle.");
}

using IdlDataReaderPyClass = py::class_<
        PyDataReader<CSampleWrapper>,
        PyIDataReader,
        std::unique_ptr<
                PyDataReader<CSampleWrapper>,
                no_gil_delete<PyDataReader<CSampleWrapper>>>>;

template<>
void init_dds_typed_datareader_template(IdlDataReaderPyClass& cls)
{
    init_dds_typed_datareader_base_template(cls);
    cls.def(
            "take_valid_data",
            [](PyDataReader<CSampleWrapper> &dr) {
                py::gil_scoped_release release;
                auto topic = dds::core::polymorphic_cast<
                        dds::topic::Topic<CSampleWrapper>>(
                        dr.topic_description());
                auto type_support =
                        py::handle(static_cast<PyObject *>(topic->get_user_data_()));

                auto tmp = dr.take();
                auto samples = rti::sub::valid_data(std::move(tmp));
                std::vector<py::object> py_samples;

                py::gil_scoped_acquire acquire;

                // This is the type support function that converts from C data
                // to the user-facing python object.
                auto create_py_sample = type_support.attr("_create_py_sample");
                for (auto &sample : samples) {
                    // We pass the pointer to the python function as an integer,
                    // because that's what ctypes.cast expects.
                    size_t sample_ptr =
                            reinterpret_cast<size_t>((sample.data().sample));
                    py_samples.push_back(
                            create_py_sample(sample_ptr));
                }

                return py_samples;
            },
            "Take copies of all available valid data");
}

template<>
void process_inits<rti::topic::cdr::CSampleWrapper>(
        py::module &module,
        ClassInitList &list)
{
    using rti::topic::cdr::CSampleWrapper;

    list.push_back([module] {
        return ([module]() mutable {
            IdlGenericITopicDescriptionPyClass itd(module, "ITopicDescription");
            IdlGenericTopicDescriptionPyClass td(module, "TopicDescription");
            IdlGenericTopicPyClass t(module, "Topic");

            pyrti::bind_vector<PyTopic<CSampleWrapper>>(module, "TopicSeq");
            py::implicitly_convertible<
                    py::iterable,
                    std::vector<PyTopic<CSampleWrapper>>>();
            init_topic<CSampleWrapper>(itd, td, t);

            IdlDataWriterPyClass dw(module, "DataWriter");

            pyrti::bind_vector<PyDataWriter<CSampleWrapper>>(
                    module,
                    "DataWriterSeq");
            py::implicitly_convertible<
                    py::iterable,
                    std::vector<PyDataWriter<CSampleWrapper>>>();

            init_datawriter<CSampleWrapper>(dw);

            IdlDataReaderPyClass dr(module, "DataReader");

            pyrti::bind_vector<PyDataReader<CSampleWrapper>>(
                    module,
                    "DataReaderSeq");
            py::implicitly_convertible<
                    py::iterable,
                    std::vector<PyDataReader<CSampleWrapper>>>();

            init_datareader<CSampleWrapper>(dr);
        });
    });
}

} // namespace pyrti