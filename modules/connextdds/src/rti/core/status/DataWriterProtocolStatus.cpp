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

#include "PyConnext.hpp"
#include <rti/core/status/Status.hpp>

using namespace rti::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<DataWriterProtocolStatus>& cls)
{
    cls.def_property_readonly(
               "pushed_sample_count",
               &DataWriterProtocolStatus::pushed_sample_count,
               "The number of user samples pushed on write from a local "
               "DataWriter to a matching remote DataReader.")
            .def_property_readonly(
                    "pushed_sample_bytes",
                    &DataWriterProtocolStatus::pushed_sample_bytes,
                    "The number of bytes of user samples pushed on write from "
                    "a local "
                    "DataWriter to a matching remote DataReader.")
            .def_property_readonly(
                    "sent_heartbeat_count",
                    &DataWriterProtocolStatus::sent_heartbeat_count,
                    "The number of Heartbeats sent between a local DataWriter "
                    "and "
                    "matching remote DataReader.")
            .def_property_readonly(
                    "sent_heartbeat_bytes",
                    &DataWriterProtocolStatus::sent_heartbeat_bytes,
                    "The number of bytes of Heartbeats sent between a local "
                    "DataWriter and matching remote DataReader.")
            .def_property_readonly(
                    "pulled_sample_count",
                    &DataWriterProtocolStatus::pulled_sample_count,
                    "The number of user samples pulled from local DataWriter "
                    "by "
                    "matching DataReaders.")
            .def_property_readonly(
                    "pulled_sample_bytes",
                    &DataWriterProtocolStatus::pulled_sample_bytes,
                    "The number of bytes of user samples pulled from local "
                    "DataWriter "
                    "by matching DataReaders.")
            .def_property_readonly(
                    "received_ack_count",
                    &DataWriterProtocolStatus::received_ack_count,
                    "The number of ACKs from a remote DataReader received by a "
                    "local "
                    "DataWriter.")
            .def_property_readonly(
                    "received_ack_bytes",
                    &DataWriterProtocolStatus::received_ack_bytes,
                    "The number of bytes of ACKs from a remote DataReader "
                    "received by "
                    "a local DataWriter.")
            .def_property_readonly(
                    "received_nack_count",
                    &DataWriterProtocolStatus::received_nack_count,
                    "The number of NACKs from a remote DataReader received by "
                    "a local "
                    "DataWriter.")
            .def_property_readonly(
                    "received_nack_bytes",
                    &DataWriterProtocolStatus::received_nack_bytes,
                    "The number of bytes of NACKs from a remote DataReader "
                    "received "
                    "by a local DataWriter.")
            .def_property_readonly(
                    "sent_gap_count",
                    &DataWriterProtocolStatus::sent_gap_count,
                    "The number of GAPs sent from local DataWriter to matching "
                    "remote "
                    "DataReaders.")
            .def_property_readonly(
                    "sent_gap_bytes",
                    &DataWriterProtocolStatus::sent_gap_bytes,
                    "The number of bytes of GAPs sent from local DataWriter to "
                    "matching remote DataReaders.")
            .def_property_readonly(
                    "rejected_sample_count",
                    &DataWriterProtocolStatus::rejected_sample_count,
                    "The number of times a sample is rejected due to "
                    "exceptions in "
                    "the send path.")
            .def_property_readonly(
                    "send_window_size",
                    &DataWriterProtocolStatus::send_window_size,
                    "Current maximum number of outstanding samples allowed in "
                    "the "
                    "DataWriter's queue.")
            .def_property_readonly(
                    "first_available_sample_sequence_number",
                    &DataWriterProtocolStatus::
                            first_available_sample_sequence_number,
                    "The sequence number of the first available sample "
                    "currently "
                    "queued in the local DataWriter.")
            .def_property_readonly(
                    "last_available_sample_sequence_number",
                    &DataWriterProtocolStatus::
                            last_available_sample_sequence_number,
                    "The sequence number of the last available sample "
                    "currently "
                    "queued in the local DataWriter.")
            .def_property_readonly(
                    "first_unacknowledged_sample_sequence_number",
                    &DataWriterProtocolStatus::
                            first_unacknowledged_sample_sequence_number,
                    "The sequence number of the first unacknowledged sample "
                    "currently "
                    "queued in the local DataWriter.")
            .def_property_readonly(
                    "first_available_sample_virtual_sequence_number",
                    &DataWriterProtocolStatus::
                            first_available_sample_virtual_sequence_number,
                    "The virtual sequence number of the first available sample "
                    "currently queued in the local DataWriter.")
            .def_property_readonly(
                    "last_available_sample_virtual_sequence_number",
                    &DataWriterProtocolStatus::
                            last_available_sample_virtual_sequence_number,
                    "The virtual sequence number of the last available sample "
                    "currently queued in the local DataWriter.")
            .def_property_readonly(
                    "first_unacknowledged_sample_virtual_sequence_number",
                    &DataWriterProtocolStatus::
                            first_unacknowledged_sample_virtual_sequence_number,
                    "The virtual sequence number of the first unacknowledged "
                    "sample "
                    "currently queued in the local DataWriter.")
            .def_property_readonly(
                    "first_unacknowledged_sample_subscription_handle",
                    &DataWriterProtocolStatus::
                            first_unacknowledged_sample_subscription_handle,
                    "The handle of a remote DataReader that has not "
                    "acknowledged the "
                    "first unacknowledged sample of the local DataWriter.")
            .def_property_readonly(
                    "first_unelapsed_keep_duration_sample_sequence_number",
                    &DataWriterProtocolStatus::
                            first_unelapsed_keep_duration_sample_sequence_number,
                    "The sequence number of the first sample whose keep "
                    "duration has "
                    "not yet elapsed.")
#if rti_connext_version_gte(6, 1, 0)
            .def_property_readonly(
                    "pushed_fragment_count",
                    &DataWriterProtocolStatus::
                            pushed_fragment_count,
                    "The number of DATA_FRAG messages that have been "
                    "pushed by this DataWriter.")
            .def_property_readonly(
                    "pushed_fragment_bytes",
                    &DataWriterProtocolStatus::
                            pushed_fragment_bytes,
                    "The number of bytes of DATA_FRAG messages that have been "
                    "pushed by this DataWriter.")
            .def_property_readonly(
                    "pulled_fragment_count",
                    &DataWriterProtocolStatus::
                            pulled_fragment_count,
                    "The number of DATA_FRAG messages that have been pulled "
                    "from this DataWriter.")
            .def_property_readonly(
                    "pulled_fragment_bytes",
                    &DataWriterProtocolStatus::
                            pulled_fragment_bytes,
                    "The number of bytes of DATA_FRAG messages that have been pulled "
                    "from this DataWriter.")
            .def_property_readonly(
                    "received_nack_fragment_count",
                    &DataWriterProtocolStatus::
                            received_nack_fragment_count,
                    "The number of NACK_FRAG messages that have been "
                    "received by this DataWriter.")
            .def_property_readonly(
                    "received_nack_fragment_bytes",
                    &DataWriterProtocolStatus::
                            received_nack_fragment_bytes,
                    "The number of bytes of NACK_FRAG messages that have been "
                    "received by this DataWriter.")
#endif
            .doc() = "Information about the DataWriter protocol status";
}

template<>
void process_inits<DataWriterProtocolStatus>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DataWriterProtocolStatus>(
                m,
                "DataWriterProtocolStatus");
    });
}

}  // namespace pyrti