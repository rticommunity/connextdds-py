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
#include <dds/core/status/State.hpp>
#include "PyMaskType.hpp"

using namespace dds::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<StatusMask>& cls)
{
    cls.def_property_readonly_static(
               "ALL",
               [](py::object&) {
                    return StatusMask::all();
               },
               "Create a StatusMask with all status bits set.")
            .def_property_readonly_static(
                    "NONE",
                    [](py::object&) {
                        return StatusMask::none();
                    },
                    "Create a StatusMask with no status bits set.")
            .def_property_readonly_static(
                    "INCONSISTENT_TOPIC",
                    [](py::object&) {
                        return StatusMask::inconsistent_topic();
                    },
                    "Create a StatusMask for a topic exists with the same name "
                    "but "
                    "different characteristics.")
            .def_property_readonly_static(
                    "OFFERED_DEADLINE_MISSED",
                    [](py::object&) {
                        return StatusMask::offered_deadline_missed();
                    },
                    "Create a StatusMask for a Deadline that was not respected "
                    "by a "
                    "DataWriter for a specific instance.")
            .def_property_readonly_static(
                    "REQUESTED_DEADLINE_MISSED",
                    [](py::object&) {
                        return StatusMask::requested_deadline_missed();
                    },
                    "Create a StatusMask for a Deadline was not respected for "
                    "a "
                    "DataReader for a specific instance.")
            .def_property_readonly_static(
                    "OFFERED_INCOMPATIBLE_QOS",
                    [](py::object&) {
                        return StatusMask::offered_incompatible_qos();
                    },
                    "Create a StatusMask for a QosPolicy value was "
                    "incompatible with "
                    "what was requested.")
            .def_property_readonly_static(
                    "REQUESTED_INCOMPATIBLE_QOS",
                    [](py::object&) {
                        return StatusMask::requested_incompatible_qos();
                    },
                    "Create a StatusMask for a QosPolicy value was "
                    "incompatible with "
                    "what is offered.")
            .def_property_readonly_static(
                    "SAMPLE_LOST",
                    [](py::object&) {
                        return StatusMask::sample_lost();
                    },
                    "Create a StatusMask for samples that are lost (i.e. never "
                    "received).")
            .def_property_readonly_static(
                    "SAMPLE_REJECTED",
                    [](py::object&) {
                        return StatusMask::sample_rejected();
                    },
                    "Create a StatusMask for received samples that are "
                    "rejected.")
            .def_property_readonly_static(
                    "DATA_ON_READERS",
                    [](py::object&) {
                        return StatusMask::data_on_readers();
                    },
                    "Create a StatusMask for new data available.")
            .def_property_readonly_static(
                    "DATA_AVAILABLE",
                    [](py::object&) {
                        return StatusMask::data_available();
                    },
                    "Create a StatusMask for one or more new data samples "
                    "received.")
            .def_property_readonly_static(
                    "LIVELINESS_LOST",
                    [](py::object&) {
                        return StatusMask::liveliness_lost();
                    },
                    "Create a StatusMask for a DataWriter not respecting "
                    "Liveliness, "
                    "thus DataReader entities will consider the writer as no "
                    "longer "
                    "alive.")
            .def_property_readonly_static(
                    "LIVELINESS_CHANGED",
                    [](py::object&) {
                        return StatusMask::liveliness_changed();
                    },
                    "Create a StatusMask for the liveliness of one or more "
                    "DataWriters changing from the perspective of a "
                    "DataReader.")
            .def_property_readonly_static(
                    "PUBLICATION_MATCHED",
                    [](py::object&) {
                        return StatusMask::publication_matched();
                    },
                    "Create a StatusMask for a DataWriter finding a DataReader "
                    "that "
                    "matches the Topic and has compatible QoS.")
            .def_property_readonly_static(
                    "SUBSCRIPTION_MATCHED",
                    [](py::object&) {
                        return StatusMask::subscription_matched();
                    },
                    "Create a StatusMask for a DataReader finding a DataWriter "
                    "that "
                    "matches the Topic and has compatible QoS.")
            .def_property_readonly_static(
                    "RELIABLE_WRITER_CACHE_CHANGED",
                    [](py::object&) {
                        return StatusMask::reliable_writer_cache_changed();
                    },
                    "Create a StatusMask for the number of unacknowledged "
                    "samples in "
                    "a reliable writer's cache changing such that it has "
                    "reached a "
                    "pre-defined trigger point.")
            .def_property_readonly_static(
                    "RELIABLE_READER_ACTIVITY_CHANGED",
                    [](py::object&) {
                        return StatusMask::reliable_reader_activity_changed();
                    },
                    "Create a StatusMask for one or more reliable readers "
                    "becoming "
                    "active or inactive.")
            .def_property_readonly_static(
                    "DATAWRITER_CACHE",
                    [](py::object&) {
                        return StatusMask::datawriter_cache();
                    },
                    "Create a StatusMask for the status of the writer's cache.")
            .def_property_readonly_static(
                    "DATAWRITER_PROTOCOL",
                    [](py::object&) {
                        return StatusMask::datawriter_protocol();
                    },
                    "Create a StatusMask for a writer's internal protocol "
                    "related "
                    "metrics.")
            .def_property_readonly_static(
                    "DATAREADER_CACHE",
                    [](py::object&) {
                        return StatusMask::datareader_cache();
                    },
                    "Create a StatusMask for the status of the reader's cache.")
            .def_property_readonly_static(
                    "DATAREADER_PROTOCOL",
                    [](py::object&) {
                        return StatusMask::datareader_protocol();
                    },
                    "Create a StatusMask for the status of a reader's internal "
                    "protocol related metrics.")
            .def_property_readonly_static(
                    "DATAWRITER_APPLICATION_ACKNOWLEDGMENT",
                    [](py::object&) {
                        return StatusMask::datawriter_application_acknowledgment();
                    },
                    "Create a StatusMask for a DataWriter receiving an "
                    "application-level acknowledgment for a sample.")
            .def_property_readonly_static(
                    "DATAWRITER_INSTANCE_REPLACED",
                    [](py::object&) {
                        return StatusMask::datawriter_instance_replaced();
                    },
                    "Create a StatusMask for a DataWriter instance being "
                    "replaced.")
            .def_property_readonly_static(
                    "SERVICE_REQUEST_ACCEPTED",
                    [](py::object&) {
                        return StatusMask::service_request_accepted();
                    },
                    "Create a StatusMask for a service request that has been "
                    "received for a DataWriter")
#if rti_connext_version_gte(6, 0, 0)
            .def_property_readonly_static(
                    "SAMPLE_REMOVED",
                    [](py::object&) {
                        return StatusMask::sample_removed();
                    },
                    "Create a StatusMask for a sample that has been removed "
                    "from a "
                    "DataWriter.")
            .def_property_readonly_static(
                    "DESTINATION_UNREACHABLE",
                    [](py::object&) {
                        return StatusMask::destination_unreachable();
                    },
                    "Create a StatusMask for a locator is unreachable from a "
                    "DataWriter.")
#endif
            .doc() = "A set of statuses.";
}

template<>
void process_inits<StatusMask>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        auto cls = init_mask_type<StatusMask, uint64_t>(
                m,
                "StatusMask",
                "Create a StatusMask equivalent to StatusMask.none()");
        return [cls]() mutable { init_class_defs<StatusMask>(cls); };
    });
}

}  // namespace pyrti
