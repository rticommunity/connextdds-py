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

    if (!py::detail::compare_buffer_info<T>::compare(info)) {
        throw py::type_error("Bad buffer element type");
    }

    if (info.strides[0] / static_cast<ssize_t>(sizeof(T)) != 1) {
        throw py::type_error(
                "Bad buffer type: only valid 1D buffers are allowed");
    }
}

// Uses the C interpreter to resize a sequence of complex types, initializing
// or finalizing elements as needed.
//
// - member_plugin is the plugin for the sequence's type
// - c_sample is the ctypes sample containing the sequence member
// - member_offset is the offset to the sequence member
// - element_count is the new element count
//
// This function is used as part of the idl_impl.sample_interpreter package
// to convert unbounded Python sequences to C.
static void resize_c_sequence(
        py::object& c_sample,
        const DynamicType& member_dynamic_type,
        size_t member_offset,
        size_t element_count)
{
    RTIXCdrBoolean failure = RTI_XCDR_FALSE;
    // RTIXCdrTypeCodeMember member_info;
    PyCTypesBuffer c_sample_buffer(c_sample);
    DDS_Sequence_set_member_element_count(
            &failure,
            c_sample_buffer.py_buffer.buf, // sample
            (RTIXCdrUnsignedLong) element_count,
            (RTIXCdrUnsignedLong) member_offset,
            reinterpret_cast<const RTIXCdrTypeCode*>(
                    &member_dynamic_type.native()), // memberInfo
            nullptr, // TODO CORE-11838: this has to be passed in to support optionals
            RTI_XCDR_TRUE, // allocateMemberIfNull
            RTI_XCDR_TRUE, // trimToSize
            RTI_XCDR_TRUE, // initializeElement
            nullptr); // programData

    if (failure) {
        throw dds::core::Error("Failed to resize sequence");
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

    // Used by TypeSupport.serialize, expects a sample that has already been
    // converted to C
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
                CSampleWrapper sample_wrapper = c_sample_buffer;
                self.type_plugin->deserialize_from_cdr_buffer(
                        sample_wrapper,
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
                CSampleWrapper sample_wrapper = c_sample_buffer;
                self.type_plugin->initialize_sample(sample_wrapper);
            },
            py::arg("c_sample"),
            py::call_guard<py::gil_scoped_acquire>());

    cls.def(
            "finalize_sample",
            [](const TypePlugin& self, py::object c_sample) {
                PyCTypesBuffer c_sample_buffer(c_sample);

                py::gil_scoped_release release;
                CSampleWrapper sample_wrapper = c_sample_buffer;
                self.type_plugin->finalize_sample(sample_wrapper);
            },
            py::arg("c_sample"),
            py::call_guard<py::gil_scoped_acquire>());

    cls.def_static(
            "resize_sequence",
            resize_c_sequence,
            py::call_guard<py::gil_scoped_release>());
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
                return reinterpret_cast<size_t>(self.sample);
            });

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