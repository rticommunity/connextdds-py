/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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
#include <dds/domain/DomainParticipantListener.hpp>
#include "PyPublisherListener.hpp"
#include "PySubscriberListener.hpp"
#include "PyAnyTopicListener.hpp"


namespace pyrti {

class PyDomainParticipantListener
        : public dds::domain::DomainParticipantListener,
          public PyPublisherListener,
          public PySubscriberListener,
          public PyAnyTopicListener {
public:
    using dds::domain::DomainParticipantListener::DomainParticipantListener;

    virtual ~PyDomainParticipantListener()
    {
    }
};

class PyNoOpDomainParticipantListener : public PyDomainParticipantListener,
                                        public PyNoOpPublisherListener,
                                        public PyNoOpSubscriberListener,
                                        public PyNoOpAnyTopicListener {
public:
    using PyDomainParticipantListener::PyDomainParticipantListener;

    using PyNoOpPublisherListener::on_application_acknowledgment;
    using PyNoOpPublisherListener::on_instance_replaced;
    using PyNoOpPublisherListener::on_liveliness_lost;
    using PyNoOpPublisherListener::on_offered_deadline_missed;
    using PyNoOpPublisherListener::on_offered_incompatible_qos;
    using PyNoOpPublisherListener::on_publication_matched;
    using PyNoOpPublisherListener::on_reliable_reader_activity_changed;
    using PyNoOpPublisherListener::on_reliable_writer_cache_changed;
    using PyNoOpPublisherListener::on_service_request_accepted;

    void on_offered_deadline_missed(
            PyAnyDataWriter& writer,
            const dds::core::status::OfferedDeadlineMissedStatus& status)
            override
    {
    }

    void on_offered_incompatible_qos(
            PyAnyDataWriter& writer,
            const dds::core::status::OfferedIncompatibleQosStatus& status)
            override
    {
    }

    void on_liveliness_lost(
            PyAnyDataWriter& writer,
            const dds::core::status::LivelinessLostStatus& status) override
    {
    }

    void on_publication_matched(
            PyAnyDataWriter& writer,
            const dds::core::status::PublicationMatchedStatus& status) override
    {
    }

    void on_reliable_writer_cache_changed(
            PyAnyDataWriter& writer,
            const rti::core::status::ReliableWriterCacheChangedStatus& status)
            override
    {
    }

    void on_reliable_reader_activity_changed(
            PyAnyDataWriter& writer,
            const rti::core::status::ReliableReaderActivityChangedStatus&
                    status) override
    {
    }

    void on_instance_replaced(
            PyAnyDataWriter& writer,
            const dds::core::InstanceHandle& handle) override
    {
    }

    void on_application_acknowledgment(
            PyAnyDataWriter& writer,
            const rti::pub::AcknowledgmentInfo& acknowledgment_info) override
    {
    }

    void on_service_request_accepted(
            PyAnyDataWriter& writer,
            const rti::core::status::ServiceRequestAcceptedStatus& status)
            override
    {
    }

    using PyNoOpSubscriberListener::on_data_available;
    using PyNoOpSubscriberListener::on_data_on_readers;
    using PyNoOpSubscriberListener::on_liveliness_changed;
    using PyNoOpSubscriberListener::on_requested_deadline_missed;
    using PyNoOpSubscriberListener::on_requested_incompatible_qos;
    using PyNoOpSubscriberListener::on_sample_lost;
    using PyNoOpSubscriberListener::on_sample_rejected;
    using PyNoOpSubscriberListener::on_subscription_matched;

    void on_requested_deadline_missed(
            PyAnyDataReader& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status)
            override
    {
    }

    void on_requested_incompatible_qos(
            PyAnyDataReader& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status)
            override
    {
    }

    void on_sample_rejected(
            PyAnyDataReader& reader,
            const dds::core::status::SampleRejectedStatus& status) override
    {
    }

    void on_liveliness_changed(
            PyAnyDataReader& reader,
            const dds::core::status::LivelinessChangedStatus& status) override
    {
    }

    void on_data_available(PyAnyDataReader& reader) override
    {
    }

    void on_subscription_matched(
            PyAnyDataReader& reader,
            const dds::core::status::SubscriptionMatchedStatus& status) override
    {
    }

    void on_sample_lost(
            PyAnyDataReader& reader,
            const dds::core::status::SampleLostStatus& status) override
    {
    }

    void on_data_on_readers(PySubscriber& sub) override
    {
    }

    using PyNoOpAnyTopicListener::on_inconsistent_topic;

    void on_inconsistent_topic(
            PyAnyTopic& topic,
            const dds::core::status::InconsistentTopicStatus& status) override
    {
    }

    virtual ~PyNoOpDomainParticipantListener()
    {
    }
};

template<class DPLBase = PyDomainParticipantListener>
class PyDomainParticipantListenerTrampoline : public DPLBase {
public:
    using DPLBase::DPLBase;

    void on_offered_deadline_missed(
            dds::pub::AnyDataWriter& writer,
            const dds::core::status::OfferedDeadlineMissedStatus& status)
            override
    {
        auto adw = PyAnyDataWriter(writer);
        this->on_offered_deadline_missed(adw, status);
    }

    void on_offered_incompatible_qos(
            dds::pub::AnyDataWriter& writer,
            const dds::core::status::OfferedIncompatibleQosStatus& status)
            override
    {
        auto adw = PyAnyDataWriter(writer);
        this->on_offered_incompatible_qos(adw, status);
    }

    void on_liveliness_lost(
            dds::pub::AnyDataWriter& writer,
            const dds::core::status::LivelinessLostStatus& status) override
    {
        auto adw = PyAnyDataWriter(writer);
        this->on_liveliness_lost(adw, status);
    }

    void on_publication_matched(
            dds::pub::AnyDataWriter& writer,
            const dds::core::status::PublicationMatchedStatus& status) override
    {
        auto adw = PyAnyDataWriter(writer);
        this->on_publication_matched(adw, status);
    }

    void on_reliable_writer_cache_changed(
            dds::pub::AnyDataWriter& writer,
            const rti::core::status::ReliableWriterCacheChangedStatus& status)
            override
    {
        auto adw = PyAnyDataWriter(writer);
        this->on_reliable_writer_cache_changed(adw, status);
    }

    void on_reliable_reader_activity_changed(
            dds::pub::AnyDataWriter& writer,
            const rti::core::status::ReliableReaderActivityChangedStatus&
                    status) override
    {
        auto adw = PyAnyDataWriter(writer);
        this->on_reliable_reader_activity_changed(adw, status);
    }

    void on_instance_replaced(
            dds::pub::AnyDataWriter& writer,
            const dds::core::InstanceHandle& handle) override
    {
        auto adw = PyAnyDataWriter(writer);
        this->on_instance_replaced(adw, handle);
    }

    void on_application_acknowledgment(
            dds::pub::AnyDataWriter& writer,
            const rti::pub::AcknowledgmentInfo& acknowledgment_info) override
    {
        auto adw = PyAnyDataWriter(writer);
        this->on_application_acknowledgment(adw, acknowledgment_info);
    }

    void on_service_request_accepted(
            dds::pub::AnyDataWriter& writer,
            const rti::core::status::ServiceRequestAcceptedStatus& status)
            override
    {
        auto adw = PyAnyDataWriter(writer);
        this->on_service_request_accepted(adw, status);
    }

    void on_offered_deadline_missed(
            PyAnyDataWriter& writer,
            const dds::core::status::OfferedDeadlineMissedStatus& status)
            override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_offered_deadline_missed,
                writer,
                status);
    }

    void on_offered_incompatible_qos(
            PyAnyDataWriter& writer,
            const dds::core::status::OfferedIncompatibleQosStatus& status)
            override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_offered_incompatible_qos,
                writer,
                status);
    }

    void on_liveliness_lost(
            PyAnyDataWriter& writer,
            const dds::core::status::LivelinessLostStatus& status) override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_liveliness_lost,
                writer,
                status);
    }

    void on_publication_matched(
            PyAnyDataWriter& writer,
            const dds::core::status::PublicationMatchedStatus& status) override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_publication_matched,
                writer,
                status);
    }

    void on_reliable_writer_cache_changed(
            PyAnyDataWriter& writer,
            const rti::core::status::ReliableWriterCacheChangedStatus& status)
            override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_reliable_writer_cache_changed,
                writer,
                status);
    }

    void on_reliable_reader_activity_changed(
            PyAnyDataWriter& writer,
            const rti::core::status::ReliableReaderActivityChangedStatus&
                    status) override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_reliable_reader_activity_changed,
                writer,
                status);
    }

    void on_instance_replaced(
            PyAnyDataWriter& writer,
            const dds::core::InstanceHandle& handle) override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_instance_replaced,
                writer,
                handle);
    }

    void on_application_acknowledgment(
            PyAnyDataWriter& writer,
            const rti::pub::AcknowledgmentInfo& acknowledgment_info) override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_application_acknowledgment,
                writer,
                acknowledgment_info);
    }

    void on_service_request_accepted(
            PyAnyDataWriter& writer,
            const rti::core::status::ServiceRequestAcceptedStatus& status)
            override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_service_request_accepted,
                writer,
                status);
    }

    void on_requested_incompatible_qos(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status)
            override
    {
        auto adr = PyAnyDataReader(reader);
        this->on_requested_incompatible_qos(adr, status);
    }

    void on_sample_rejected(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::SampleRejectedStatus& status) override
    {
        auto adr = PyAnyDataReader(reader);
        this->on_sample_rejected(adr, status);
    }

    void on_liveliness_changed(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::LivelinessChangedStatus& status) override
    {
        auto adr = PyAnyDataReader(reader);
        this->on_liveliness_changed(adr, status);
    }

    void on_requested_deadline_missed(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status)
            override
    {
        auto adr = PyAnyDataReader(reader);
        this->on_requested_deadline_missed(adr, status);
    }

    void on_data_available(dds::sub::AnyDataReader& reader) override
    {
        auto adr = PyAnyDataReader(reader);
        this->on_data_available(adr);
    }

    void on_subscription_matched(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::SubscriptionMatchedStatus& status) override
    {
        auto adr = PyAnyDataReader(reader);
        this->on_subscription_matched(adr, status);
    }

    void on_sample_lost(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::SampleLostStatus& status) override
    {
        auto adr = PyAnyDataReader(reader);
        this->on_sample_lost(adr, status);
    }

    void on_data_on_readers(dds::sub::Subscriber& sub) override
    {
        PySubscriber s(sub);
        this->on_data_on_readers(s);
    }

    void on_requested_deadline_missed(
            PyAnyDataReader& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status)
            override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_requested_deadline_missed,
                reader,
                status);
    }

    void on_requested_incompatible_qos(
            PyAnyDataReader& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status)
            override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_requested_incompatible_qos,
                reader,
                status);
    }

    void on_sample_rejected(
            PyAnyDataReader& reader,
            const dds::core::status::SampleRejectedStatus& status) override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_sample_rejected,
                reader,
                status);
    }

    void on_liveliness_changed(
            PyAnyDataReader& reader,
            const dds::core::status::LivelinessChangedStatus& status) override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_liveliness_changed,
                reader,
                status);
    }

    void on_data_available(PyAnyDataReader& reader) override
    {
        PYBIND11_OVERLOAD_PURE(void, DPLBase, on_data_available, reader);
    }

    void on_subscription_matched(
            PyAnyDataReader& reader,
            const dds::core::status::SubscriptionMatchedStatus& status) override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_subscription_matched,
                reader,
                status);
    }

    void on_sample_lost(
            PyAnyDataReader& reader,
            const dds::core::status::SampleLostStatus& status) override
    {
        PYBIND11_OVERLOAD_PURE(void, DPLBase, on_sample_lost, reader, status);
    }

    void on_data_on_readers(PySubscriber& sub) override
    {
        PYBIND11_OVERLOAD_PURE(void, DPLBase, on_data_on_readers, sub);
    }

    void on_inconsistent_topic(
            dds::topic::AnyTopic& topic,
            const dds::core::status::InconsistentTopicStatus& status) override
    {
        auto at = PyAnyTopic(topic);
        this->on_inconsistent_topic(at, status);
    }

    void on_inconsistent_topic(
            PyAnyTopic& topic,
            const dds::core::status::InconsistentTopicStatus& status) override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_inconsistent_topic,
                topic,
                status);
    }

    virtual ~PyDomainParticipantListenerTrampoline()
    {
    }
};


template<class DPLBase = PyNoOpDomainParticipantListener>
class PyNoOpDomainParticipantListenerTrampoline
        : public PyDomainParticipantListenerTrampoline<DPLBase> {
public:
    using PyDomainParticipantListenerTrampoline<
            DPLBase>::PyDomainParticipantListenerTrampoline;

    void on_offered_deadline_missed(
            PyAnyDataWriter& writer,
            const dds::core::status::OfferedDeadlineMissedStatus& status)
            override
    {
        PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_offered_deadline_missed,
                writer,
                status);
    }

    void on_offered_incompatible_qos(
            PyAnyDataWriter& writer,
            const dds::core::status::OfferedIncompatibleQosStatus& status)
            override
    {
        PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_offered_incompatible_qos,
                writer,
                status);
    }

    void on_liveliness_lost(
            PyAnyDataWriter& writer,
            const dds::core::status::LivelinessLostStatus& status) override
    {
        PYBIND11_OVERLOAD(void, DPLBase, on_liveliness_lost, writer, status);
    }

    void on_publication_matched(
            PyAnyDataWriter& writer,
            const dds::core::status::PublicationMatchedStatus& status) override
    {
        PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_publication_matched,
                writer,
                status);
    }

    void on_reliable_writer_cache_changed(
            PyAnyDataWriter& writer,
            const rti::core::status::ReliableWriterCacheChangedStatus& status)
            override
    {
        PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_reliable_writer_cache_changed,
                writer,
                status);
    }

    void on_reliable_reader_activity_changed(
            PyAnyDataWriter& writer,
            const rti::core::status::ReliableReaderActivityChangedStatus&
                    status) override
    {
        PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_reliable_reader_activity_changed,
                writer,
                status);
    }

    void on_instance_replaced(
            PyAnyDataWriter& writer,
            const dds::core::InstanceHandle& handle) override
    {
        PYBIND11_OVERLOAD(void, DPLBase, on_instance_replaced, writer, handle);
    }

    void on_application_acknowledgment(
            PyAnyDataWriter& writer,
            const rti::pub::AcknowledgmentInfo& acknowledgment_info) override
    {
        PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_application_acknowledgment,
                writer,
                acknowledgment_info);
    }

    void on_service_request_accepted(
            PyAnyDataWriter& writer,
            const rti::core::status::ServiceRequestAcceptedStatus& status)
            override
    {
        PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_service_request_accepted,
                writer,
                status);
    }

    void on_requested_deadline_missed(
            PyAnyDataReader& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status)
            override
    {
        PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_requested_deadline_missed,
                reader,
                status);
    }

    void on_requested_incompatible_qos(
            PyAnyDataReader& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status)
            override
    {
        PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_requested_incompatible_qos,
                reader,
                status);
    }

    void on_sample_rejected(
            PyAnyDataReader& reader,
            const dds::core::status::SampleRejectedStatus& status) override
    {
        PYBIND11_OVERLOAD(void, DPLBase, on_sample_rejected, reader, status);
    }

    void on_liveliness_changed(
            PyAnyDataReader& reader,
            const dds::core::status::LivelinessChangedStatus& status) override
    {
        PYBIND11_OVERLOAD(void, DPLBase, on_liveliness_changed, reader, status);
    }

    void on_data_available(PyAnyDataReader& reader) override
    {
        PYBIND11_OVERLOAD(void, DPLBase, on_data_available, reader);
    }

    void on_subscription_matched(
            PyAnyDataReader& reader,
            const dds::core::status::SubscriptionMatchedStatus& status) override
    {
        PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_subscription_matched,
                reader,
                status);
    }

    void on_sample_lost(
            PyAnyDataReader& reader,
            const dds::core::status::SampleLostStatus& status) override
    {
        PYBIND11_OVERLOAD(void, DPLBase, on_sample_lost, reader, status);
    }

    void on_data_on_readers(PySubscriber& sub) override
    {
        PYBIND11_OVERLOAD(void, DPLBase, on_data_on_readers, sub);
    }

    void on_inconsistent_topic(
            PyAnyTopic& topic,
            const dds::core::status::InconsistentTopicStatus& status) override
    {
        PYBIND11_OVERLOAD(void, DPLBase, on_inconsistent_topic, topic, status);
    }

    virtual ~PyNoOpDomainParticipantListenerTrampoline()
    {
    }
};

}  // namespace pyrti
