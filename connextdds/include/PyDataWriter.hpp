#pragma once

#include "PyConnext.hpp"
#include <pybind11/stl_bind.h>
#include <pybind11/functional.h>
#include <dds/pub/DataWriter.hpp>
#include <dds/pub/discovery.hpp>
#include <dds/topic/TopicInstance.hpp>
#include <dds/pub/find.hpp>
#include "PyEntity.hpp"
#include "PyAnyDataWriter.hpp"
#include "PyDynamicTypeMap.hpp"
#include "PyTopic.hpp"
#include "PyDataWriterListener.hpp"
#include "PyAsyncioExecutor.hpp"


namespace pyrti {

template<typename T>
class PyDataWriterListener;

template<typename T>
class PyDataWriter :    public dds::pub::DataWriter<T>,
                        public PyIAnyDataWriter, 
                        public PyIEntity {
public:
    using dds::pub::DataWriter<T>::DataWriter;

    PyDataWriter(
        const PyPublisher& p,
        const PyTopic<T>& t,
        const dds::pub::qos::DataWriterQos& q,
        PyDataWriterListener<T>* l,
        const dds::core::status::StatusMask& m
    ) : dds::pub::DataWriter<T>(p, t, q, l, m) {
        if (nullptr != l) py::cast(l).inc_ref();
    }

    virtual
    ~PyDataWriter() {
        if (this->delegate().use_count() <= 2 && !this->delegate()->closed() && nullptr != this->listener()) {
            py::object listener = py::cast(this->listener());
            this->listener(nullptr, dds::core::status::StatusMask::none());
            listener.dec_ref();
        }
    }

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

    const PyPublisher py_publisher() const override {
        auto s = this->publisher();
        return PyPublisher(s);
    }

    void py_wait_for_acknowledgments(const dds::core::Duration& d) override { this->wait_for_acknowledgments(d); }

    void py_close() override {
        if (nullptr != this->listener()) {
            py::object listener = py::cast(this->listener());
            this->listener(nullptr, dds::core::status::StatusMask::none());
            listener.dec_ref();
        }
        this->close();
    }

    void py_retain() override { this->retain(); }

    bool py_closed() override { return this->delegate()->closed(); }

    bool py_enabled() override { return this->delegate()->enabled(); }

    int py_use_count() override { return this->delegate().use_count(); }

    void py_unretain() override { this->delegate()->unretain(); }
};

template<typename T>
void init_dds_typed_datawriter_base_template(py::class_<PyDataWriter<T>, PyIEntity, PyIAnyDataWriter>& cls) {
    cls
        .def(
            py::init<
                const PyPublisher&,
                const PyTopic<T>&>(),
            py::arg("pub"),
            py::arg("topic"),
            py::call_guard<py::gil_scoped_release>(),
            "Creates a DataWriter."
        )
        .def(
            py::init(
                [](
                    const PyPublisher& p,
                    const PyTopic<T>& t,
                    const dds::pub::qos::DataWriterQos& q,
                    dds::core::optional<PyDataWriterListener<T>*> l, 
                    const dds::core::status::StatusMask& m
                ) {
                    auto listener = has_value(l) ? get_value(l) : nullptr;
                    return PyDataWriter<T>(p, t, q, listener, m);
                }
            ),
            py::arg("pub"),
            py::arg("topic"),
            py::arg("qos"),
            py::arg("listener") = py::none(),
            py::arg_v("mask", dds::core::status::StatusMask::all(), "StatusMask.all()"),
            py::call_guard<py::gil_scoped_release>(),
            "Creates a DataWriter with QoS and a listener."
        )
        .def(
            py::init([](PyIAnyDataWriter& adw) {
                return PyDataWriter<T>(adw.get_any_datawriter().get<T>());
            }),
            py::arg("writer"),
            py::call_guard<py::gil_scoped_release>(),
            "Create a typed DataWriter from an AnyDataWriter."
        )
        .def(
            py::init([](PyIEntity& e) {
                auto entity = e.get_entity();
                return dds::core::polymorphic_cast<PyDataWriter<T>>(entity);
            }),
            py::arg("entity"),
            py::call_guard<py::gil_scoped_release>(),
            "Create a typed DataWriter from an Entity."
        )
        .def(
            "write",
            (void (PyDataWriter<T>::*)(const T&)) &PyDataWriter<T>::write,
            py::arg("sample"),
            py::call_guard<py::gil_scoped_release>(),
            "Write a sample.")
        .def(
            "write",
            (void (PyDataWriter<T>::*)(const T&, const dds::core::Time&)) &PyDataWriter<T>::write,
            py::arg("sample"),
            py::arg("timestamp"),
            py::call_guard<py::gil_scoped_release>(),
            "Write a sample with a specified timestamp."
        )
        .def(
            "write",
            (void (PyDataWriter<T>::*)(const T&, const dds::core::InstanceHandle&)) &PyDataWriter<T>::write,
            py::arg("sample"),
            py::arg("handle"),
            py::call_guard<py::gil_scoped_release>(),
            "Write a sample with an instance handle."
        )
        .def(
            "write",
            (void (PyDataWriter<T>::*)(const T&, const dds::core::InstanceHandle&, const dds::core::Time&)) &PyDataWriter<T>::write,
            py::arg("sample"),
            py::arg("handle"),
            py::arg("timestamp"),
            py::call_guard<py::gil_scoped_release>(),
            "Write a sample with an instance handle and specified timestamp."
        )
        .def(
            "write",
            (void (PyDataWriter<T>::*)(const dds::topic::TopicInstance<T>&)) &PyDataWriter<T>::write,
            py::arg("topic_instance"),
            py::call_guard<py::gil_scoped_release>(),
            "Write a sample given a topic instance."
        )
        .def(
            "write",
            (void (PyDataWriter<T>::*)(const dds::topic::TopicInstance<T>&, const dds::core::Time&)) &PyDataWriter<T>::write,
            py::arg("topic_instance"),
            py::arg("timestamp"),
            py::call_guard<py::gil_scoped_release>(),
            "Write a sample given a topic instance and specified timestamp."
        )
        .def(
            "write",
            [](PyDataWriter<T>& writer, const std::vector<T>& values) {
                writer.write(values.begin(), values.end());
            },
            py::arg("samples"),
            py::call_guard<py::gil_scoped_release>(),
            "Write a sequence of samples."
        )
        .def(
            "write",
            [](PyDataWriter<T>& writer, const std::vector<dds::topic::TopicInstance<T>>& values) {
                for (auto ti : values) {
                    writer.write(ti);
                }
            },
            py::arg("topic_instances"),
            py::call_guard<py::gil_scoped_release>(),
            "Write a sequence of topic instances."
        )
        .def(
            "write",
            [](PyDataWriter<T>& writer, const std::vector<T>& values, const dds::core::Time& timestamp) {
                writer.write(values.begin(), values.end(), timestamp);
            },
            py::arg("samples"),
            py::arg("timestamp"),
            py::call_guard<py::gil_scoped_release>(),
            "Write a sequence of samples with a timestamp."
        )
        .def(
            "write",
            [](PyDataWriter<T>& writer, const std::vector<dds::topic::TopicInstance<T>>& values, const dds::core::Time& timestamp) {
                for (auto ti : values) {
                    writer.write(ti, timestamp);
                }
            },
            py::arg("topic_instances"),
            py::arg("timestamp"),
            py::call_guard<py::gil_scoped_release>(),
            "Write a sequence of topic instances with a timestamp."
        )
        .def(
            "write",
            [](PyDataWriter<T>& writer, const std::vector<T>& values, const std::vector<dds::core::InstanceHandle>& handles) {
                writer.write(values.begin(), values.end(), handles.begin(), handles.end());
            },
            py::arg("samples"),
            py::arg("handles"),
            py::call_guard<py::gil_scoped_release>(),
            "Write a sequence of samples with their instance handles."
        )
        .def(
            "write",
            [](PyDataWriter<T>& writer, const std::vector<T>& values, const std::vector<dds::core::InstanceHandle>& handles, const dds::core::Time& timestamp) {
                writer.write(values.begin(), values.end(), handles.begin(), handles.end(), timestamp);
            },
            py::arg("samples"),
            py::arg("handles"),
            py::arg("timestamp"),
            py::call_guard<py::gil_scoped_release>(),
            "Write a sequence of samples with their instance handles and a timestamp."
        )
        .def(
            "__lshift__",
            [](PyDataWriter<T>& dw, const T& data) {
                dw << data;
                return dw;
            },
            py::is_operator(),
            py::call_guard<py::gil_scoped_release>(),
            "Writes a sample."
        )
        .def(
            "__lshift__",
            [](PyDataWriter<T>& dw, const dds::topic::TopicInstance<T>& ti) {
                dw.write(ti);
                return dw;
            },
            py::is_operator(),
            py::call_guard<py::gil_scoped_release>(),
            "Writes a topic instance."
        )
        .def(
            "__lshift__",
            [](PyDataWriter<T>& dw, const std::pair<T, dds::core::Time>& data) {
                dw << data;
                return dw;
            },
            py::is_operator(),
            py::call_guard<py::gil_scoped_release>(),
            "Writes a paired sample with a timestamp."
        )
        .def(
            "__lshift__",
            [](PyDataWriter<T>& dw, const std::pair<dds::topic::TopicInstance<T>, dds::core::Time>& p) {
                dw.write(p.first, p.second);
                return dw;
            },
            py::is_operator(),
            py::call_guard<py::gil_scoped_release>(),
            "Writes a paired sample with a timestamp."
        )
        .def(
            "__lshift__",
            [](PyDataWriter<T>& dw, const std::pair<T, dds::core::InstanceHandle>& data) {
                dw << data;
                return dw;
            },
            py::is_operator(),
            py::call_guard<py::gil_scoped_release>(),
            "Writes a paired sample with an instance handle."
        )
        .def(
            "__lshift__",
            [](PyDataWriter<T>& writer, const std::vector<T>& data) {
                writer.write(data.begin(), data.end());
                return writer;
            },
            py::is_operator(),
            py::call_guard<py::gil_scoped_release>(),
            "Writes a sequence of samples."
        )
        .def(
            "__lshift__",
            [](PyDataWriter<T>& writer, const std::vector<dds::topic::TopicInstance<T>>& data) {
                for (auto ti : data) {
                    writer.write(ti);
                }
                return writer;
            },
            py::is_operator(),
            py::call_guard<py::gil_scoped_release>(),
            "Writes a sequence of topic instances."
        )
        .def(
            "__lshift__",
            [](PyDataWriter<T>& writer, const std::vector<std::pair<T, dds::core::Time>>& data) {
                for (const std::pair<T, dds::core::Time>& d : data) {
                    writer << d;
                }
                return writer;
            },
            py::is_operator(),
            py::call_guard<py::gil_scoped_release>(),
            "Writes a sequence of pairs of samples with timestamps."
        )
        .def(
            "__lshift__",
            [](PyDataWriter<T>& writer, const std::vector<std::pair<dds::topic::TopicInstance<T>, dds::core::Time>>& data) {
                for (const std::pair<dds::topic::TopicInstance<T>, dds::core::Time>& d : data) {
                    writer.write(d.first, d.second);
                }
                return writer;
            },
            py::is_operator(),
            py::call_guard<py::gil_scoped_release>(),
            "Writes a sequence of pairs of topic instances with timestamps."
        )
        .def(
            "register_instance",
            (const dds::core::InstanceHandle (PyDataWriter<T>::*)(const T&)) &PyDataWriter<T>::register_instance,
            py::arg("handle"),
            "Informs RTI Connext that the application will be modifying a particular instance."
        )
        .def(
            "register_instance",
            (const dds::core::InstanceHandle (PyDataWriter<T>::*)(const T&, const dds::core::Time&)) &PyDataWriter<T>::register_instance,
            py::arg("handle"),
            py::arg("timestamp"),
            "Informs RTI Connext that the application will be modifying a particular instance and specified the timestamp."
        )
        .def(
            "unregister_instance",
            [](PyDataWriter<T>& dw, const dds::core::InstanceHandle& h) {
                dw.unregister_instance(h);
                return dw;
            },
            py::arg("handle"),
            "Unregister an instance."
        )
        .def(
            "unregister_instance",
            [](PyDataWriter<T>& dw, const dds::core::InstanceHandle& h, const dds::core::Time& t) {
                dw.unregister_instance(h, t);
                return dw;
            },
            py::arg("handle"),
            py::arg("timestamp"),
            "Unregister an instance with timestamp."
        )
        .def(
            "dispose_instance",
            [](PyDataWriter<T>& dw, const dds::core::InstanceHandle& h) {
                dw.dispose_instance(h);
                return dw;
            },
            py::arg("handle"),
            "Dispose an instance."
        )
        .def(
            "dispose_instance",
            [](PyDataWriter<T>& dw, const dds::core::InstanceHandle& h, const dds::core::Time& t) {
                dw.dispose_instance(h, t);
                return dw;
            },
            py::arg("handle"),
            py::arg("timestamp"),
            "Dispose an instance with a timestamp"
        )
        .def(
            "key_value",
            (T& (PyDataWriter<T>::*)(T&, const dds::core::InstanceHandle&)) &PyDataWriter<T>::key_value,
            py::arg("key_holder"),
            py::arg("handle"),
            "Retrieve the instance key that corresponds to an instance handle."
        )
        .def(
            "key_value",
            (dds::topic::TopicInstance<T>& (PyDataWriter<T>::*)(dds::topic::TopicInstance<T>&, const dds::core::InstanceHandle&)) &PyDataWriter<T>::key_value,
            py::arg("topic_instance"),
            py::arg("handle"),
            "Retrieve the instance key that corresponds to an instance handle."
        )
        .def(
            "lookup_instance",
            &PyDataWriter<T>::lookup_instance,
            py::arg("key_holder"),
            "Retrieve the instance handle that corresponds to an instance key_holder"
        )
        .def_property(
            "qos",
            (dds::pub::qos::DataWriterQos (PyDataWriter<T>::*)() const) &PyDataWriter<T>::qos,
            (void (PyDataWriter<T>::*)(const dds::pub::qos::DataWriterQos&)) &PyDataWriter<T>::qos,
            "The DataWriterQos for this DataWriter."
            "\n\n"
            "This property's getter returns a deep copy."
        )
        .def(
            "__lshift__",
            [](PyDataWriter<T>& dw, const dds::pub::qos::DataWriterQos& q) {
                dw << q;
                return dw;
            },
            py::is_operator(),
            "Sets the DataWriterQos."
        )
        .def(
            "__rshift__",
            [](const PyDataWriter<T>& dw, dds::pub::qos::DataWriterQos& q) {
                dw >> q;
                return dw;
            },
            py::is_operator(),
            "Get the DataWriterQos."
        )
        .def_property_readonly(
            "topic",
            [](const PyDataWriter<T>& dw) {
                dds::topic::Topic<T> t = dw.topic();
                return PyTopic<T>(t);
            },
            "Get the Topic object associated with this DataWriter."
        )
        .def_property_readonly(
            "type_name",
            [](const PyDataWriter<T>& dw) {
                return dw.py_type_name();
            },
            "Get the type name for the topic object associated with this DataWriter."
        )
        .def_property_readonly(
            "topic_name",
            [](const PyDataWriter<T>& dw) {
                return dw.py_topic_name();
            },
            "Get the topic name associated with this DataWriter."
        )
        .def_property_readonly(
            "publisher",
            &PyDataWriter<T>::py_publisher,
            "Get the Publisher that owns this DataWriter."
        )
        .def(
            "wait_for_acknowledgments",
            &PyDataWriter<T>::py_wait_for_acknowledgments,
            py::arg("max_wait"),
            "Blocks the calling thread until all data written by a realiable DataWriter is acknowledged or until the timeout expires."
        )
        .def_property_readonly(
            "listener",
            [](PyDataWriter<T>& dw) {
                dds::core::optional<PyDataWriterListener<T>*> l;
                auto ptr = dynamic_cast<PyDataWriterListener<T>*>(dw.listener());
                if (nullptr != ptr) l = ptr;
                return l;
            },
            "Get the listener associated with the DataWriter or set the listener and status mask as a tuple."
        )
        .def(
            "bind_listener",
            [](PyDataWriter<T>& dw, dds::core::optional<PyDataWriterListener<T>*> l, const dds::core::status::StatusMask& m) {
                auto listener = has_value(l) ? get_value(l) : nullptr;
                if (nullptr != listener) {
                    py::cast(listener).inc_ref();
                }
                if (nullptr != dw.listener()) {
                    py::cast(dw.listener()).dec_ref();
                }
                dw.listener(listener, m);
            },
            py::arg("listener"),
            py::arg("event_mask"),
            "Set the listener and event mask for the DataWriter."
        )
        .def_property_readonly(
            "liveliness_lost_status",
            &PyDataWriter<T>::liveliness_lost_status,
            "Get a copy of the LivelinessLostStatus."
        )
        .def_property_readonly(
            "offered_deadline_missed_status",
            &PyDataWriter<T>::offered_deadline_missed_status,
            "Get a copy of the OfferedDeadlineMissedStatus."
        )
        .def_property_readonly(
            "offered_incompatible_qos_status",
            &PyDataWriter<T>::offered_incompatible_qos_status,
            "Get a copy of the OfferedIncompatibleQosStatus"
        )
        .def_property_readonly(
            "publication_matched_status",
            &PyDataWriter<T>::publication_matched_status,
            "Get a copy of the PublicationMatchedStatus"
        )
        .def(
            "assert_liveliness",
            &PyDataWriter<T>::assert_liveliness,
            "Manually asserts the liveliness of the DataWriter."
        )
        .def(
            "unregister_instance",
            [](PyDataWriter<T>& writer, rti::pub::WriteParams& params) {
                writer->unregister_instance(params);
            },
            py::arg("params"),
            "Unregister an instance with parameters."
        )
        .def(
            "dispose_instance",
            [](PyDataWriter<T>& writer, rti::pub::WriteParams& params) {
                writer->dispose_instance(params);
            },
            py::arg("params"),
            "Dispose an instance with params."
        )
        .def(
            "is_sample_app_acknowledged",
            [](PyDataWriter<T>& writer, const rti::core::SampleIdentity& sample_id) {
                return writer->is_sample_app_acknowledged(sample_id);
            },
            py::arg("sample_id"),
            "Indicates if a sample is considered application-acknowledged."
        )
        .def(
            "wait_for_asynchronous_publishing",
            [](PyDataWriter<T>& writer, const dds::core::Duration& max_wait) {
                writer->wait_for_asynchronous_publishing(max_wait);
            },
            py::arg("max_wait"),
            "This operation blocks the calling thread (up to max_wait) "
            "until all data written by the asynchronous DataWriter is "
            "sent and acknowledged (if reliable) by all matched "
            "DataReader entities. A successful completion indicates that "
            "all the samples written have been sent and acknowledged "
            "where applicable; a time out indicates that max_wait elapsed "
            "before all the data was sent and/or acknowledged."
            "\n\n"
            "In other words, this guarantees that sending to best effort "
            "DataReader is complete in addition to what "
            "DataWriter.wait_for_acknowledgments() provides."
            "\n\n"
            "If the DataWriter does not have PublishMode kind set to "
            "PublishModeKind.ASYNCHRONOUS the operation will complete "
            "immediately"
        )
        .def_property_readonly(
            "reliable_writer_cache_changed_status",
            [](PyDataWriter<T>& writer) {
                return writer->reliable_writer_cache_changed_status();
            },
            "Get a copy of the reliable cache status for this writer."
        )
        .def_property_readonly(
            "reliable_reader_activity_changed_status",
            [](PyDataWriter<T>& writer) {
                return writer->reliable_reader_activity_changed_status();
            },
            "Get a copy of the reliable reader activity changed status for this writer."
        )
        .def_property_readonly(
            "datawriter_cache_status",
            [](PyDataWriter<T>& writer) {
                return writer->datawriter_cache_status();
            },
            "Get a copy of the cache status for this writer."
        )
        .def_property_readonly(
            "datawriter_protocol_status",
            [](PyDataWriter<T>& writer) {
                return writer->datawriter_protocol_status();
            },
            "Get a copy of the protocol status for this writer."
        )
        .def(
            "matched_subscription_datawriter_procotol_status",
            [](PyDataWriter<T>& writer, const dds::core::InstanceHandle& subscription_handle) {
                return writer->matched_subscription_datawriter_protocol_status(subscription_handle);
            },
            py::arg("handle"),
            "Get a copy of the protocol status for this writer per a matched subscription handle."
        )
        .def(
            "matched_subscription_datawriter_procotol_status",
            [](PyDataWriter<T>& writer, const rti::core::Locator& subscription_locator) {
                return writer->matched_subscription_datawriter_protocol_status(subscription_locator);
            },
            py::arg("locator"),
            "Get a copy of the protocol status for this writer per a matched subscription locator."
        )
        .def_property_readonly(
            "service_request_accepted_status",
            [](PyDataWriter<T>& writer) {
                return writer->service_request_accepted_status();
            },
            "Get a copy of the service request accepted status for this writer."
        )
        .def(
            "flush",
            [](PyDataWriter<T>& writer) {
                return writer->flush();
            },
            "Flushes the batch in progress in the context of the calling thread."
        )
        .def(
            "write",
            [](PyDataWriter<T>& writer, const T& instance_data, rti::pub::WriteParams& params) {
                writer->write(instance_data, params);
            },
            py::arg("sample"),
            py::arg("params"),
            "Write with advanced parameters."
        )
        .def(
            "register_instance",
            [](PyDataWriter<T>& writer, const T& key, rti::pub::WriteParams& params) {
                writer->register_instance(key, params);
            },
            py::arg("key_holder"),
            py::arg("params"),
            "Registers instance with parameters."
        )
        .def_property_readonly(
            "matched_subscriptions",
            [](const PyDataWriter<T>& dw) {
                return dds::pub::matched_subscriptions(dw);
            },
            "Get a copy of the list of the currently matched subscription handles."
        )
        .def(
            "matched_subscription_data",
            [](const PyDataWriter<T>& dw, const dds::core::InstanceHandle& h) {
                return dds::pub::matched_subscription_data<T>(dw, h);
            },
            py::arg("handle"),
            "Get the SubscriptionBuiltinTopicData for a subscription matched to this DataWriter."
        )
        .def_static(
            "find_all_by_topic",
            [](PyPublisher& pub, const std::string& topic_name) {
                std::vector<PyDataWriter<T>> v;
                dds::pub::find<PyDataWriter<T>>(pub, topic_name, std::back_inserter(v));
                return v;
            },
            py::arg("publisher"),
            py::arg("topic_name"),
            "Retrieve all DataWriters for the given topic name in the publisher."
        )
        .def_static(
            "find_by_name",
            [](PyPublisher& pub, const std::string& name) {
                dds::core::optional<PyDataWriter<T>> retval;
                auto dw = rti::pub::find_datawriter_by_name<PyDataWriter<T>>(pub, name);
                if (dw != dds::core::null) retval = dw;
                return retval;
            },
            py::arg("publisher"),
            py::arg("name"),
            "Find DataWriter in Publisher with the DataReader's name, "
            "returning the first found."
        )
        .def_static(
            "find_by_name",
            [](PyDomainParticipant& dp, const std::string name) {
                dds::core::optional<PyDataWriter<T>> retval;
                auto dw = rti::pub::find_datawriter_by_name<PyDataWriter<T>>(dp, name);
                if(dw != dds::core::null) retval = dw;
                return retval;
            },
            py::arg("participant"),
            py::arg("name"),
            "Find DataWriter in DomainParticipant with the provided name, "
            "returning the first found."
        )
        .def_static(
            "find_by_topic",
            [](PyPublisher& pub, const std::string& topic_name) {
                dds::core::optional<PyDataWriter<T>> retval;
                auto dw = rti::pub::find_datawriter_by_topic_name<PyDataWriter<T>>(pub, topic_name);
                if (dw != dds::core::null) retval = dw;
                return retval;
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
        )
        .def(
            "write_async",
            [](PyDataWriter<T>& dw, const T& sample) {
                return PyAsyncioExecutor::run<void>(
                    std::function<void()>([&dw, &sample]() {
                        dw.write(sample);
                    })
                );
            },
            py::arg("sample"),
            py::keep_alive<0, 1>(),
            py::keep_alive<0, 2>(),
            "Write a sample. This method is awaitable and is only for use "
            "with asyncio.")
        
        .def(
            "write_async",
            [](PyDataWriter<T>& dw, const T& sample, const dds::core::Time& timestamp) {
                return PyAsyncioExecutor::run<void>(
                    std::function<void()>([&dw, &sample, &timestamp]() {
                        dw.write(sample, timestamp);
                    })
                );
            },
            py::arg("sample"),
            py::arg("timestamp"),
            py::keep_alive<0, 1>(),
            py::keep_alive<0, 2>(),
            py::keep_alive<0, 3>(),
            "Write a sample with a specified timestamp. This methods is "
            "awaitable and only for use with asyncio."
        )
        .def(
            "write_async",
            [](PyDataWriter<T>& dw, const T& sample, const dds::core::InstanceHandle& handle) {
                return PyAsyncioExecutor::run<void>(
                    std::function<void()>([&dw, &sample, &handle]() {
                        dw.write(sample, handle);
                    })
                );
            },
            py::arg("sample"),
            py::arg("handle"),
            py::keep_alive<0, 1>(),
            py::keep_alive<0, 2>(),
            py::keep_alive<0, 3>(),
            "Write a sample with an instance handle. This method is awaitable "
            "and only for use with asyncio."
        )
        .def(
            "write_async",
            [](PyDataWriter<T>& dw, const T& sample, const dds::core::InstanceHandle& handle, const dds::core::Time& timestamp) {
                return PyAsyncioExecutor::run<void>(
                    std::function<void()>([&dw, &sample, &handle, &timestamp]() {
                        dw.write(sample, handle, timestamp);
                    })
                );
            },
            py::arg("sample"),
            py::arg("handle"),
            py::arg("timestamp"),
            py::keep_alive<0, 1>(),
            py::keep_alive<0, 2>(),
            py::keep_alive<0, 3>(),
            py::keep_alive<0, 4>(),
            "Write a sample with an instance handle and specified timestamp. "
            "This method is awaitable and only for use with asyncio."
        )
        .def(
            "write_async",
            [](PyDataWriter<T>& dw, const dds::topic::TopicInstance<T>& topic_instance) {
                return PyAsyncioExecutor::run<void>(
                    std::function<void()>([&dw, &topic_instance]() {
                        dw.write(topic_instance);
                    })
                );
            },
            py::arg("topic_instance"),
            py::keep_alive<0, 1>(),
            py::keep_alive<0, 2>(),
            "Write a sample given a topic instance. This method is awaitable "
            "and only for use with asyncio."
        )
        .def(
            "write_async",
            [](PyDataWriter<T>& dw, const dds::topic::TopicInstance<T>& topic_instance, const dds::core::Time& timestamp) {
                return PyAsyncioExecutor::run<void>(
                    std::function<void()>([&dw, &topic_instance, &timestamp]() {
                        dw.write(topic_instance, timestamp);
                    })
                );
            },
            py::arg("topic_instance"),
            py::arg("timestamp"),
            py::keep_alive<0, 1>(),
            py::keep_alive<0, 2>(),
            py::keep_alive<0, 3>(),
            "Write a sample given a topic instance and specified timestamp. "
            "This method is awaitable and only for use with asyncio."
        )
        .def(
            "write_async",
            [](PyDataWriter<T>& dw, const std::vector<T>& values) {
                return PyAsyncioExecutor::run<void>(
                    std::function<void()>([&dw, &values]() {
                        dw.write(values.begin(), values.end());
                    })
                );
            },
            py::arg("samples"),
            py::keep_alive<0, 1>(),
            py::keep_alive<0, 2>(),
            "Write a sequence of samples. This method is awaitable and only "
            "for use with asyncio."
        )
        .def(
            "write_async",
            [](PyDataWriter<T>& dw, const std::vector<dds::topic::TopicInstance<T>>& values) {
                return PyAsyncioExecutor::run<void>(
                    std::function<void()>([&dw, &values]() {
                        for (auto ti : values) {
                            dw.write(ti);
                        }
                    })
                );
            },
            py::arg("topic_instances"),
            py::keep_alive<0, 1>(),
            py::keep_alive<0, 2>(),
            "Write a sequence of topic instances. This method is awaitable "
            "and only for use with asyncio."
        )
        .def(
            "write_async",
            [](PyDataWriter<T>& dw, const std::vector<T>& values, const dds::core::Time& timestamp) {
                return PyAsyncioExecutor::run<void>(
                    std::function<void()>([&dw, &values, &timestamp]() {
                        dw.write(values.begin(), values.end(), timestamp);
                    })
                );
            },
            py::arg("samples"),
            py::arg("timestamp"),
            py::keep_alive<0, 1>(),
            py::keep_alive<0, 2>(),
            py::keep_alive<0, 3>(),
            "Write a sequence of samples with a timestamp. This method is "
            "awaitable and only for use with asyncio."
        )
        .def(
            "write_async",
            [](PyDataWriter<T>& dw, const std::vector<dds::topic::TopicInstance<T>>& values, const dds::core::Time& timestamp) {
                return PyAsyncioExecutor::run<void>(
                    std::function<void()>([&dw, &values, &timestamp]() {
                        for (auto ti : values) {
                            dw.write(ti, timestamp);
                        }
                    })
                );
            },
            py::arg("topic_instances"),
            py::arg("timestamp"),
            py::keep_alive<0, 1>(),
            py::keep_alive<0, 2>(),
            py::keep_alive<0, 3>(),
            "Write a sequence of topic instances with a timestamp. This "
            "method is awaitable and only for use with asyncio."
        )
        .def(
            "write_async",
            [](PyDataWriter<T>& dw, const std::vector<T>& values, const std::vector<dds::core::InstanceHandle>& handles) {
                return PyAsyncioExecutor::run<void>(
                    std::function<void()>([&dw, &values, &handles]() {
                        dw.write(values.begin(), values.end(), handles.begin(), handles.end());
                    })
                );
            },
            py::arg("samples"),
            py::arg("handles"),
            py::keep_alive<0, 1>(),
            py::keep_alive<0, 2>(),
            py::keep_alive<0, 3>(),
            "Write a sequence of samples with their instance handles. This "
            "method is awaitable and only for use with asyncio."
        )
        .def(
            "write_async",
            [](PyDataWriter<T>& dw, const std::vector<T>& values, const std::vector<dds::core::InstanceHandle>& handles, const dds::core::Time& timestamp) {
                return PyAsyncioExecutor::run<void>(
                    std::function<void()>([&dw, &values, &handles, &timestamp]() {
                        dw.write(values.begin(), values.end(), handles.begin(), handles.end(), timestamp);
                    })
                );
            },
            py::arg("samples"),
            py::arg("handles"),
            py::arg("timestamp"),
            py::keep_alive<0, 1>(),
            py::keep_alive<0, 2>(),
            py::keep_alive<0, 3>(),
            py::keep_alive<0, 4>(),
            "Write a sequence of samples with their instance handles and a "
            "timestamp. This method is awaitable and only for use with "
            "asyncio."
        );

    py::implicitly_convertible<PyIAnyDataWriter, PyDataWriter<T>>();
    py::implicitly_convertible<PyIEntity, PyDataWriter<T>>();
}

template<typename T>
void init_dds_typed_datawriter_template(py::class_<PyDataWriter<T>, PyIEntity, PyIAnyDataWriter>& cls) {
    init_dds_typed_datawriter_base_template<T>(cls);
    cls
#if rti_connext_version_gte(6, 0, 0)
        .def(
            "create_data",
            [](PyDataWriter<T>& dw) {
                return dw->create_data();
            },
            "Create data of the writer's associated type and initialize it."
        )
#endif
        .def(
            "key_value",
            [](PyDataWriter<T>& dw, const dds::core::InstanceHandle& handle) {
                T value;
                dw.key_value(value, handle);
                return value;
            },
            py::arg("handle"),
            "Retrieve the instance key that corresponds to an instance handle."
        )
        .def(
            "topic_instance_key_value",
            [](PyDataWriter<T>& dw, const dds::core::InstanceHandle& handle) {
                T value;
                dds::topic::TopicInstance<T> ti(handle, value);
                dw.key_value(ti, handle);
                return ti;
            },
            py::arg("handle"),
            "Retrieve the instance key that corresponds to an instance handle."
        );
}

template<typename T>
void init_datawriter(py::class_<PyDataWriter<T>, PyIEntity, PyIAnyDataWriter>& dw) {
    init_dds_typed_datawriter_template(dw);
}

}
