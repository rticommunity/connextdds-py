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
#include "PyInitOpaqueTypeContainers.hpp"

#include "IdlTopic.hpp"
#include "IdlContentFilteredTopic.hpp"
#include "IdlDataWriter.hpp"
#include "IdlDataReader.hpp"

#include "dds_c/dds_c_interpreter.h"

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

    if (info.itemsize != 1) {
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
void init_class_defs(py::class_<TypePlugin>& cls)
{
    // Get the DynamicType without making a copy; this type includes all
    // the access sample info. This is used internally only.
    cls.def(
            "get_dynamic_type_ref",
            [](const TypePlugin& self) { return self.type; },
            py::return_value_policy::reference);

    // This function serves two purposes:
    //
    // 1) It allows to polymorphically get the dynamic type of a member, which
    // the C++ API currently doesn't allow (it requires knowing whether it's a
    // struct or a union), and
    //
    // 2) it uses py::return_value_policy::reference to
    // avoid making a copy and preserving the sampleAccessInfo. This is safe
    // because the lifecycle of the type and access info is tied to the type
    // plugin factory.
    cls.def(
            "get_member_dynamic_type_ref",
            [](const TypePlugin& self,
               const std::string& member_name) {
                DDS_ExceptionCode_t ex;
                auto index = DDS_TypeCode_find_member_by_name(
                        &self.type->native(),
                        member_name.c_str(),
                        &ex);
                rti::core::check_tc_ex_code(
                        ex,
                        "failed to find member by name");

                DDS_TypeCode *member_type = DDS_TypeCode_member_type(
                        &self.type->native(),
                        index,
                        &ex);
                rti::core::check_tc_ex_code(
                        ex,
                        "failed to get member type");

                return reinterpret_cast<dds::core::xtypes::DynamicType*>(member_type);
            },
            py::return_value_policy::reference);

    // Return a copy without the sample access info.
    cls.def(
            "clone_type",
            [](const TypePlugin& self) { return py_cast_type(*self.type); });

    // Gets the serialized sample size of a given sample
    cls.def(
            "serialized_sample_size",
            [](const TypePlugin& self, py::object c_sample) {
                PyCTypesBuffer c_sample_buffer(c_sample);
                return self.type_plugin->serialized_sample_size(c_sample_buffer);
            },
            py::arg("c_sample"),
            py::call_guard<py::gil_scoped_release>());

    // Gets the maximum serialized sample size of a type
    cls.def(
            "max_serialized_sample_size",
            [](const TypePlugin& self) {
                return self.type_plugin->serialized_sample_max_size();
            },
            py::call_guard<py::gil_scoped_release>());

    // Used by TypeSupport.serialize, expects a sample that has already
    // been converted to C
    cls.def(
            "serialize",
            [](const TypePlugin& self, py::object c_sample) {
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
            [](const TypePlugin& self,
               py::object c_sample,
               py::buffer buffer) {
                // TODO PY-25: Add support for an input buffer type that is not
                // convertible to py::buffer (e.g. a list of bytes).
                auto buffer_info = buffer.request();
                validate_cdr_buffer_type<uint8_t>(buffer_info);

                PyCTypesBuffer c_sample_buffer(c_sample);

                py::gil_scoped_release release;
                self.type_plugin->deserialize_from_cdr_buffer(
                        c_sample_buffer,
                        reinterpret_cast<char*>(buffer_info.ptr),
                        buffer_info.size);
            },
            py::arg("c_sample"),
            py::arg("buffer"),
            py::call_guard<py::gil_scoped_acquire>());

    cls.def(
            "initialize_sample",
            [](const TypePlugin& self,
               py::object c_sample) {
                PyCTypesBuffer c_sample_buffer(c_sample);

                py::gil_scoped_release release;
                self.type_plugin->initialize_sample(c_sample_buffer);
            },
            py::arg("c_sample"),
            py::call_guard<py::gil_scoped_acquire>());

    cls.def(
            "finalize_sample",
            [](const TypePlugin& self, py::object c_sample) {
                PyCTypesBuffer c_sample_buffer(c_sample);

                py::gil_scoped_release release;
                self.type_plugin->finalize_sample(c_sample_buffer);
            },
            py::arg("c_sample"),
            py::call_guard<py::gil_scoped_acquire>());

    cls.def(
            "initialize_member",
            [](const TypePlugin& self,
               py::object c_sample,
               uint32_t member_index) {
                PyCTypesBuffer c_sample_buffer(c_sample);

                py::gil_scoped_release release;
                self.type_plugin->initialize_member(
                        c_sample_buffer,
                        member_index);
            },
            py::arg("c_sample"),
            py::arg("member_index"),
            py::call_guard<py::gil_scoped_acquire>());

    cls.def(
            "resize_member",
            [](const TypePlugin& self,
               py::object c_sample,
               uint32_t member_index,
               uint32_t new_size) {
                PyCTypesBuffer c_sample_buffer(c_sample);

                py::gil_scoped_release release;
                self.type_plugin->resize_member(
                        c_sample_buffer,
                        member_index,
                        new_size);
            },
            py::arg("c_sample"),
            py::arg("member_index"),
            py::arg("new_size"),
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

            // TODO PY-18: This is temporary while some functions are not
            // yet updated to take python sample so that the stub generation
            // doesn't complain about not knowing what CSampleWrapper is.
            py::class_<CSampleWrapper> sample_wrapper_cls(
                    module,
                    "CSampleWrapper");

            sample_wrapper_cls.def("get_ptr", [](CSampleWrapper& self) {
                return reinterpret_cast<size_t>(self.sample());
            });

            IdlITopicDescriptionPyClass itd(module, "ITopicDescription");
            IdlTopicDescriptionPyClass td(module, "TopicDescription");
            IdlTopicPyClass t(module, "Topic");
            IdlContentFilteredTopicPyClass cft(module, "ContentFilteredTopic");

            pyrti::bind_vector<PyTopic<CSampleWrapper>>(module, "TopicSeq");
            py::implicitly_convertible<
                    py::iterable,
                    std::vector<PyTopic<CSampleWrapper>>>();
            init_topic<CSampleWrapper>(itd, td, t);

            pyrti::bind_vector<PyContentFilteredTopic<CSampleWrapper>>(
                    module,
                    "ContentFilteredTopicSeq");
            py::implicitly_convertible<
                    py::iterable,
                    std::vector<PyContentFilteredTopic<CSampleWrapper>>>();
            init_content_filtered_topic<CSampleWrapper>(cft);

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