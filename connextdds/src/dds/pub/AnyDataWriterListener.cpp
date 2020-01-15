#include "PyConnext.hpp"
#include "PyAnyDataWriterListener.hpp"

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyAnyDataWriterListener, pyrti::PyAnyDataWriterListenerTrampoline<>>& cls) {
    cls
        .def(
            py::init<>()
        )
        .def(
            "on_offered_deadline_missed",
            (void (pyrti::PyAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const dds::core::status::OfferedDeadlineMissedStatus&)) &pyrti::PyAnyDataWriterListener::on_offered_deadline_missed,
            "Offered deadline missed callback."
        )
        .def(
            "on_offered_incompatible_qos",
            (void (pyrti::PyAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const dds::core::status::OfferedIncompatibleQosStatus&)) &pyrti::PyAnyDataWriterListener::on_offered_incompatible_qos,
            "Offered incompatible QoS callback."
        )
        .def(
            "on_liveliness_lost",
            (void (pyrti::PyAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const dds::core::status::LivelinessLostStatus&)) &pyrti::PyAnyDataWriterListener::on_liveliness_lost,
            "Liveliness lost callback."
        )
        .def(
            "on_publication_matched",
            (void (pyrti::PyAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const dds::core::status::PublicationMatchedStatus&)) &pyrti::PyAnyDataWriterListener::on_publication_matched,
            "Publication matched callback."
        )
        .def(
            "on_reliable_writer_cache_changed",
            (void (pyrti::PyAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const rti::core::status::ReliableWriterCacheChangedStatus&)) &pyrti::PyAnyDataWriterListener::on_reliable_writer_cache_changed,
            "Reliable writer cache changed callback."
        )
        .def(
            "on_reliable_reader_activity_changed",
            (void (pyrti::PyAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const rti::core::status::ReliableReaderActivityChangedStatus&)) &pyrti::PyAnyDataWriterListener::on_reliable_reader_activity_changed,
            "Reliable reader activity changed callback."
        )
        .def(
            "on_instance_replaced",
            (void (pyrti::PyAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const dds::core::InstanceHandle&)) &pyrti::PyAnyDataWriterListener::on_instance_replaced,
            "On instance replaced callback."
        )
        .def(
            "on_application_acknowledgment",
            (void (pyrti::PyAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const rti::pub::AcknowledgmentInfo&)) &pyrti::PyAnyDataWriterListener::on_application_acknowledgment,
            "On application acknowledgment callback"
        )
        .def(
            "on_service_request_accepted",
            (void (pyrti::PyAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const rti::core::status::ServiceRequestAcceptedStatus&)) &pyrti::PyAnyDataWriterListener::on_service_request_accepted,
            "On service request accepted callback."
        );
}

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyNoOpAnyDataWriterListener, pyrti::PyAnyDataWriterListener, pyrti::PyNoOpAnyDataWriterListenerTrampoline<>>& cls) {
    cls
        .def(
            py::init<>()
        )
        .def(
            "on_offered_deadline_missed",
            (void (pyrti::PyNoOpAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const dds::core::status::OfferedDeadlineMissedStatus&)) &pyrti::PyNoOpAnyDataWriterListener::on_offered_deadline_missed,
            "Offered deadline missed callback."
        )
        .def(
            "on_offered_incompatible_qos",
            (void (pyrti::PyNoOpAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const dds::core::status::OfferedIncompatibleQosStatus&)) &pyrti::PyNoOpAnyDataWriterListener::on_offered_incompatible_qos,
            "Offered incompatible QoS callback."
        )
        .def(
            "on_liveliness_lost",
            (void (pyrti::PyNoOpAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const dds::core::status::LivelinessLostStatus&)) &pyrti::PyNoOpAnyDataWriterListener::on_liveliness_lost,
            "Liveliness lost callback."
        )
        .def(
            "on_publication_matched",
            (void (pyrti::PyNoOpAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const dds::core::status::PublicationMatchedStatus&)) &pyrti::PyNoOpAnyDataWriterListener::on_publication_matched,
            "Publication matched callback."
        )
        .def(
            "on_reliable_writer_cache_changed",
            (void (pyrti::PyNoOpAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const rti::core::status::ReliableWriterCacheChangedStatus&)) &pyrti::PyNoOpAnyDataWriterListener::on_reliable_writer_cache_changed,
            "Reliable writer cache changed callback."
        )
        .def(
            "on_reliable_reader_activity_changed",
            (void (pyrti::PyNoOpAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const rti::core::status::ReliableReaderActivityChangedStatus&)) &pyrti::PyNoOpAnyDataWriterListener::on_reliable_reader_activity_changed,
            "Reliable reader activity changed callback."
        )
        .def(
            "on_instance_replaced",
            (void (pyrti::PyNoOpAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const dds::core::InstanceHandle&)) &pyrti::PyNoOpAnyDataWriterListener::on_instance_replaced,
            "On instance replaced callback."
        )
        .def(
            "on_application_acknowledgment",
            (void (pyrti::PyNoOpAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const rti::pub::AcknowledgmentInfo&)) &pyrti::PyNoOpAnyDataWriterListener::on_application_acknowledgment,
            "On application acknowledgment callback"
        )
        .def(
            "on_service_request_accepted",
            (void (pyrti::PyNoOpAnyDataWriterListener::*)(pyrti::PyAnyDataWriter&, const rti::core::status::ServiceRequestAcceptedStatus&)) &pyrti::PyNoOpAnyDataWriterListener::on_service_request_accepted,
            "On service request accepted callback."
        );
}

template<>
void pyrti::process_inits<dds::pub::AnyDataWriterListener>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyAnyDataWriterListener, pyrti::PyAnyDataWriterListenerTrampoline<>>(m, "AnyDataWriterListener");
        }
    );

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyNoOpAnyDataWriterListener, pyrti::PyAnyDataWriterListener, pyrti::PyNoOpAnyDataWriterListenerTrampoline<>>(m, "NoOpAnyDataWriterListener");
        }
    );
}
