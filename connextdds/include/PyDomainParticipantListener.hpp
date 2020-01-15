#pragma once

#include "PyConnext.hpp"
#include <dds/domain/DomainParticipantListener.hpp>
#include "PyPublisherListener.hpp"
#include "PySubscriberListener.hpp"
#include "PyAnyTopicListener.hpp"


namespace pyrti {
    class PyDomainParticipantListener :
        public dds::domain::DomainParticipantListener,
        public pyrti::PyPublisherListener,
        public pyrti::PySubscriberListener,
        public pyrti::PyAnyTopicListener {
    public:
        using dds::domain::DomainParticipantListener::DomainParticipantListener;

        virtual
        ~PyDomainParticipantListener() {}
    };

    class PyNoOpDomainParticipantListener : 
        public pyrti::PyDomainParticipantListener,
        public pyrti::PyNoOpPublisherListener,
        public pyrti::PyNoOpSubscriberListener,
        public pyrti::PyNoOpAnyTopicListener {
    public:
        using pyrti::PyDomainParticipantListener::PyDomainParticipantListener;

        using pyrti::PyNoOpPublisherListener::on_offered_deadline_missed;
        using pyrti::PyNoOpPublisherListener::on_offered_incompatible_qos;
        using pyrti::PyNoOpPublisherListener::on_liveliness_lost;
        using pyrti::PyNoOpPublisherListener::on_publication_matched;
        using pyrti::PyNoOpPublisherListener::on_reliable_writer_cache_changed;
        using pyrti::PyNoOpPublisherListener::on_reliable_reader_activity_changed;
        using pyrti::PyNoOpPublisherListener::on_instance_replaced;
        using pyrti::PyNoOpPublisherListener::on_application_acknowledgment;
        using pyrti::PyNoOpPublisherListener::on_service_request_accepted;
    
        void on_offered_deadline_missed(
            pyrti::PyAnyDataWriter& writer,
            const dds::core::status::OfferedDeadlineMissedStatus& status
        ) override {}

        void on_offered_incompatible_qos(
            pyrti::PyAnyDataWriter& writer,
            const dds::core::status::OfferedIncompatibleQosStatus& status
        ) override {}

        void on_liveliness_lost(
            pyrti::PyAnyDataWriter& writer,
            const dds::core::status::LivelinessLostStatus& status
        ) override {}

        void on_publication_matched(
            pyrti::PyAnyDataWriter& writer,
            const dds::core::status::PublicationMatchedStatus& status
        ) override {}

        void on_reliable_writer_cache_changed(
            pyrti::PyAnyDataWriter& writer,
            const rti::core::status::ReliableWriterCacheChangedStatus& status
        ) override {}

        void on_reliable_reader_activity_changed(
            pyrti::PyAnyDataWriter& writer,
            const rti::core::status::ReliableReaderActivityChangedStatus& status
        ) override {}

        void on_instance_replaced(
            pyrti::PyAnyDataWriter& writer,
            const dds::core::InstanceHandle& handle
        ) override {}

        void on_application_acknowledgment(
            pyrti::PyAnyDataWriter& writer,
            const rti::pub::AcknowledgmentInfo& acknowledgment_info
        ) override {}

        void on_service_request_accepted(
            pyrti::PyAnyDataWriter& writer,
            const rti::core::status::ServiceRequestAcceptedStatus& status
        ) override {}

        using pyrti::PyNoOpSubscriberListener::on_requested_deadline_missed;
        using pyrti::PyNoOpSubscriberListener::on_requested_incompatible_qos;
        using pyrti::PyNoOpSubscriberListener::on_sample_rejected;
        using pyrti::PyNoOpSubscriberListener::on_liveliness_changed;
        using pyrti::PyNoOpSubscriberListener::on_data_available;
        using pyrti::PyNoOpSubscriberListener::on_subscription_matched;
        using pyrti::PyNoOpSubscriberListener::on_sample_lost;
        using pyrti::PyNoOpSubscriberListener::on_data_on_readers;

        void on_requested_deadline_missed(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status
        ) override {}

        void on_requested_incompatible_qos(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status
        ) override {}

        void on_sample_rejected(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SampleRejectedStatus& status
        ) override {}

        void on_liveliness_changed(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::LivelinessChangedStatus& status
        ) override{}

        void on_data_available(
            pyrti::PyAnyDataReader& reader
        ) override{}

        void on_subscription_matched(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SubscriptionMatchedStatus& status
        ) override {}

        void on_sample_lost(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SampleLostStatus& status
        ) override {}

        void on_data_on_readers(
            pyrti::PySubscriber& sub
        ) override {}

        using pyrti::PyNoOpAnyTopicListener::on_inconsistent_topic;

        void on_inconsistent_topic(
            pyrti::PyAnyTopic& topic,
            const dds::core::status::InconsistentTopicStatus& status
        ) override {}
    
        virtual
        ~PyNoOpDomainParticipantListener() {}
    };

    template<class DPLBase = pyrti::PyDomainParticipantListener>
    class PyDomainParticipantListenerTrampoline : public DPLBase {
    public:
        using DPLBase::DPLBase;

        void on_offered_deadline_missed(
            dds::pub::AnyDataWriter& writer,
            const dds::core::status::OfferedDeadlineMissedStatus& status
        ) override {
            auto adw = pyrti::PyAnyDataWriter(writer);
            this->on_offered_deadline_missed(adw, status);
        }

        void on_offered_incompatible_qos(
            dds::pub::AnyDataWriter& writer,
            const dds::core::status::OfferedIncompatibleQosStatus& status
        ) override {
            auto adw = pyrti::PyAnyDataWriter(writer);
            this->on_offered_incompatible_qos(adw, status);
        }

        void on_liveliness_lost(
            dds::pub::AnyDataWriter& writer,
            const dds::core::status::LivelinessLostStatus& status
        ) override {
            auto adw = pyrti::PyAnyDataWriter(writer);
            this->on_liveliness_lost(adw, status);
        }

        void on_publication_matched(
            dds::pub::AnyDataWriter& writer,
            const dds::core::status::PublicationMatchedStatus& status
        ) override {
            auto adw = pyrti::PyAnyDataWriter(writer);
            this->on_publication_matched(adw, status);
        }

        void on_reliable_writer_cache_changed(
            dds::pub::AnyDataWriter& writer,
            const rti::core::status::ReliableWriterCacheChangedStatus& status
        ) override {
            auto adw = pyrti::PyAnyDataWriter(writer);
            this->on_reliable_writer_cache_changed(adw, status);
        }

        void on_reliable_reader_activity_changed(
            dds::pub::AnyDataWriter& writer,
            const rti::core::status::ReliableReaderActivityChangedStatus& status
        ) override {
            auto adw = pyrti::PyAnyDataWriter(writer);
            this->on_reliable_reader_activity_changed(adw, status);
        }

        void on_instance_replaced(
            dds::pub::AnyDataWriter& writer,
            const dds::core::InstanceHandle& handle
        ) override {
            auto adw = pyrti::PyAnyDataWriter(writer);
            this->on_instance_replaced(adw, handle);
        }

        void on_application_acknowledgment(
            dds::pub::AnyDataWriter& writer,
            const rti::pub::AcknowledgmentInfo& acknowledgment_info
        ) override {
            auto adw = pyrti::PyAnyDataWriter(writer);
            this->on_application_acknowledgment(adw, acknowledgment_info);
        }

        void on_service_request_accepted(
            dds::pub::AnyDataWriter& writer,
            const rti::core::status::ServiceRequestAcceptedStatus& status
        ) override {
            auto adw = pyrti::PyAnyDataWriter(writer);
            this->on_service_request_accepted(adw, status);
        }

        void on_offered_deadline_missed(
            pyrti::PyAnyDataWriter& writer,
            const dds::core::status::OfferedDeadlineMissedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_offered_deadline_missed,
                writer,
                status
            );
        }

        void on_offered_incompatible_qos(
            pyrti::PyAnyDataWriter& writer,
            const dds::core::status::OfferedIncompatibleQosStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_offered_incompatible_qos,
                writer,
                status
            );
        }

        void on_liveliness_lost(
            pyrti::PyAnyDataWriter& writer,
            const dds::core::status::LivelinessLostStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_liveliness_lost,
                writer,
                status
            );
        }

        void on_publication_matched(
            pyrti::PyAnyDataWriter& writer,
            const dds::core::status::PublicationMatchedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_publication_matched,
                writer,
                status
            );
        }

        void on_reliable_writer_cache_changed(
            pyrti::PyAnyDataWriter& writer,
            const rti::core::status::ReliableWriterCacheChangedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_reliable_writer_cache_changed,
                writer,
                status
            );
        }

        void on_reliable_reader_activity_changed(
            pyrti::PyAnyDataWriter& writer,
            const rti::core::status::ReliableReaderActivityChangedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_reliable_reader_activity_changed,
                writer,
                status
            );
        }

        void on_instance_replaced(
            pyrti::PyAnyDataWriter& writer,
            const dds::core::InstanceHandle& handle
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_instance_replaced,
                writer,
                handle
            );
        }

        void on_application_acknowledgment(
            pyrti::PyAnyDataWriter& writer,
            const rti::pub::AcknowledgmentInfo& acknowledgment_info
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_application_acknowledgment,
                writer,
                acknowledgment_info
            );
        }

        void on_service_request_accepted(
            pyrti::PyAnyDataWriter& writer,
            const rti::core::status::ServiceRequestAcceptedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_service_request_accepted,
                writer,
                status
            );
        }

        void on_requested_incompatible_qos(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status
        ) override {
            auto adr = pyrti::PyAnyDataReader(reader);
            this->on_requested_incompatible_qos(adr, status);
        }

        void on_sample_rejected(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::SampleRejectedStatus& status
        ) override {
            auto adr = pyrti::PyAnyDataReader(reader);
            this->on_sample_rejected(adr, status);
        }

        void on_liveliness_changed(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::LivelinessChangedStatus& status
        ) override {
            auto adr = pyrti::PyAnyDataReader(reader);
            this->on_liveliness_changed(adr, status);
        }

        void on_requested_deadline_missed(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status
        ) override {
            auto adr = pyrti::PyAnyDataReader(reader);
            this->on_requested_deadline_missed(adr, status);
        }

        void on_data_available(
            dds::sub::AnyDataReader& reader
        ) override {
            auto adr = pyrti::PyAnyDataReader(reader);
            this->on_data_available(adr);
        }

        void on_subscription_matched(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::SubscriptionMatchedStatus& status
        ) override {
            auto adr = pyrti::PyAnyDataReader(reader);
            this->on_subscription_matched(adr, status);
        }

        void on_sample_lost(
            dds::sub::AnyDataReader& reader,
            const dds::core::status::SampleLostStatus& status
        ) override {
            auto adr = pyrti::PyAnyDataReader(reader);
            this->on_sample_lost(adr, status);
        }

        void on_data_on_readers(
            dds::sub::Subscriber& sub
        ) override {
            pyrti::PySubscriber s(sub);
            this->on_data_on_readers(s);
        }

        void on_requested_deadline_missed(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_requested_deadline_missed,
                reader,
                status
            );
        }
        
        void on_requested_incompatible_qos(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_requested_incompatible_qos,
                reader,
                status
            );
        }

        void on_sample_rejected(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SampleRejectedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_sample_rejected,
                reader,
                status
            );
        }

        void on_liveliness_changed(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::LivelinessChangedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_liveliness_changed,
                reader,
                status
            );
        }

        void on_data_available(
            pyrti::PyAnyDataReader& reader
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_data_available,
                reader
            );
        }

        void on_subscription_matched(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SubscriptionMatchedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_subscription_matched,
                reader,
                status
            );
        }

        void on_sample_lost(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SampleLostStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_sample_lost,
                reader,
                status
            );
        }

        void on_data_on_readers(
            pyrti::PySubscriber& sub
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_data_on_readers,
                sub
            );
        }

        void on_inconsistent_topic(
            dds::topic::AnyTopic& topic,
            const dds::core::status::InconsistentTopicStatus& status
        ) override {
            auto at = pyrti::PyAnyTopic(topic);
            this->on_inconsistent_topic(at, status);
        }

        void on_inconsistent_topic(
            pyrti::PyAnyTopic& topic,
            const dds::core::status::InconsistentTopicStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DPLBase,
                on_inconsistent_topic,
                topic,
                status
            );
        }
    
        virtual
        ~PyDomainParticipantListenerTrampoline() {}
        
    };



    template<class DPLBase = pyrti::PyNoOpDomainParticipantListener>
    class PyNoOpDomainParticipantListenerTrampoline : public PyDomainParticipantListenerTrampoline<DPLBase> {
    public:
        using PyDomainParticipantListenerTrampoline<DPLBase>::PyDomainParticipantListenerTrampoline;

        void on_offered_deadline_missed(
            pyrti::PyAnyDataWriter& writer,
            const dds::core::status::OfferedDeadlineMissedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_offered_deadline_missed,
                writer,
                status
            );
        }

        void on_offered_incompatible_qos(
            pyrti::PyAnyDataWriter& writer,
            const dds::core::status::OfferedIncompatibleQosStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_offered_incompatible_qos,
                writer,
                status
            );
        }

        void on_liveliness_lost(
            pyrti::PyAnyDataWriter& writer,
            const dds::core::status::LivelinessLostStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_liveliness_lost,
                writer,
                status
            );
        }

        void on_publication_matched(
            pyrti::PyAnyDataWriter& writer,
            const dds::core::status::PublicationMatchedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_publication_matched,
                writer,
                status
            );
        }

        void on_reliable_writer_cache_changed(
            pyrti::PyAnyDataWriter& writer,
            const rti::core::status::ReliableWriterCacheChangedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_reliable_writer_cache_changed,
                writer,
                status
            );
        }

        void on_reliable_reader_activity_changed(
            pyrti::PyAnyDataWriter& writer,
            const rti::core::status::ReliableReaderActivityChangedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_reliable_reader_activity_changed,
                writer,
                status
            );
        }

        void on_instance_replaced(
            pyrti::PyAnyDataWriter& writer,
            const dds::core::InstanceHandle& handle
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_instance_replaced,
                writer,
                handle
            );
        }

        void on_application_acknowledgment(
            pyrti::PyAnyDataWriter& writer,
            const rti::pub::AcknowledgmentInfo& acknowledgment_info
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_application_acknowledgment,
                writer,
                acknowledgment_info
            );
        }

        void on_service_request_accepted(
            pyrti::PyAnyDataWriter& writer,
            const rti::core::status::ServiceRequestAcceptedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_service_request_accepted,
                writer,
                status
            );
        }

        void on_requested_deadline_missed(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::RequestedDeadlineMissedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_requested_deadline_missed,
                reader,
                status
            );
        }
        
        void on_requested_incompatible_qos(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::RequestedIncompatibleQosStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_requested_incompatible_qos,
                reader,
                status
            );
        }

        void on_sample_rejected(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SampleRejectedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_sample_rejected,
                reader,
                status
            );
        }

        void on_liveliness_changed(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::LivelinessChangedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_liveliness_changed,
                reader,
                status
            );
        }

        void on_data_available(
            pyrti::PyAnyDataReader& reader
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_data_available,
                reader
            );
        }

        void on_subscription_matched(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SubscriptionMatchedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_subscription_matched,
                reader,
                status
            );
        }

        void on_sample_lost(
            pyrti::PyAnyDataReader& reader,
            const dds::core::status::SampleLostStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_sample_lost,
                reader,
                status
            );
        }

        void on_data_on_readers(
            pyrti::PySubscriber& sub
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_data_on_readers,
                sub
            );
        }

        void on_inconsistent_topic(
            pyrti::PyAnyTopic& topic,
            const dds::core::status::InconsistentTopicStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DPLBase,
                on_inconsistent_topic,
                topic,
                status
            );
        }

        virtual
        ~PyNoOpDomainParticipantListenerTrampoline() {}
    };
}

