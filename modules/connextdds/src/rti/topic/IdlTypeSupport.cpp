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
#include "IdlTypeSupport.hpp"
#include "PyInitType.hpp"
#include "PyDynamicTypeMap.hpp"
#include "PyInitOpaqueTypeContainers.hpp"

#include "IdlTopic.hpp"
#include "IdlDataWriter.hpp"
#include "IdlDataReader.hpp"

using namespace dds::core::xtypes;
using namespace dds::topic;
using namespace rti::topic::cdr;

namespace pyrti {

// Checks that a Python buffer definition is valid for CDR serialization
template<typename T>
static void validate_cdr_buffer_type(const py::buffer_info& info)
{
    if (info.ndim != 1 || info.strides[0] % static_cast<ssize_t>(sizeof(T))) {
        throw py::type_error(
                "Bad buffer type: only valid 1D buffers are allowed");
    }

    if (!py::detail::compare_buffer_info<T>::compare(info)) {
        throw py::type_error("Bad buffer element type");
    }

    if (info.strides[0] / static_cast<ssize_t>(sizeof(T)) != 1) {
        throw py::type_error(
                "Bad buffer type: only valid 1D buffers are allowed");
    }
}

// This class and methods are internally used (in Python code) by the
// TypeSupport class
template<>
void init_class_defs(py::class_<PluginDynamicTypeHolder>& cls)
{
    cls.def("clone", [](const PluginDynamicTypeHolder& self) {
        rti::core::xtypes::DynamicTypeImpl copy = *self.type;
        return py_cast_type(copy);
    });

    // Used by TypeSupport.serialize, expects a sample that has already been
    // converted to C
    cls.def(
            "serialize",
            [](const PluginDynamicTypeHolder& self, py::object c_sample) {
                PyCTypesBuffer c_sample_buffer(c_sample);
                std::vector<uint8_t> cdr_buffer;
                self.type_plugin->serialize_to_cdr_buffer(
                        reinterpret_cast<std::vector<char>&>(cdr_buffer),
                        c_sample_buffer);

                return cdr_buffer;
            },
            py::arg("c_sample"),
            py::call_guard<py::gil_scoped_release>());

    // Used by TypeSupport.deserialize, expects a sample that has already been
    // converted to C
    cls.def(
            "deserialize",
            [](const PluginDynamicTypeHolder& self,
               py::object c_sample,
               py::buffer buffer) {
                // TODO PY-25: Add support for an input buffer type that is not
                // convertible to py::buffer (e.g. a list of bytes).
                auto buffer_info = buffer.request();
                validate_cdr_buffer_type<uint8_t>(buffer_info);

                PyCTypesBuffer c_sample_buffer(c_sample);

                py::gil_scoped_release release;
                CSampleWrapper sample_wrapper = c_sample_buffer;
                self.type_plugin->deserialize_from_cdr_buffer(
                        sample_wrapper,
                        reinterpret_cast<char*>(buffer_info.ptr),
                        buffer_info.size);
            },
            py::arg("c_sample"),
            py::arg("buffer"),
            py::call_guard<py::gil_scoped_acquire>());
}

template<>
void init_dds_typed_topic_template(IdlTopicPyClass &cls)
{
    // First initialize the "base" class, i.e. the python methods that are
    // common to all topics
    init_dds_typed_topic_base_template(cls);

    // Then add new methods that are only available for IDL topics
    cls.def(py::init([](PyDomainParticipant& participant,
                        const ::std::string& topic_name,
                        py::object& type) {
                return create_idl_py_topic(participant, topic_name, type);
            }),
            py::arg("participant"),
            py::arg("topic_name"),
            py::arg("type"),
            "Create a Topic for an @idl.struct or @idl.union type with default "
            "QoS");

    cls.def(py::init([](PyDomainParticipant& participant,
                        const ::std::string& topic_name,
                        py::object& type,
                        const dds::topic::qos::TopicQos& qos) {
                return create_idl_py_topic(participant, topic_name, type, &qos);
            }),
            py::arg("participant"),
            py::arg("topic_name"),
            py::arg("type"),
            py::arg("qos"),
            "Create a Topic for an @idl.struct or @idl.union type with specific "
            "QoS");

    // TODO PY-17: add the rest of constructors (with listener)

    cls.def_property_readonly(
            "type_support",
            [](PyTopic<CSampleWrapper>& self) {
                return get_py_type_support_from_topic(self);
            },
            "Get the type_support object associated with the topic.");

    cls.def_property_readonly(
            "type",
            [](PyTopic<CSampleWrapper>& self) {
                return get_py_type_support_from_topic(self).attr("type");
            },
            "Get the type associated with the topic.");
}

// Initializes the Topic, DataWriter and DataReader Python classes for IDL types
// (dds.Topic, dds.DataWriter and dds.DataReader)
template<>
void process_inits<rti::topic::cdr::CSampleWrapper>(
        py::module &module,
        ClassInitList &list)
{
    using rti::topic::cdr::CSampleWrapper;

    list.push_back([module] {
        return ([module]() mutable {
            IdlITopicDescriptionPyClass itd(module, "ITopicDescription");
            IdlTopicDescriptionPyClass td(module, "TopicDescription");
            IdlTopicPyClass t(module, "Topic");

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

            init_datawriter(dw);

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