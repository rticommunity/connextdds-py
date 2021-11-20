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

// Gets the python TypeSupport from a C++ Topic object, which is stored in its
// user data.
static py::handle get_py_type_support_from_topic(PyTopic<CSampleWrapper>& topic)
{
    return py::handle(static_cast<PyObject*>(topic->get_user_data_()));
}

using rti::topic::cdr::CTypePlugin;

using IdlTopicPyClass = py::class_<
        PyTopic<CSampleWrapper>,
        PyITopicDescription<CSampleWrapper>,
        PyIAnyTopic,
        std::unique_ptr<
                PyTopic<CSampleWrapper>,
                no_gil_delete<PyTopic<CSampleWrapper>>>>;

using IdlTopicDescriptionPyClass = py::class_<
        PyTopicDescription<CSampleWrapper>,
        PyITopicDescription<CSampleWrapper>,
        std::unique_ptr<
                PyTopicDescription<CSampleWrapper>,
                no_gil_delete<PyTopicDescription<CSampleWrapper>>>>;

using IdlITopicDescriptionPyClass = py::class_<
        PyITopicDescription<CSampleWrapper>,
        PyIEntity,
        std::unique_ptr<
                PyITopicDescription<CSampleWrapper>,
                no_gil_delete<PyITopicDescription<CSampleWrapper>>>>;

static PyTopic<CSampleWrapper> create_idl_py_topic(
        PyDomainParticipant& participant,
        const ::std::string& topic_name,
        py::object& type,
        const dds::topic::qos::TopicQos *qos = NULL)
{
    // Get the type support created in python
    py::object type_support = get_type_support_from_idl_type(type);

    // Get the Type Plugin based on a DynamicType that was created
    // reflectively in python by inspecting the IDL-derived
    // dataclass
    CTypePlugin* plugin = get_type_plugin_from_type_support(type_support);

    // Register the plugin with this Topic's participant
    plugin->register_type(participant);

    auto topic = PyTopic<CSampleWrapper>(
            dds::core::construct_from_native_tag_t(),
            new rti::topic::TopicImpl<CSampleWrapper>(
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

template<>
void init_dds_typed_topic_template(IdlTopicPyClass &cls)
{
    // First initialize the "base" class, i.e. the python methods that are
    // common to all topics
    //
    // TODO PY-20: the constructor from AnyTopic is not safe for IDL Topics
    // as it is implemented in the base class.
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