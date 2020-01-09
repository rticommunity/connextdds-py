#include "PyConnext.hpp"
#include <rti/core/status/Status.hpp>

using namespace rti::core::status;

template<>
void pyrti::init_class_defs(py::class_<DataWriterProtocolStatus>& cls) {
    cls
        .def_property_readonly(
            "pushed_sample_count",
            &DataWriterProtocolStatus::pushed_sample_count,
            "The number of user samples pushed on write from a local "
            "DataWriter to a matching remote DataReader."
        )
        .def_property_readonly(
            "pushed_sample_bytes",
            &DataWriterProtocolStatus::pushed_sample_bytes,
            "The number of bytes of user samples pushed on write from a local "
            "DataWriter to a matching remote DataReader."
        )
        .def_property_readonly(
            "sent_heartbeat_count",
            &DataWriterProtocolStatus::sent_heartbeat_count,
            "The number of Heartbeats sent between a local DataWriter and "
            "matching remote DataReader."
        )
        .def_property_readonly(
            "sent_heartbeat_bytes",
            &DataWriterProtocolStatus::sent_heartbeat_bytes,
            "The number of bytes of Heartbeats sent between a local "
            "DataWriter and matching remote DataReader."
        )
        .def_property_readonly(
            "pulled_sample_count",
            &DataWriterProtocolStatus::pulled_sample_count,
            "The number of user samples pulled from local DataWriter by "
            "matching DataReaders."
        )
        .def_property_readonly(
            "pulled_sample_bytes",
            &DataWriterProtocolStatus::pulled_sample_bytes,
            "The number of bytes of user samples pulled from local DataWriter "
            "by matching DataReaders."
        )
        .def_property_readonly(
            "received_ack_count",
            &DataWriterProtocolStatus::received_ack_count,
            "The number of ACKs from a remote DataReader received by a local "
            "DataWriter."
        )
        .def_property_readonly(
            "received_ack_bytes",
            &DataWriterProtocolStatus::received_ack_bytes,
            "The number of bytes of ACKs from a remote DataReader received by "
            "a local DataWriter."
        )
        .def_property_readonly(
            "received_nack_count",
            &DataWriterProtocolStatus::received_nack_count,
            "The number of NACKs from a remote DataReader received by a local "
            "DataWriter."
        )
        .def_property_readonly(
            "received_nack_bytes",
            &DataWriterProtocolStatus::received_nack_bytes,
            "The number of bytes of NACKs from a remote DataReader received "
            "by a local DataWriter."
        )
        .def_property_readonly(
            "sent_gap_count",
            &DataWriterProtocolStatus::sent_gap_count,
            "The number of GAPs sent from local DataWriter to matching remote "
            "DataReaders."
        )
        .def_property_readonly(
            "sent_gap_bytes",
            &DataWriterProtocolStatus::sent_gap_bytes,
            "The number of bytes of GAPs sent from local DataWriter to "
            "matching remote DataReaders."
        )
        .def_property_readonly(
            "rejected_sample_count",
            &DataWriterProtocolStatus::rejected_sample_count,
            "The number of times a sample is rejected due to exceptions in "
            "the send path."
        )
        .def_property_readonly(
            "send_window_size",
            &DataWriterProtocolStatus::send_window_size,
            "Current maximum number of outstanding samples allowed in the "
            "DataWriter's queue."
        )
        .def_property_readonly(
            "first_available_sample_sequence_number",
            &DataWriterProtocolStatus::first_available_sample_sequence_number,
            "The sequence number of the first available sample currently "
            "queued in the local DataWriter."
        )
        .def_property_readonly(
            "last_available_sample_sequence_number",
            &DataWriterProtocolStatus::last_available_sample_sequence_number,
            "The sequence number of the last available sample currently "
            "queued in the local DataWriter."
        )
        .def_property_readonly(
            "first_unacknowledged_sample_sequence_number",
            &DataWriterProtocolStatus::first_unacknowledged_sample_sequence_number,
            "The sequence number of the first unacknowledged sample currently "
            "queued in the local DataWriter."
        )
        .def_property_readonly(
            "first_available_sample_virtual_sequence_number",
            &DataWriterProtocolStatus::first_available_sample_virtual_sequence_number,
            "The virtual sequence number of the first available sample "
            "currently queued in the local DataWriter."
        )
        .def_property_readonly(
            "last_available_sample_virtual_sequence_number",
            &DataWriterProtocolStatus::last_available_sample_virtual_sequence_number,
            "The virtual sequence number of the last available sample "
            "currently queued in the local DataWriter."
        )
        .def_property_readonly(
            "first_unacknowledged_sample_virtual_sequence_number",
            &DataWriterProtocolStatus::first_unacknowledged_sample_virtual_sequence_number,
            "The virtual sequence number of the first unacknowledged sample "
            "currently queued in the local DataWriter."
        )
        .def_property_readonly(
            "first_unacknowledged_sample_subscription_handle",
            &DataWriterProtocolStatus::first_unacknowledged_sample_subscription_handle,
            "The handle of a remote DataReader that has not acknowledged the "
            "first unacknowledged sample of the local DataWriter."
        )
        .def_property_readonly(
            "first_unelapsed_keep_duration_sample_sequence_number",
            &DataWriterProtocolStatus::first_unelapsed_keep_duration_sample_sequence_number,
            "The sequence number of the first sample whose keep duration has "
            "not yet elapsed."
        )
        .doc() = "Information about the DataWriter protocol status";
}

template<>
void pyrti::process_inits<DataWriterProtocolStatus>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<DataWriterProtocolStatus>(m, "DataWriterProtocolStatus");
        }
    );
}