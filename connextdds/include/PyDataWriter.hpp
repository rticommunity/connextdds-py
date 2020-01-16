#pragma once

#include "PyConnext.hpp"
#include <pybind11/stl_bind.h>
#include <dds/pub/DataWriter.hpp>
#include <dds/pub/discovery.hpp>
//#include <dds/topic/TopicInstance.hpp>
#include <dds/pub/find.hpp>
#include "PyEntity.hpp"
#include "PyAnyDataWriter.hpp"
#include "PyDynamicTypeMap.hpp"
#include "PyTopic.hpp"
#include "PyDataWriterListener.hpp"

namespace pyrti {
    template<typename T>
    class PyDataWriterListener;

    template<typename T>
    class PyDataWriter :    public dds::pub::DataWriter<T>,
                            public pyrti::PyIAnyDataWriter, 
                            public pyrti::PyIEntity {
    public:
        using dds::pub::DataWriter<T>::DataWriter;

        /*PyDataWriter(const pyrti::PyPublisher& p, const pyrti::PyTopic<T>& t) : dds::pub::DataWriter<T>(p, t) {}

        PyDataWriter(
            const pyrti::PyPublisher& p,
            const pyrti::PyTopic<T>& t,
            const dds::pub::qos::DataWriterQos& q,
            pyrti::PyDataWriterListener<T>* l,
            const dds::core::status::StatusMask& m
        ) : dds::pub::DataWriter<T>(p, t, q, l, m) {} */

        dds::core::Entity get_entity() override {
            return dds::core::Entity(*this);
        }

        dds::pub::AnyDataWriter get_any_datawriter() const override {
            return dds::pub::AnyDataWriter(*this);
        }

        void py_enable() override { this->enable(); }

        const dds::core::status::StatusMask py_status_changes() override { return this->status_changes(); }

        const dds::core::InstanceHandle py_instance_handle() const override { return this->instance_handle(); }

        dds::pub::qos::DataWriterQos py_qos() const override { return this->qos(); }

        void py_qos(const dds::pub::qos::DataWriterQos& q) override { return this->qos(q); }

        const std::string& py_topic_name() const override { return this->topic().name(); }

        const std::string& py_type_name() const override { return this->topic().type_name(); }

        const pyrti::PyPublisher py_publisher() const override {
            auto s = this->publisher();
            return pyrti::PyPublisher(s);
        }

        void py_wait_for_acknowledgments(const dds::core::Duration& d) override { this->wait_for_acknowledgments(d); }

        void py_close() override { this->close(); }

        void py_retain() override { this->retain(); }
    };

    template<typename T>
    void init_dds_typed_datawriter_base_template(py::class_<pyrti::PyDataWriter<T>, pyrti::PyIEntity, pyrti::PyIAnyDataWriter>& cls) {
        cls
            .def(
                py::init<
                    const pyrti::PyPublisher&,
                    const pyrti::PyTopic<T>&>(),
                py::arg("pub"),
                py::arg("topic"),
                "Creates a DataWriter."
            )
            .def(
                py::init<
                    const pyrti::PyPublisher&,
                    const pyrti::PyTopic<T>&,
                    const dds::pub::qos::DataWriterQos&,
                    pyrti::PyDataWriterListener<T>*, 
                    const dds::core::status::StatusMask&>(),
                py::arg("pub"),
                py::arg("topic"),
                py::arg("qos"),
                py::arg("listener") = (pyrti::PyDataWriterListener<T>*) nullptr,
                py::arg_v("mask", dds::core::status::StatusMask::all(), "StatusMask.all()"),
                py::keep_alive<1,5>(),
                "Creates a DataWriter with QoS and a listener."
            )
            .def(
                py::init([](pyrti::PyIAnyDataWriter& adw) {
                    return pyrti::PyDataWriter<T>(adw.get_any_datawriter().get<T>());
                }),
                py::arg("writer"),
                "Create a typed DataWriter from an AnyDataWriter."
            )
            .def(
                py::init([](pyrti::PyIEntity& e) {
                    auto entity = e.get_entity();
                    return dds::core::polymorphic_cast<pyrti::PyDataWriter<T>>(entity);
                }),
                py::arg("entity"),
                "Create a typed DataWriter from an Entity."
            )
            .def(
                "write",
                (void (pyrti::PyDataWriter<T>::*)(const T&)) &pyrti::PyDataWriter<T>::write,
                py::arg("sample"),
                "Write a sample.")
            .def(
                "write_timestamped",
                (void (pyrti::PyDataWriter<T>::*)(const T&, const dds::core::Time&)) &pyrti::PyDataWriter<T>::write,
                py::arg("sample"),
                py::arg("timestamp"),
                "Write a sample with a specified timestamp."
            )
            .def(
                "write",
                (void (pyrti::PyDataWriter<T>::*)(const T&, const dds::core::InstanceHandle&)) &pyrti::PyDataWriter<T>::write,
                py::arg("sample"),
                py::arg("handle"),
                "Write a sample with an instance handle."
            )
            .def(
                "write",
                (void (pyrti::PyDataWriter<T>::*)(const T&, const dds::core::InstanceHandle&, const dds::core::Time&)) &pyrti::PyDataWriter<T>::write,
                py::arg("sample"),
                py::arg("handle"),
                py::arg("timestamp"),
                "Write a sample with an instance handle and specified timestamp."
            )
            /*.def(
                "write",
                (void (pyrti::PyDataWriter<T>::*)(const dds::topic::TopicInstance<T>&)) &pyrti::PyDataWriter<T>::write,
                py::arg("topic_instance"),
                "Write a sample given a topic instance."
            )
            .def(
                "write",
                (void (pyrti::PyDataWriter<T>::*)(const dds::topic::TopicInstance<T>&, const dds::core::Time&)) &pyrti::PyDataWriter<T>::write,
                py::arg("topic_instance"),
                py::arg("timestamp"),
                "Write a sample given a topic instance and specified timestamp."
            )*/
            .def(
                "write",
                [](pyrti::PyDataWriter<T>& writer, const std::vector<T>& values) {
                    writer.write(values.begin(), values.end());
                },
                py::arg("samples"),
                "Write a sequence of samples."
            )
            /*.def(
                "write",
                [](pyrti::PyDataWriter<T>& writer, const std::vector<dds::topic::TopicInstance<T>>& values) {
                    for (auto ti : values) {
                        writer.write(ti);
                    }
                },
                py::arg("topic_instances"),
                "Write a sequence of topic instances."
            )*/
            .def(
                "write",
                [](pyrti::PyDataWriter<T>& writer, const std::vector<T>& values, const dds::core::Time& timestamp) {
                    writer.write(values.begin(), values.end(), timestamp);
                },
                py::arg("samples"),
                py::arg("timestamp"),
                "Write a sequence of samples with a timestamp."
            )
            /*.def(
                "write",
                [](pyrti::PyDataWriter<T>& writer, const std::vector<dds::topic::TopicInstance<T>>& values, const dds::core::Time& timestamp) {
                    for (auto ti : values) {
                        writer.write(ti, timestamp);
                    }
                },
                py::arg("topic_instances"),
                py::arg("timestamp"),
                "Write a sequence of topic instances with a timestamp."
            )*/
            .def(
                "write",
                [](pyrti::PyDataWriter<T>& writer, const std::vector<T>& values, const std::vector<dds::core::InstanceHandle>& handles) {
                    writer.write(values.begin(), values.end(), handles.begin(), handles.end());
                },
                py::arg("samples"),
                py::arg("handles"),
                "Write a sequence of samples with their instance handles."
            )
            .def(
                "write",
                [](pyrti::PyDataWriter<T>& writer, const std::vector<T>& values, const std::vector<dds::core::InstanceHandle>& handles, const dds::core::Time& timestamp) {
                    writer.write(values.begin(), values.end(), handles.begin(), handles.end(), timestamp);
                },
                py::arg("samples"),
                py::arg("handles"),
                py::arg("timestamp"),
                "Write a sequence of samples with their instance handles and a timestamp."
            )
            .def(
                "__lshift__",
                [](pyrti::PyDataWriter<T>& dw, const T& data) {
                    dw << data;
                    return dw;
                },
                py::is_operator(),
                "Writes a sample."
            )
            /*.def(
                "__lshift__",
                [](pyrti::PyDataWriter<T>& dw, const dds::topic::TopicInstance<T>& ti) {
                    dw.write(ti);
                    return dw;
                },
                py::is_operator(),
                "Writes a topic instance."
            )*/
            .def(
                "__lshift__",
                [](pyrti::PyDataWriter<T>& dw, const std::pair<T, dds::core::Time>& data) {
                    dw << data;
                    return dw;
                },
                py::is_operator(),
                "Writes a paired sample with a timestamp."
            )
            /*.def(
                "__lshift__",
                [](pyrti::PyDataWriter<T>& dw, const std::pair<dds::topic::TopicInstance<T>, dds::core::Time>& p) {
                    dw.write(p.first, p.second);
                    return dw;
                },
                py::is_operator(),
                "Writes a paired sample with a timestamp."
            )*/
            .def(
                "__lshift__",
                [](pyrti::PyDataWriter<T>& dw, const std::pair<T, dds::core::InstanceHandle>& data) {
                    dw << data;
                    return dw;
                },
                py::is_operator(),
                "Writes a paired sample with an instance handle."
            )
            .def(
                "__lshift__",
                [](pyrti::PyDataWriter<T>& writer, const std::vector<T>& data) {
                    writer.write(data.begin(), data.end());
                    return writer;
                },
                py::is_operator(),
                "Writes a sequence of samples."
            )
            /*.def(
                "__lshift__",
                [](pyrti::PyDataWriter<T>& writer, const std::vector<dds::topic::TopicInstance<T>>& data) {
                    for (auto ti : data) {
                        writer.write(ti);
                    }
                    return writer;
                },
                py::is_operator(),
                "Writes a sequence of topic instances."
            )*/
            .def(
                "__lshift__",
                [](pyrti::PyDataWriter<T>& writer, const std::vector<std::pair<T, dds::core::Time>>& data) {
                    for (const std::pair<T, dds::core::Time>& d : data) {
                        writer << d;
                    }
                    return writer;
                },
                py::is_operator(),
                "Writes a sequence of pairs of samples with timestamps."
            )
            /*.def(
                "__lshift__",
                [](pyrti::PyDataWriter<T>& writer, const std::vector<std::pair<dds::topic::TopicInstance<T>, dds::core::Time>>& data) {
                    for (const std::pair<dds::topic::TopicInstance<T>, dds::core::Time>& d : data) {
                        writer.write(d.first, d.second);
                    }
                    return writer;
                },
                py::is_operator(),
                "Writes a sequence of pairs of topic instances with timestamps."
            )*/
            .def(
                "register_instance",
                (const dds::core::InstanceHandle (pyrti::PyDataWriter<T>::*)(const T&)) &pyrti::PyDataWriter<T>::register_instance,
                py::arg("handle"),
                "Informs RTI Connext that the application will be modifying a particular instance."
            )
            .def(
                "register_instance",
                (const dds::core::InstanceHandle (pyrti::PyDataWriter<T>::*)(const T&, const dds::core::Time&)) &pyrti::PyDataWriter<T>::register_instance,
                py::arg("handle"),
                py::arg("timestamp"),
                "Informs RTI Connext that the application will be modifying a particular instance and specified the timestamp."
            )
            .def(
                "unregister_instance",
                [](pyrti::PyDataWriter<T>& dw, const dds::core::InstanceHandle& h) {
                    dw.unregister_instance(h);
                    return dw;
                },
                py::arg("handle"),
                "Unregister an instance."
            )
            .def(
                "unregister_instance",
                [](pyrti::PyDataWriter<T>& dw, const dds::core::InstanceHandle& h, const dds::core::Time& t) {
                    dw.unregister_instance(h, t);
                    return dw;
                },
                py::arg("handle"),
                py::arg("timestamp"),
                "Unregister an instance with timestamp."
            )
            .def(
                "dispose_instance",
                [](pyrti::PyDataWriter<T>& dw, const dds::core::InstanceHandle& h) {
                    dw.dispose_instance(h);
                    return dw;
                },
                py::arg("handle"),
                "Dispose an instance."
            )
            .def(
                "dispose_instance",
                [](pyrti::PyDataWriter<T>& dw, const dds::core::InstanceHandle& h, const dds::core::Time& t) {
                    dw.dispose_instance(h, t);
                    return dw;
                },
                py::arg("handle"),
                py::arg("timestamp"),
                "Dispose an instance with a timestamp"
            )
            .def(
                "key_value",
                (T& (pyrti::PyDataWriter<T>::*)(T&, const dds::core::InstanceHandle&)) &pyrti::PyDataWriter<T>::key_value,
                py::arg("key_holder"),
                py::arg("handle"),
                "Retrieve the instance key that corresponds to an instance handle."
            )
            /*.def(
                "key_value",
                (dds::topic::TopicInstance<T>& (pyrti::PyDataWriter<T>::*)(dds::topic::TopicInstance<T>&, const dds::core::InstanceHandle&)) &pyrti::PyDataWriter<T>::key_value,
                py::arg("topic_instance"),
                py::arg("handle"),
                "Retrieve the instance key that corresponds to an instance handle."
            )*/
            .def(
                "lookup_instance",
                &pyrti::PyDataWriter<T>::lookup_instance,
                py::arg("key_holder"),
                "Retrieve the instance handle that corresponds to an instance key_holder"
            )
            .def_property(
                "qos",
                (dds::pub::qos::DataWriterQos (pyrti::PyDataWriter<T>::*)() const) &pyrti::PyDataWriter<T>::qos,
                (void (pyrti::PyDataWriter<T>::*)(const dds::pub::qos::DataWriterQos&)) &pyrti::PyDataWriter<T>::qos,
                "Get a copy of or set the DataWriterQos."
            )
            .def(
                "__lshift__",
                [](pyrti::PyDataWriter<T>& dw, const dds::pub::qos::DataWriterQos& q) {
                    dw << q;
                    return dw;
                },
                py::is_operator(),
                "Sets the DataWriterQos."
            )
            .def(
                "__rshift__",
                [](const pyrti::PyDataWriter<T>& dw, dds::pub::qos::DataWriterQos& q) {
                    dw >> q;
                    return dw;
                },
                py::is_operator(),
                "Get the DataWriterQos."
            )
            .def_property_readonly(
                "topic",
                [](const pyrti::PyDataWriter<T>& dw) {
                    dds::topic::Topic<T> t = dw.topic();
                    return pyrti::PyTopic<T>(t);
                },
                "Get the Topic object associated with this DataWriter."
            )
            .def_property_readonly(
                "publisher",
                &pyrti::PyDataWriter<T>::py_publisher,
                "Get the Publisher that owns this DataWriter."
            )
            .def(
                "wait_for_acknowledgments",
                &pyrti::PyDataWriter<T>::py_wait_for_acknowledgments,
                py::arg("max_wait"),
                "Blocks the calling thread until all data written by a realiable DataWriter is acknowledged or until the timeout expires."
            )
            .def_property_readonly(
                "listener",
                [](pyrti::PyDataWriter<T>& dw) {
                    return dynamic_cast<pyrti::PyDataWriterListener<T>*>(dw.listener());
                },
                "Get the listener associated with the DataWriter or set the listener and status mask as a tuple."
            )
            .def(
                "bind_listener",
                [](pyrti::PyDataWriter<T>& dw, pyrti::PyDataWriterListener<T>* l, const dds::core::status::StatusMask& m) {
                    dw.listener(l, m);
                },
                py::arg("listener"),
                py::arg("event_mask"),
                "Set the listener and event mask for the DataWriter."
            )
            .def_property_readonly(
                "liveliness_lost_status",
                &pyrti::PyDataWriter<T>::liveliness_lost_status,
                "Get a copy of the LivelinessLostStatus."
            )
            .def_property_readonly(
                "offered_deadline_missed_status",
                &pyrti::PyDataWriter<T>::offered_deadline_missed_status,
                "Get a copy of the OfferedDeadlineMissedStatus."
            )
            .def_property_readonly(
                "offered_incompatible_qos_status",
                &pyrti::PyDataWriter<T>::offered_incompatible_qos_status,
                "Get a copy of the OfferedIncompatibleQosStatus"
            )
            .def_property_readonly(
                "publication_matched_status",
                &pyrti::PyDataWriter<T>::publication_matched_status,
                "Get a copy of the PublicationMatchedStatus"
            )
            .def(
                "assert_liveliness",
                &pyrti::PyDataWriter<T>::assert_liveliness,
                "Manually asserts the liveliness of the DataWriter."
            )
            .def(
                "unregister_instance",
                [](pyrti::PyDataWriter<T>& writer, rti::pub::WriteParams& params) {
                    writer->unregister_instance(params);
                },
                py::arg("params"),
                "Unregister an instance with parameters."
            )
            .def(
                "dispose_instance",
                [](pyrti::PyDataWriter<T>& writer, rti::pub::WriteParams& params) {
                    writer->dispose_instance(params);
                },
                py::arg("params"),
                "Dispose an instance with params."
            )
            .def(
                "is_sample_app_acknowledged",
                [](pyrti::PyDataWriter<T>& writer, const rti::core::SampleIdentity& sample_id) {
                    return writer->is_sample_app_acknowledged(sample_id);
                },
                py::arg("sample_id"),
                "Indicates if a sample is considered application-acknowledged."
            )
            .def(
                "wait_for_asynchronous_publishing",
                [](pyrti::PyDataWriter<T>& writer, const dds::core::Duration& max_wait) {
                    writer->wait_for_asynchronous_publishing(max_wait);
                },
                py::arg("max_wait"),
                "This operation blocks the calling thread (up to max_wait) until all data written by the asynchronous dds::pub::DataWriter is sent and acknowledged (if reliable) by all matched DataReader entities. A successful completion indicates that all the samples written have been sent and acknowledged where applicable; a time out indicates that max_wait elapsed before all the data was sent and/or acknowledged.\n"
                "In other words, this guarantees that sending to best effort dds::sub::DataReader is complete in addition to what dds::pub::DataWriter::wait_for_acknowledgments provides.\n"
                "If the dds::pub::DataWriter does not have rti::core::policy::PublishMode kind set to rti::core::policy::PublishModeKind_def::ASYNCHRONOUS the operation will complete immediately"
            )
            .def_property_readonly(
                "reliable_writer_cache_changed_status",
                [](pyrti::PyDataWriter<T>& writer) {
                    return writer->reliable_writer_cache_changed_status();
                },
                "Get a copy of the reliable cache status for this writer."
            )
            .def_property_readonly(
                "reliable_reader_activity_changed_status",
                [](pyrti::PyDataWriter<T>& writer) {
                    return writer->reliable_reader_activity_changed_status();
                },
                "Get a copy of the reliable reader activity changed status for this writer."
            )
            .def_property_readonly(
                "datawriter_cache_status",
                [](pyrti::PyDataWriter<T>& writer) {
                    return writer->datawriter_cache_status();
                },
                "Get a copy of the cache status for this writer."
            )
            .def_property_readonly(
                "datawriter_protocol_status",
                [](pyrti::PyDataWriter<T>& writer) {
                    return writer->datawriter_protocol_status();
                },
                "Get a copy of the protocol status for this writer."
            )
            .def(
                "matched_subscription_datawriter_procotol_status",
                [](pyrti::PyDataWriter<T>& writer, const dds::core::InstanceHandle& subscription_handle) {
                    return writer->matched_subscription_datawriter_protocol_status(subscription_handle);
                },
                py::arg("handle"),
                "Get a copy of the protocol status for this writer per a matched subscription handle."
            )
            .def(
                "matched_subscription_datawriter_procotol_status",
                [](pyrti::PyDataWriter<T>& writer, const rti::core::Locator& subscription_locator) {
                    return writer->matched_subscription_datawriter_protocol_status(subscription_locator);
                },
                py::arg("locator"),
                "Get a copy of the protocol status for this writer per a matched subscription locator."
            )
            .def_property_readonly(
                "service_request_accepted_status",
                [](pyrti::PyDataWriter<T>& writer) {
                    return writer->service_request_accepted_status();
                },
                "Get a copy of the service request accepted status for this writer."
            )
            .def(
                "flush",
                [](pyrti::PyDataWriter<T>& writer) {
                    return writer->flush();
                },
                "Flushes the batch in progress in the context of the calling thread."
            )
            .def(
                "write",
                [](pyrti::PyDataWriter<T>& writer, const T& instance_data, rti::pub::WriteParams& params) {
                    writer->write(instance_data, params);
                },
                py::arg("sample"),
                py::arg("params"),
                "Write with advanced parameters."
            )
            /*.def(
                "delete_data",
                [](DataWriter<T>& writer, T* sample) {
                    return writer->delete_data(sample);
                },
                py::arg("data"),
                "Delete data of the writer's associated type."
            ) */
            .def(
                "register_instance",
                [](pyrti::PyDataWriter<T>& writer, const T& key, rti::pub::WriteParams& params) {
                    writer->register_instance(key, params);
                },
                py::arg("key_holder"),
                py::arg("params"),
                "Registers instance with parameters."
            )
            .def_property_readonly(
                "matched_subscriptions",
                [](const pyrti::PyDataWriter<T>& dw) {
                    return dds::pub::matched_subscriptions(dw);
                },
                "Get a copy of the list of the currently matched subscription handles."
            )
            .def(
                "matched_subscription_data",
                [](const pyrti::PyDataWriter<T>& dw, const dds::core::InstanceHandle& h) {
                    return dds::pub::matched_subscription_data<T>(dw, h);
                },
                py::arg("handle"),
                "Get the SubscriptionBuiltinTopicData for a subscription matched to this DataWriter."
            )
            .def_static(
                "find_all_by_topic",
                [](pyrti::PyPublisher& pub, const std::string& topic_name) {
                    std::vector<pyrti::PyDataWriter<T>> v;
                    dds::pub::find<pyrti::PyDataWriter<T>>(pub, topic_name, std::back_inserter(v));
                    return v;
                },
                py::arg("publisher"),
                py::arg("topic_name"),
                "Retrieve all DataWriters for the given topic name in the publisher."
            )
            .def_static(
                "find_by_name",
                [](pyrti::PyPublisher& pub, const std::string& name) -> py::object {
                    auto dw = rti::pub::find_datawriter_by_name<pyrti::PyDataWriter<T>>(pub, name);
                    return (dw == dds::core::null) ? py::cast(nullptr) : py::cast(dw);
                },
                py::arg("publisher"),
                py::arg("name"),
                "Find DataWriter in Publisher with the DataReader's name, "
                "returning the first found."
            )
            .def_static(
                "find_by_name",
                [](pyrti::PyDomainParticipant& dp, const std::string name) ->py::object {
                    auto dw = rti::pub::find_datawriter_by_name<pyrti::PyDataWriter<T>>(dp, name);
                    return (dw == dds::core::null) ? py::cast(nullptr) : py::cast(dw);
                },
                py::arg("participant"),
                py::arg("name"),
                "Find DataWriter in DomainParticipant with the provided name, "
                "returning the first found."
            )
            .def_static(
                "find_by_topic",
                [](pyrti::PyPublisher& pub, const std::string& topic_name) -> py::object {
                    auto dw = rti::pub::find_datawriter_by_topic_name<pyrti::PyDataWriter<T>>(pub, topic_name);
                    return (dw == dds::core::null) ? py::cast(nullptr) : py::cast(dw);
                },
                py::arg("publisher"),
                py::arg("name"),
                "Find DataWriter in publisher with a topic name, "
                "returning the first found."
            )
            .def(
                py::self == py::self,
                "Test for equality."
            )
            .def(
                py::self != py::self,
                "Test for inequality."
            );

        py::implicitly_convertible<pyrti::PyIAnyDataWriter, pyrti::PyDataWriter<T>>();
        py::implicitly_convertible<pyrti::PyIEntity, pyrti::PyDataWriter<T>>();

        py::bind_vector<std::vector<pyrti::PyDataWriter<T>>>(cls, "Seq");
        py::implicitly_convertible<py::iterable, std::vector<pyrti::PyDataWriter<T>>>();
    }

    template<typename T>
    void init_dds_typed_datawriter_template(py::class_<pyrti::PyDataWriter<T>, pyrti::PyIEntity, pyrti::PyIAnyDataWriter>& cls) {
        init_dds_typed_datawriter_base_template<T>(cls);
        cls
            .def(
                "create_data",
                [](dds::pub::DataWriter<T>& dw) {
                    return std::unique_ptr<T>(dw->create_data());
                },
                "Create data of the writer's associated type and initialize it."
            )
            .def(
                "key_value",
                [](dds::pub::DataWriter<T>& dw, const dds::core::InstanceHandle& handle) {
                    auto d = std::unique_ptr<T>(dw->create_data());
                    dw.key_value(*d, handle);
                    return d;
                },
                py::arg("handle"),
                "Retrieve the instance key that corresponds to an instance handle."
            );
            /*.def(
                "topic_instance_key_value",
                [](dds::pub::DataWriter<T>& dw, const dds::core::InstanceHandle& handle) {
                    auto d = std::unique_ptr<T>(dw->create_data());
                    dds::topic::TopicInstance<T> ti(handle, *d);
                    dw.key_value(ti, handle);
                    return ti;
                },
                py::arg("handle"),
                "Retrieve the instance key that corresponds to an instance handle."
            );*/
    }

    template<typename T>
    void init_datawriter(py::object& o) {
        py::class_<pyrti::PyDataWriter<T>, pyrti::PyIEntity, pyrti::PyIAnyDataWriter> dw(o, "DataWriter");

        init_dds_typed_datawriter_template(dw);
    }
}
