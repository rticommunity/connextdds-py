#pragma once

#include "PyAnyDataWriter.hpp"
#include "PyConnext.hpp"
#include <dds/pub/AnyDataWriterListener.hpp>

namespace pyrti {
class PyAnyDataWriterListener : public dds::pub::AnyDataWriterListener {
public:
    using dds::pub::AnyDataWriterListener::AnyDataWriterListener;

    using dds::pub::AnyDataWriterListener::on_application_acknowledgment;
    using dds::pub::AnyDataWriterListener::on_instance_replaced;
    using dds::pub::AnyDataWriterListener::on_liveliness_lost;
    using dds::pub::AnyDataWriterListener::on_offered_deadline_missed;
    using dds::pub::AnyDataWriterListener::on_offered_incompatible_qos;
    using dds::pub::AnyDataWriterListener::on_publication_matched;
    using dds::pub::AnyDataWriterListener::on_reliable_reader_activity_changed;
    using dds::pub::AnyDataWriterListener::on_reliable_writer_cache_changed;
    using dds::pub::AnyDataWriterListener::on_service_request_accepted;

    virtual void on_offered_deadline_missed(
            PyAnyDataWriter& writer,
            const dds::core::status::OfferedDeadlineMissedStatus& status) = 0;

    virtual void on_offered_incompatible_qos(
            PyAnyDataWriter& writer,
            const dds::core::status::OfferedIncompatibleQosStatus& status) = 0;

    virtual void on_liveliness_lost(
            PyAnyDataWriter& writer,
            const dds::core::status::LivelinessLostStatus& status) = 0;

    virtual void on_publication_matched(
            PyAnyDataWriter& writer,
            const dds::core::status::PublicationMatchedStatus& status) = 0;

    virtual void on_reliable_writer_cache_changed(
            PyAnyDataWriter& writer,
            const rti::core::status::ReliableWriterCacheChangedStatus&
                    status) = 0;

    virtual void on_reliable_reader_activity_changed(
            PyAnyDataWriter& writer,
            const rti::core::status::ReliableReaderActivityChangedStatus&
                    status) = 0;

    virtual void on_instance_replaced(
            PyAnyDataWriter& writer,
            const dds::core::InstanceHandle& handle) = 0;

    virtual void on_application_acknowledgment(
            PyAnyDataWriter& writer,
            const rti::pub::AcknowledgmentInfo& acknowledgment_info) = 0;

    virtual void on_service_request_accepted(
            PyAnyDataWriter& writer,
            const rti::core::status::ServiceRequestAcceptedStatus& status) = 0;

    virtual ~PyAnyDataWriterListener()
    {
    }
};


class PyNoOpAnyDataWriterListener : public PyAnyDataWriterListener {
public:
    using PyAnyDataWriterListener::PyAnyDataWriterListener;

    using PyAnyDataWriterListener::on_application_acknowledgment;
    using PyAnyDataWriterListener::on_instance_replaced;
    using PyAnyDataWriterListener::on_liveliness_lost;
    using PyAnyDataWriterListener::on_offered_deadline_missed;
    using PyAnyDataWriterListener::on_offered_incompatible_qos;
    using PyAnyDataWriterListener::on_publication_matched;
    using PyAnyDataWriterListener::on_reliable_reader_activity_changed;
    using PyAnyDataWriterListener::on_reliable_writer_cache_changed;
    using PyAnyDataWriterListener::on_service_request_accepted;

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

    virtual ~PyNoOpAnyDataWriterListener()
    {
    }
};


template<class ADWLBase = PyAnyDataWriterListener>
class PyAnyDataWriterListenerTrampoline : public ADWLBase {
public:
    using ADWLBase::ADWLBase;

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
                ADWLBase,
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
                ADWLBase,
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
                ADWLBase,
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
                ADWLBase,
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
                ADWLBase,
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
                ADWLBase,
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
                ADWLBase,
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
                ADWLBase,
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
                ADWLBase,
                on_service_request_accepted,
                writer,
                status);
    }

    virtual ~PyAnyDataWriterListenerTrampoline()
    {
    }
};


template<class ADWLBase = PyNoOpAnyDataWriterListener>
class PyNoOpAnyDataWriterListenerTrampoline
        : public PyAnyDataWriterListenerTrampoline<ADWLBase> {
public:
    using PyAnyDataWriterListenerTrampoline<
            ADWLBase>::PyAnyDataWriterListenerTrampoline;

    using PyAnyDataWriterListenerTrampoline<
            ADWLBase>::on_offered_deadline_missed;
    using PyAnyDataWriterListenerTrampoline<
            ADWLBase>::on_offered_incompatible_qos;
    using PyAnyDataWriterListenerTrampoline<ADWLBase>::on_liveliness_lost;
    using PyAnyDataWriterListenerTrampoline<ADWLBase>::on_publication_matched;
    using PyAnyDataWriterListenerTrampoline<
            ADWLBase>::on_reliable_writer_cache_changed;
    using PyAnyDataWriterListenerTrampoline<
            ADWLBase>::on_reliable_reader_activity_changed;
    using PyAnyDataWriterListenerTrampoline<ADWLBase>::on_instance_replaced;
    using PyAnyDataWriterListenerTrampoline<
            ADWLBase>::on_application_acknowledgment;
    using PyAnyDataWriterListenerTrampoline<
            ADWLBase>::on_service_request_accepted;

    void on_offered_deadline_missed(
            PyAnyDataWriter& writer,
            const dds::core::status::OfferedDeadlineMissedStatus& status)
            override
    {
        PYBIND11_OVERLOAD(
                void,
                ADWLBase,
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
                ADWLBase,
                on_offered_incompatible_qos,
                writer,
                status);
    }

    void on_liveliness_lost(
            PyAnyDataWriter& writer,
            const dds::core::status::LivelinessLostStatus& status) override
    {
        PYBIND11_OVERLOAD(void, ADWLBase, on_liveliness_lost, writer, status);
    }

    void on_publication_matched(
            PyAnyDataWriter& writer,
            const dds::core::status::PublicationMatchedStatus& status) override
    {
        PYBIND11_OVERLOAD(
                void,
                ADWLBase,
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
                ADWLBase,
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
                ADWLBase,
                on_reliable_reader_activity_changed,
                writer,
                status);
    }

    void on_instance_replaced(
            PyAnyDataWriter& writer,
            const dds::core::InstanceHandle& handle) override
    {
        PYBIND11_OVERLOAD(void, ADWLBase, on_instance_replaced, writer, handle);
    }

    void on_application_acknowledgment(
            PyAnyDataWriter& writer,
            const rti::pub::AcknowledgmentInfo& acknowledgment_info) override
    {
        PYBIND11_OVERLOAD(
                void,
                ADWLBase,
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
                ADWLBase,
                on_service_request_accepted,
                writer,
                status);
    }

    virtual ~PyNoOpAnyDataWriterListenerTrampoline()
    {
    }
};
}  // namespace pyrti
