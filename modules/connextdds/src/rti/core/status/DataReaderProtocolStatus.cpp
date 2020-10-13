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
void init_class_defs(py::class_<DataReaderProtocolStatus>& cls)
{
    cls.def_property_readonly(
               "received_sample_count",
               &DataReaderProtocolStatus::received_sample_count,
               "The number of samples received by a DataReader.")
            .def_property_readonly(
                    "received_sample_bytes",
                    &DataReaderProtocolStatus::received_sample_bytes,
                    "The number of bytes from samples received by a "
                    "DataReader.")
            .def_property_readonly(
                    "duplicate_sample_count",
                    &DataReaderProtocolStatus::duplicate_sample_count,
                    "The number of samples from a remote DataWriter received, "
                    "not for "
                    "the first time, by a local DataReader.")
            .def_property_readonly(
                    "duplicate_sample_bytes",
                    &DataReaderProtocolStatus::duplicate_sample_bytes,
                    "The number of bytes of sample data from a remote "
                    "DataWriter "
                    "received, not for the first time, by a local DataReader.")
            .def_property_readonly(
                    "received_heartbeat_count",
                    &DataReaderProtocolStatus::received_heartbeat_count,
                    "The number of Heartbeats from a remote DataWriter "
                    "received by a "
                    "local DataReader.")
            .def_property_readonly(
                    "received_heartbeat_bytes",
                    &DataReaderProtocolStatus::received_heartbeat_bytes,
                    "The number of bytes of Heartbeat data from a remote "
                    "DataWriter "
                    "received by a local DataReader.")
            .def_property_readonly(
                    "sent_ack_count",
                    &DataReaderProtocolStatus::sent_ack_count,
                    "The number of ACKs sent from a local DataReader to a "
                    "matching "
                    "remote DataWriter.")
            .def_property_readonly(
                    "sent_ack_bytes",
                    &DataReaderProtocolStatus::sent_ack_bytes,
                    "The number of bytes of ACK data sent from a local "
                    "DataReader to "
                    "a matching remote DataWriter.")
            .def_property_readonly(
                    "sent_nack_count",
                    &DataReaderProtocolStatus::sent_nack_count,
                    "The number of NACKs sent from a local DataReader to a "
                    "matching "
                    "remote DataWriter.")
            .def_property_readonly(
                    "sent_nack_bytes",
                    &DataReaderProtocolStatus::sent_nack_bytes,
                    "The number of bytes of NACK data sent from a local "
                    "DataReader to "
                    "a matching remote DataWriter.")
            .def_property_readonly(
                    "received_gap_count",
                    &DataReaderProtocolStatus::received_gap_count,
                    "The number of GAPs received from remote DataWriter to "
                    "this "
                    "DataReader.")
            .def_property_readonly(
                    "received_gap_bytes",
                    &DataReaderProtocolStatus::received_gap_bytes,
                    "The number of bytes of GAP data received from remote "
                    "DataWriter "
                    "to this DataReader.")
            .def_property_readonly(
                    "rejected_sample_count",
                    &DataReaderProtocolStatus::rejected_sample_count,
                    "The number of times samples were rejected due to "
                    "exceptions in "
                    "the receive path.")
            .def_property_readonly(
                    "first_available_sample_sequence_number",
                    &DataReaderProtocolStatus::
                            first_available_sample_sequence_number,
                    "Sequence number of the first available sample in a "
                    "matched "
                    "DataWriters reliability queue.")
            .def_property_readonly(
                    "last_available_sample_sequence_number",
                    &DataReaderProtocolStatus::
                            last_available_sample_sequence_number,
                    "Sequence number of the last available sample in a matched "
                    "Datawriter's reliability queue.")
            .def_property_readonly(
                    "last_committed_sample_sequence_number",
                    &DataReaderProtocolStatus::
                            last_committed_sample_sequence_number,
                    "Sequence number of the newest sample received from the "
                    "matched "
                    "DataWriter committed to the DataReader's queue.")
            .def_property_readonly(
                    "uncommitted_sample_count",
                    &DataReaderProtocolStatus::uncommitted_sample_count,
                    "Number of received samples that are not yet available to "
                    "be read "
                    "or taken, due to being received out of order.")
            .doc() = "Information about the DataReader's protocol status.";
}

template<>
void process_inits<DataReaderProtocolStatus>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DataReaderProtocolStatus>(
                m,
                "DataReaderProtocolStatus");
    });
}

}  // namespace pyrti
