#pragma once

#include "PyConnext.hpp"
#include <dds/pub/DataWriterListener.hpp>
#include "PyDataWriter.hpp"

namespace pyrti {
    template<typename T>
    class PyDataWriter;

    template<typename T>
    class PyDataWriterListener : public dds::pub::DataWriterListener<T> {
    public:
        using dds::pub::DataWriterListener<T>::DataWriterListener;

        using dds::pub::DataWriterListener<T>::on_offered_deadline_missed;
        using dds::pub::DataWriterListener<T>::on_offered_incompatible_qos;
        using dds::pub::DataWriterListener<T>::on_liveliness_lost;
        using dds::pub::DataWriterListener<T>::on_publication_matched;
        using dds::pub::DataWriterListener<T>::on_reliable_writer_cache_changed;
        using dds::pub::DataWriterListener<T>::on_reliable_reader_activity_changed;
        using dds::pub::DataWriterListener<T>::on_instance_replaced;
        using dds::pub::DataWriterListener<T>::on_application_acknowledgment;
        using dds::pub::DataWriterListener<T>::on_service_request_accepted;

        virtual
        void on_offered_deadline_missed(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::OfferedDeadlineMissedStatus& status
        ) = 0;

        virtual
        void on_offered_incompatible_qos(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::OfferedIncompatibleQosStatus& status
        ) = 0;

        virtual
        void on_liveliness_lost(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::LivelinessLostStatus& status
        ) = 0;

        virtual
        void on_publication_matched(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::PublicationMatchedStatus& status
        ) = 0;

        virtual
        void on_reliable_writer_cache_changed(
            pyrti::PyDataWriter<T>& writer,
            const rti::core::status::ReliableWriterCacheChangedStatus& status
        ) = 0;

        virtual
        void on_reliable_reader_activity_changed(
            pyrti::PyDataWriter<T>& writer,
            const rti::core::status::ReliableReaderActivityChangedStatus& status
        ) = 0;

        virtual
        void on_instance_replaced(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::InstanceHandle& handle
        ) = 0;

        virtual
        void on_application_acknowledgment(
            pyrti::PyDataWriter<T>& writer,
            const rti::pub::AcknowledgmentInfo& acknowledgment_info
        ) = 0;

        virtual
        void on_service_request_accepted(
            pyrti::PyDataWriter<T>& writer,
            const rti::core::status::ServiceRequestAcceptedStatus& status
        ) = 0;

        void on_destination_unreachable(
            dds::pub::DataWriter<T>& writer,
            const dds::core::InstanceHandle& handle,
            const rti::core::Locator& locator
        ) override {}

        void* on_data_request(
            dds::pub::DataWriter<T>& writer,
            const rti::core::Cookie& cookie
        ) override { return nullptr; }

        void on_data_return(
            dds::pub::DataWriter<T>& writer,
            void* data,
            const rti::core::Cookie& cookie
        ) override {}

        void on_sample_removed(
            dds::pub::DataWriter<T>& writer,
            const rti::core::Cookie& cookie
        ) override {}
    };

    template<typename T>
    class PyNoOpDataWriterListener : public pyrti::PyDataWriterListener<T> {
    public:
        using pyrti::PyDataWriterListener<T>::PyDataWriterListener;

        using pyrti::PyDataWriterListener<T>::on_offered_deadline_missed;
        using pyrti::PyDataWriterListener<T>::on_offered_incompatible_qos;
        using pyrti::PyDataWriterListener<T>::on_liveliness_lost;
        using pyrti::PyDataWriterListener<T>::on_publication_matched;
        using pyrti::PyDataWriterListener<T>::on_reliable_writer_cache_changed;
        using pyrti::PyDataWriterListener<T>::on_reliable_reader_activity_changed;
        using pyrti::PyDataWriterListener<T>::on_instance_replaced;
        using pyrti::PyDataWriterListener<T>::on_application_acknowledgment;
        using pyrti::PyDataWriterListener<T>::on_service_request_accepted;

        void on_offered_deadline_missed(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::OfferedDeadlineMissedStatus& status
        ) override {}

        void on_offered_incompatible_qos(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::OfferedIncompatibleQosStatus& status
        ) override {}

        void on_liveliness_lost(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::LivelinessLostStatus& status
        ) override {}

        void on_publication_matched(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::PublicationMatchedStatus& status
        ) override {}

        void on_reliable_writer_cache_changed(
            pyrti::PyDataWriter<T>& writer,
            const rti::core::status::ReliableWriterCacheChangedStatus& status
        ) override {}

        void on_reliable_reader_activity_changed(
            pyrti::PyDataWriter<T>& writer,
            const rti::core::status::ReliableReaderActivityChangedStatus& status
        ) override {}

        void on_instance_replaced(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::InstanceHandle& handle
        ) override {}

        void on_application_acknowledgment(
            pyrti::PyDataWriter<T>& writer,
            const rti::pub::AcknowledgmentInfo& acknowledgment_info
        ) override {}

        void on_service_request_accepted(
            pyrti::PyDataWriter<T>& writer,
            const rti::core::status::ServiceRequestAcceptedStatus& status
        ) override {}
    };

    template<typename T, class DWLBase = pyrti::PyDataWriterListener<T>>
    class PyDataWriterListenerTrampoline : public DWLBase {
    public:
        using DWLBase::DWLBase;

        void on_offered_deadline_missed(
            dds::pub::DataWriter<T>& writer,
            const dds::core::status::OfferedDeadlineMissedStatus& status
        ) override {
            pyrti::PyDataWriter<T> dw(writer);
            this->on_offered_deadline_missed(dw, status);
        }

        void on_offered_incompatible_qos(
            dds::pub::DataWriter<T>& writer,
            const dds::core::status::OfferedIncompatibleQosStatus& status
        ) override {
            pyrti::PyDataWriter<T> dw(writer);
            this->on_offered_incompatible_qos(dw, status);
        }

        void on_liveliness_lost(
            dds::pub::DataWriter<T>& writer,
            const dds::core::status::LivelinessLostStatus& status
        ) override {
            pyrti::PyDataWriter<T> dw(writer);
            this->on_liveliness_lost(dw, status);
        }

        void on_publication_matched(
            dds::pub::DataWriter<T>& writer,
            const dds::core::status::PublicationMatchedStatus& status
        ) override {
            pyrti::PyDataWriter<T> dw(writer);
            this->on_publication_matched(dw, status);
        }

        void on_reliable_writer_cache_changed(
            dds::pub::DataWriter<T>& writer,
            const rti::core::status::ReliableWriterCacheChangedStatus& status
        ) override {
            pyrti::PyDataWriter<T> dw(writer);
            this->on_reliable_writer_cache_changed(dw, status);
        }

        void on_reliable_reader_activity_changed(
            dds::pub::DataWriter<T>& writer,
            const rti::core::status::ReliableReaderActivityChangedStatus& status
        ) override {
            pyrti::PyDataWriter<T> dw(writer);
            this->on_reliable_reader_activity_changed(dw, status);
        }

        void on_instance_replaced(
            dds::pub::DataWriter<T>& writer,
            const dds::core::InstanceHandle& handle
        ) override {
            pyrti::PyDataWriter<T> dw(writer);
            this->on_instance_replaced(dw, handle);
        }

        void on_application_acknowledgment(
            dds::pub::DataWriter<T>& writer,
            const rti::pub::AcknowledgmentInfo& acknowledgment_info
        ) override {
            pyrti::PyDataWriter<T> dw(writer);
            this->on_application_acknowledgment(dw, acknowledgment_info);
        }

        void on_service_request_accepted(
            dds::pub::DataWriter<T>& writer,
            const rti::core::status::ServiceRequestAcceptedStatus& status
        ) override {
            pyrti::PyDataWriter<T> dw(writer);
            this->on_service_request_accepted(dw, status);
        }

        void on_offered_deadline_missed(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::OfferedDeadlineMissedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DWLBase,
                on_offered_deadline_missed,
                writer,
                status
            );
        }

        void on_offered_incompatible_qos(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::OfferedIncompatibleQosStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DWLBase,
                on_offered_incompatible_qos,
                writer,
                status
            );
        }

        void on_liveliness_lost(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::LivelinessLostStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DWLBase,
                on_liveliness_lost,
                writer,
                status
            );
        }

        void on_publication_matched(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::PublicationMatchedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DWLBase,
                on_publication_matched,
                writer,
                status
            );
        }

        void on_reliable_writer_cache_changed(
            pyrti::PyDataWriter<T>& writer,
            const rti::core::status::ReliableWriterCacheChangedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DWLBase,
                on_reliable_writer_cache_changed,
                writer,
                status
            );
        }

        void on_reliable_reader_activity_changed(
            pyrti::PyDataWriter<T>& writer,
            const rti::core::status::ReliableReaderActivityChangedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DWLBase,
                on_reliable_reader_activity_changed,
                writer,
                status
            );
        }

        void on_instance_replaced(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::InstanceHandle& handle
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DWLBase,
                on_instance_replaced,
                writer,
                handle
            );
        }

        void on_application_acknowledgment(
            pyrti::PyDataWriter<T>& writer,
            const rti::pub::AcknowledgmentInfo& acknowledgment_info
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DWLBase,
                on_application_acknowledgment,
                writer,
                acknowledgment_info
            );
        }

        void on_service_request_accepted(
            pyrti::PyDataWriter<T>& writer,
            const rti::core::status::ServiceRequestAcceptedStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                DWLBase,
                on_service_request_accepted,
                writer,
                status
            );
        }
    };

    template<typename T, class DWLBase = pyrti::PyNoOpDataWriterListener<T>>
    class PyNoOpDataWriterListenerTrampoline : public pyrti::PyDataWriterListenerTrampoline<T, DWLBase> {
    public:
        using pyrti::PyDataWriterListenerTrampoline<T, DWLBase>::PyDataWriterListenerTrampoline;

        using pyrti::PyDataWriterListenerTrampoline<T, DWLBase>::on_offered_deadline_missed;
        using pyrti::PyDataWriterListenerTrampoline<T, DWLBase>::on_offered_incompatible_qos;
        using pyrti::PyDataWriterListenerTrampoline<T, DWLBase>::on_liveliness_lost;
        using pyrti::PyDataWriterListenerTrampoline<T, DWLBase>::on_publication_matched;
        using pyrti::PyDataWriterListenerTrampoline<T, DWLBase>::on_reliable_writer_cache_changed;
        using pyrti::PyDataWriterListenerTrampoline<T, DWLBase>::on_reliable_reader_activity_changed;
        using pyrti::PyDataWriterListenerTrampoline<T, DWLBase>::on_instance_replaced;
        using pyrti::PyDataWriterListenerTrampoline<T, DWLBase>::on_application_acknowledgment;
        using pyrti::PyDataWriterListenerTrampoline<T, DWLBase>::on_service_request_accepted;

        void on_offered_deadline_missed(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::OfferedDeadlineMissedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DWLBase,
                on_offered_deadline_missed,
                writer,
                status
            );
        }

        void on_offered_incompatible_qos(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::OfferedIncompatibleQosStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DWLBase,
                on_offered_incompatible_qos,
                writer,
                status
            );
        }

        void on_liveliness_lost(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::LivelinessLostStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DWLBase,
                on_liveliness_lost,
                writer,
                status
            );
        }

        void on_publication_matched(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::status::PublicationMatchedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DWLBase,
                on_publication_matched,
                writer,
                status
            );
        }

        void on_reliable_writer_cache_changed(
            pyrti::PyDataWriter<T>& writer,
            const rti::core::status::ReliableWriterCacheChangedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DWLBase,
                on_reliable_writer_cache_changed,
                writer,
                status
            );
        }

        void on_reliable_reader_activity_changed(
            pyrti::PyDataWriter<T>& writer,
            const rti::core::status::ReliableReaderActivityChangedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DWLBase,
                on_reliable_reader_activity_changed,
                writer,
                status
            );
        }

        void on_instance_replaced(
            pyrti::PyDataWriter<T>& writer,
            const dds::core::InstanceHandle& handle
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DWLBase,
                on_instance_replaced,
                writer,
                handle
            );
        }

        void on_application_acknowledgment(
            pyrti::PyDataWriter<T>& writer,
            const rti::pub::AcknowledgmentInfo& acknowledgment_info
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DWLBase,
                on_application_acknowledgment,
                writer,
                acknowledgment_info
            );
        }

        void on_service_request_accepted(
            pyrti::PyDataWriter<T>& writer,
            const rti::core::status::ServiceRequestAcceptedStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                DWLBase,
                on_service_request_accepted,
                writer,
                status
            );
        }
    };

    template<typename T>
    void init_class_defs(py::class_<pyrti::PyDataWriterListener<T>, pyrti::PyDataWriterListenerTrampoline<T>>& cls) {
        cls
            .def(
                py::init<>()
            )
            .def(
                "on_offered_deadline_missed",
                (void (pyrti::PyDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const dds::core::status::OfferedDeadlineMissedStatus&)) &pyrti::PyDataWriterListener<T>::on_offered_deadline_missed,
                "Offered deadline missed callback."
            )
            .def(
                "on_offered_incompatible_qos",
                (void (pyrti::PyDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const dds::core::status::OfferedIncompatibleQosStatus&)) &pyrti::PyDataWriterListener<T>::on_offered_incompatible_qos,
                "Offered incompatible QoS callback."
            )
            .def(
                "on_liveliness_lost",
                (void (pyrti::PyDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const dds::core::status::LivelinessLostStatus&)) &pyrti::PyDataWriterListener<T>::on_liveliness_lost,
                "Liveliness lost callback."
            )
            .def(
                "on_publication_matched",
                (void (pyrti::PyDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const dds::core::status::PublicationMatchedStatus&)) &pyrti::PyDataWriterListener<T>::on_publication_matched,
                "Publication matched callback."
            )
            .def(
                "on_reliable_writer_cache_changed",
                (void (pyrti::PyDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const rti::core::status::ReliableWriterCacheChangedStatus&)) &pyrti::PyDataWriterListener<T>::on_reliable_writer_cache_changed,
                "Reliable writer cache changed callback."
            )
            .def(
                "on_reliable_reader_activity_changed",
                (void (pyrti::PyDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const rti::core::status::ReliableReaderActivityChangedStatus&)) &pyrti::PyDataWriterListener<T>::on_reliable_reader_activity_changed,
                "Reliable reader activity changed callback."
            )
            .def(
                "on_instance_replaced",
                (void (pyrti::PyDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const dds::core::InstanceHandle&)) &pyrti::PyDataWriterListener<T>::on_instance_replaced,
                "On instance replaced callback."
            )
            .def(
                "on_application_acknowledgment",
                (void (pyrti::PyDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const rti::pub::AcknowledgmentInfo&)) &pyrti::PyDataWriterListener<T>::on_application_acknowledgment,
                "On application acknowledgment callback"
            )
            .def(
                "on_service_request_accepted",
                (void (pyrti::PyDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const rti::core::status::ServiceRequestAcceptedStatus&)) &pyrti::PyDataWriterListener<T>::on_service_request_accepted,
                "On service request accepted callback."
            );
            /*.def(
                "on_destination_unreachable",
                &pyrti::PyDataWriterListener<T>::on_destination_unreachable,
                "On destination unreachable callback"
            )
            .def(
                "on_data_request",
                &pyrti::PyDataWriterListener<T>::on_data_request,
                "On data request callback"
            )
            .def(
                "on_data_return",
                &pyrti::PyDataWriterListener<T>::on_data_return,
                "On data return callback"
            )
            .def(
                "on_sample_removed",
                &pyrti::PyDataWriterListener<T>::on_sample_removed,
                "On sample removed callback"
            );*/
    }


    template<typename T>
    void init_class_defs(py::class_<pyrti::PyNoOpDataWriterListener<T>, pyrti::PyNoOpDataWriterListenerTrampoline<T>>& cls) {
        cls
            .def(
                py::init<>()
            )
            .def(
                "on_offered_deadline_missed",
                (void (pyrti::PyNoOpDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const dds::core::status::OfferedDeadlineMissedStatus&)) &pyrti::PyNoOpDataWriterListener<T>::on_offered_deadline_missed,
                "Offered deadline missed callback."
            )
            .def(
                "on_offered_incompatible_qos",
                (void (pyrti::PyNoOpDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const dds::core::status::OfferedIncompatibleQosStatus&)) &pyrti::PyNoOpDataWriterListener<T>::on_offered_incompatible_qos,
                "Offered incompatible QoS callback."
            )
            .def(
                "on_liveliness_lost",
                (void (pyrti::PyNoOpDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const dds::core::status::LivelinessLostStatus&)) &pyrti::PyNoOpDataWriterListener<T>::on_liveliness_lost,
                "Liveliness lost callback."
            )
            .def(
                "on_publication_matched",
                (void (pyrti::PyNoOpDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const dds::core::status::PublicationMatchedStatus&)) &pyrti::PyNoOpDataWriterListener<T>::on_publication_matched,
                "Publication matched callback."
            )
            .def(
                "on_reliable_writer_cache_changed",
                (void (pyrti::PyNoOpDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const rti::core::status::ReliableWriterCacheChangedStatus&)) &pyrti::PyNoOpDataWriterListener<T>::on_reliable_writer_cache_changed,
                "Reliable writer cache changed callback."
            )
            .def(
                "on_reliable_reader_activity_changed",
                (void (pyrti::PyNoOpDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const rti::core::status::ReliableReaderActivityChangedStatus&)) &pyrti::PyNoOpDataWriterListener<T>::on_reliable_reader_activity_changed,
                "Reliable reader activity changed callback."
            )
            .def(
                "on_instance_replaced",
                (void (pyrti::PyNoOpDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const dds::core::InstanceHandle&)) &pyrti::PyNoOpDataWriterListener<T>::on_instance_replaced,
                "On instance replaced callback."
            )
            .def(
                "on_application_acknowledgment",
                (void (pyrti::PyNoOpDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const rti::pub::AcknowledgmentInfo&)) &pyrti::PyNoOpDataWriterListener<T>::on_application_acknowledgment,
                "On application acknowledgment callback"
            )
            .def(
                "on_service_request_accepted",
                (void (pyrti::PyNoOpDataWriterListener<T>::*)(pyrti::PyDataWriter<T>&, const rti::core::status::ServiceRequestAcceptedStatus&)) &pyrti::PyNoOpDataWriterListener<T>::on_service_request_accepted,
                "On service request accepted callback."
            );
            /*.def(
                "on_destination_unreachable",
                &pyrti::PyNoOpDataWriterListener<T>::on_destination_unreachable,
                "On destination unreachable callback"
            )
            .def(
                "on_data_request",
                &pyrti::PyNoOpDataWriterListener<T>::on_data_request,
                "On data request callback"
            )
            .def(
                "on_data_return",
                &pyrti::PyNoOpDataWriterListener<T>::on_data_return,
                "On data return callback"
            )
            .def(
                "on_sample_removed",
                &pyrti::PyNoOpDataWriterListener<T>::on_sample_removed,
                "On sample removed callback"
            ); */
    }

    template<typename T>
    void init_datawriter_listener(py::object& o) {
        py::class_<pyrti::PyDataWriterListener<T>, pyrti::PyDataWriterListenerTrampoline<T>> dwl(o, "DataWriterListener");
        py::class_<pyrti::PyNoOpDataWriterListener<T>, pyrti::PyNoOpDataWriterListenerTrampoline<T>> nodwl(o, "NoOpDataWriterListener");

        pyrti::init_class_defs(dwl);
        pyrti::init_class_defs(nodwl);
    }
}

