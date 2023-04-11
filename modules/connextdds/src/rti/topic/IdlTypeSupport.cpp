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
#include "IdlTopicListener.hpp"
#include "IdlDataWriter.hpp"
#include "IdlDataWriterListener.hpp"
#include "IdlDataReader.hpp"
#include "IdlDataReaderListener.hpp"

#include "dds_c/dds_c_interpreter.h"

using namespace dds::core::xtypes;
using namespace dds::topic;
using namespace rti::topic::cdr;

namespace pyrti {

DDS_Topic* PyFactoryIdlPluginSupport::create_topic(
        DDS_DomainParticipant* c_participant,
        const char* topic_name,
        const char* type_name,
        const struct DDS_TopicQos* qos,
        const ListenerInfo info,
        DDS_StatusMask mask)
{
    py::gil_scoped_acquire gil;

    // IDL types are registered here so we can retrieve the Python type_class,
    // which is required to create an IDL-based Topic.
    auto type_class = py::module::import("rti.types.type_registry")
            .attr("get_type")(type_name);

    if (type_class.is(py::none())) {
        // If the type was not registered, we assume it is a DynamicData one,
        // which is handled by the base method in the C++ API.
        return rti::domain::FactoryPluginSupport::create_topic(
                c_participant,
                topic_name,
                type_name,
                qos,
                info,
                mask);
    }

    auto participant = rti::core::detail::create_from_native_entity<
            PyDomainParticipant>(c_participant);
    auto topic = create_idl_py_topic(
            participant,
            topic_name,
            type_class,
            type_name,
            reinterpret_cast<const dds::topic::qos::TopicQos*>(qos));
    // The topic has to be explicitly retained so it's not destroyed right away
    topic.retain();
    return topic->native_topic();
}

static PyTopic<CSampleWrapper> get_idl_topic_from_c_topic(DDS_Topic *c_topic)
{
    if (c_topic == nullptr) {
        return dds::core::null;
    }

    const char* endpoint_name = DDS_TopicDescription_get_endpoint_type_name(
            DDS_Topic_as_topicdescription(c_topic));
    if (endpoint_name != nullptr
            && strcmp(endpoint_name, DDS_DYNAMIC_DATA_PLUGIN_TYPE_NAME) == 0) {
        return dds::core::null;
    }

    return rti::core::detail::create_from_native_entity<
            PyTopic<CSampleWrapper>>(c_topic);
}

DDS_TopicDescription* PyFactoryIdlPluginSupport::create_content_filtered_topic(
        DDS_DomainParticipant* participant,
        const char* name,
        DDS_Topic* related_topic,
        const char* filter_expression,
        const struct DDS_StringSeq* filter_parameters,
        const char* filter_name)
{
    DDS_TopicDescription* td =
            rti::domain::FactoryPluginSupport::create_content_filtered_topic(
                    participant,
                    name,
                    related_topic,
                    filter_expression,
                    filter_parameters,
                    filter_name);
    if (td == nullptr) {
        return nullptr;
    }

    auto c_cft = DDS_ContentFilteredTopic_narrow(td);
    if (c_cft == nullptr) {
        return nullptr;
    }

    const char* endpoint_name =
            DDS_TopicDescription_get_endpoint_type_name(td);
    if (endpoint_name != nullptr
            && strcmp(endpoint_name, DDS_DYNAMIC_DATA_PLUGIN_TYPE_NAME) == 0) {
        // This is not an IDL-based CFT, so we don't need to recreate it as a
        // PyContentFilteredTopic<CSampleWrapper>>.
        return td;
    }

    auto cft = rti::core::detail::create_from_native_entity<
            PyContentFilteredTopic<CSampleWrapper>>(c_cft);
    // We need to retain the CFT because we also retained the related PyTopic.
    cft.retain();

    return td;
}

DDS_DataWriter* PyFactoryIdlPluginSupport::create_datawriter(
        DDS_Publisher* publisher,
        DDS_Topic* c_topic,
        const DDS_DataWriterQos* qos,
        const ListenerInfo listener_info,
        DDS_StatusMask mask)
{
    PyTopic<CSampleWrapper> topic = get_idl_topic_from_c_topic(c_topic);
    if (topic == dds::core::null) {
        // This is not an IDL Topic, it is a DynamicData one, so use the base
        // function from the C++ API.
        return rti::domain::FactoryPluginSupport::create_datawriter(
                publisher,
                c_topic,
                qos,
                listener_info,
                mask);
    }

    auto writer = create_idl_py_writer(
            rti::core::detail::create_from_native_entity<PyPublisher>(
                    publisher),
            topic,
            reinterpret_cast<const dds::pub::qos::DataWriterQos*>(qos));
    writer.retain();
    return writer->native_writer();
}

DDS_DataReader* PyFactoryIdlPluginSupport::create_datareader(
        DDS_Subscriber* c_subscriber,
        DDS_TopicDescription* c_topic_description,
        const struct DDS_DataReaderQos* qos,
        const ListenerInfo listener_info,
        DDS_StatusMask mask)
{
    PyTopic<CSampleWrapper> topic = dds::core::null;
    PyContentFilteredTopic<CSampleWrapper> cft = dds::core::null;

    auto c_topic = DDS_Topic_narrow(c_topic_description);
    if (c_topic != nullptr) {
        topic = get_idl_topic_from_c_topic(c_topic);
    } else {
        auto c_cft = DDS_ContentFilteredTopic_narrow(c_topic_description);
        if (c_cft != nullptr
                && get_idl_topic_from_c_topic(
                           DDS_ContentFilteredTopic_get_related_topic(c_cft))
                        != dds::core::null) {
            cft = rti::core::detail::create_from_native_entity<
                    PyContentFilteredTopic<CSampleWrapper>>(c_cft);
        }
    }

    if (topic == dds::core::null && cft == dds::core::null) {
        // This is not a Python IDL topic, so we use the base method to create
        // the reader
        return rti::domain::FactoryPluginSupport::create_datareader(
                c_subscriber,
                c_topic_description,
                qos,
                listener_info,
                mask);
    }

    auto subscriber =
            rti::core::detail::create_from_native_entity<PySubscriber>(
                    c_subscriber);
    auto reader = create_idl_py_reader(
            subscriber,
            topic,
            cft,
            reinterpret_cast<const dds::sub::qos::DataReaderQos*>(qos));
    reader.retain();
    return reader->native_reader();
}

void PyFactoryIdlPluginSupport::create_factory_plugin_support(
        DDS_FactoryPluginSupport* plugin_support)
{
    rti::domain::FactoryPluginSupport::create_factory_plugin_support(
            plugin_support);

    // Topic, DataReader, and DataWriter need additional setup to be configured
    // for IDL types, so the C++ constructor is not enough. Other entities don't
    // need additional initialization.
    plugin_support->createTopic = &PyFactoryIdlPluginSupport::create_topic;
    plugin_support->createContentFilteredTopic =
            &PyFactoryIdlPluginSupport::create_content_filtered_topic;
    plugin_support->createWriter =
            &PyFactoryIdlPluginSupport::create_datawriter;
    plugin_support->createReader =
            &PyFactoryIdlPluginSupport::create_datareader;
}

void register_idl_type(py::object& type, const std::string& type_name)
{
    // Get the type support created in python
    py::object type_support = get_type_support_from_idl_type(type);
    rti::topic::cdr::CTypePlugin* plugin =
            get_type_plugin_from_type_support(type_support);

    // Register the type plugin with the DPF so it can be used to register types
    // referenced in XML
    rti::topic::cdr::register_c_type_plugin<PyFactoryIdlPluginSupport>(
            plugin,
            type_name);

    // Register the python type class with the given name so it can be retrieved
    // when the XML App callbacks need to create a Python topic.
    py::gil_scoped_acquire gil;
    py::module::import("rti.types.type_registry")
            .attr("register_type")(type_name, type);
}


// Checks that a Python buffer definition is valid for CDR serialization
template<typename T>
static void validate_cdr_buffer_type(const py::buffer_info& info)
{
    if (info.ndim != 1
            || info.strides[0] % static_cast<py::ssize_t>(sizeof(T))) {
        throw py::type_error(
                "Bad buffer type: only valid 1D buffers are allowed");
    }

    if (info.itemsize != 1) {
        throw py::type_error("Bad buffer element type");
    }

    if (info.strides[0] / static_cast<py::ssize_t>(sizeof(T)) != 1) {
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

    // Return a copy without the sample access info.
    cls.def("clone_type", [](const TypePlugin& self) {
        return py_cast_type(*self.type, false /* resolve_alias */);
    });

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
            "finalize_optional_members",
            [](const TypePlugin& self, py::object c_sample) {
                PyCTypesBuffer c_sample_buffer(c_sample);

                py::gil_scoped_release release;
                self.type_plugin->finalize_optional_members(c_sample_buffer);
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

    cls.def(
            "get_instruction_index_from_member_index",
            [](const TypePlugin& self, uint32_t member_index) -> uint32_t {
                return self.type_plugin->get_instruction_index_from_member_index(
                        member_index);
            },
            py::arg("member_index"),
            py::call_guard<py::gil_scoped_release>());

    cls.def("set_allowed_data_representation_mask",
        [](const TypePlugin& self, int32_t mask) {
            self.type->native()._data._annotations._allowedDataRepresentationMask = mask;
        },
        py::arg("mask"));
}

template<>
void init_dds_typed_topic_template(IdlTopicPyClass &cls)
{
    // First initialize the "base" class, i.e. the python methods that are
    // common to all topics
    init_dds_typed_topic_base_template(cls);

    // Then add new methods that are only available for IDL topics
    cls.def(py::init([](PyDomainParticipant& participant,
                             const std::string& topic_name,
                             py::object& type,
                             const dds::topic::qos::TopicQos* qos,
                             PyTopicListenerPtr<CSampleWrapper> listener,
                             const dds::core::status::StatusMask& mask,
                             const char* type_name) {
                return create_idl_py_topic(
                    participant,
                    topic_name,
                    type,
                    type_name,
                    qos,
                    listener,
                    mask);
            }),
            py::arg("participant"),
            py::arg("topic_name"),
            py::arg("type"),
            py::arg("qos") = py::none(),
            py::arg("listener") = py::none(),
            py::arg_v(
                    "mask",
                    dds::core::status::StatusMask::all(),
                    "StatusMask.ALL"),
            py::arg("type_name") = py::none(),
            "Create a Topic for an @idl.struct or @idl.union type.\n\n The "
            "participant, topic_name and type arguments are required. The "
            "type_name, qos, listener, mask arguments are optional. The "
            "type_name argument can be used to register the type with a name "
            "different from the class name.");

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
                    "UserDataSample",
                    "An alias for a Python ``object`` representing a data "
                    "sample of an IDL type");

            sample_wrapper_cls.def("_get_ptr", [](CSampleWrapper& self) {
                return reinterpret_cast<size_t>(self.sample());
            });

            IdlTopicListenerPyClass tl(module, "TopicListener");
            IdlNoOpTopicListenerPyClass notl(module, "NoOpTopicListener");
            IdlITopicDescriptionPyClass itd(module, "ITopicDescription");
            IdlTopicDescriptionPyClass td(module, "TopicDescription");
            IdlTopicPyClass t(module, "Topic");
            IdlContentFilteredTopicPyClass cft(module, "ContentFilteredTopic");

            pyrti::bind_vector<PyTopic<CSampleWrapper>>(module, "TopicSeq");
            py::implicitly_convertible<
                    py::iterable,
                    std::vector<PyTopic<CSampleWrapper>>>();
            init_topic<CSampleWrapper>(itd, td, t);
            init_topic_listener<CSampleWrapper>(tl, notl);

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

            IdlDataWriterListenerPyClass dwl(module, "DataWriterListener");
            IdlNoOpDataWriterListenerPyClass nodwl(
                    module,
                    "NoOpDataWriterListener");
            init_datawriter_listener(dwl, nodwl);

            init_datawriter(dw);

            IdlDataReaderPyClass dr(module, "DataReader");

            pyrti::bind_vector<PyDataReader<CSampleWrapper>>(
                    module,
                    "DataReaderSeq");
            py::implicitly_convertible<
                    py::iterable,
                    std::vector<PyDataReader<CSampleWrapper>>>();

            IdlDataReaderListenerPyClass drl(module, "DataReaderListener");
            IdlNoOpDataReaderListenerPyClass nodrl(
                    module,
                    "NoOpDataReaderListener");

            init_datareader<CSampleWrapper>(dr);
            init_datareader_listener(drl, nodrl);

        });
    });
}

} // namespace pyrti