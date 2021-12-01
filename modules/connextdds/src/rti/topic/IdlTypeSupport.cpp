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

#include "IdlTopic.hpp"

using namespace dds::core::xtypes;
using namespace dds::topic;
using namespace rti::topic::cdr;

namespace pyrti {

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


struct PyCTypesBuffer {
    Py_buffer py_buffer;

    explicit PyCTypesBuffer(const py::object& ctypes_sample)
    {
        PyObject* py_object_ptr = ctypes_sample.ptr();
        PyObject_GetBuffer(py_object_ptr, &py_buffer, PyBUF_SIMPLE);
    }

    // move-only struct
    PyCTypesBuffer(const PyCTypesBuffer&) = delete;
    PyCTypesBuffer& operator=(const PyCTypesBuffer&) = delete;
    PyCTypesBuffer(PyCTypesBuffer&& other) = default;
    PyCTypesBuffer& operator=(PyCTypesBuffer&& other) = default;

    ~PyCTypesBuffer()
    {
        PyBuffer_Release(&py_buffer);
    }

    // Obtain the actual buffer as a CSampleWrapper, as required by
    // PyDataWriter::py_write
    operator CSampleWrapper()
    {
        return { py_buffer.buf };
    }
};

struct PyToCSampleConverter {
    static PyCTypesBuffer convert(
            dds::pub::DataWriter<CSampleWrapper>& writer,
            const py::object& sample)
    {
        auto&& type_support = py::handle(
                static_cast<PyObject*>(writer.topic()->get_user_data_()));
        auto&& c_sample = type_support.attr("_create_c_sample")(sample);
        return PyCTypesBuffer(c_sample);
    }
};

struct IdlDataWriterTraits {
    using py_type = py::object;
    using before_write_gil_policy = py::gil_scoped_acquire;
    using native_write_gil_policy = py::gil_scoped_release;
    using sample_converter = PyToCSampleConverter;
};

using IdlPyDataWriter = TPyDataWriter<CSampleWrapper, IdlDataWriterTraits>;

using IdlDataWriterPyClass =
        PyDataWriterClass<CSampleWrapper, IdlDataWriterTraits>;

template<>
void init_dds_typed_datawriter_template(IdlDataWriterPyClass &cls)
{
    init_dds_typed_datawriter_base_template(cls);

}

// TODO PY-16: all that follows is temporary

using IdlDataReaderPyClass = py::class_<
        PyDataReader<CSampleWrapper>,
        PyIDataReader,
        std::unique_ptr<
                PyDataReader<CSampleWrapper>,
                no_gil_delete<PyDataReader<CSampleWrapper>>>>;

template<>
void init_dds_typed_datareader_template(IdlDataReaderPyClass& cls);

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