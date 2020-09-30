#include "PyConnext.hpp"
#include <dds/core/status/State.hpp>
#include "PyMaskType.hpp"

using namespace dds::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<StatusMask>& cls)
{
    cls.def_static(
               "all",
               &StatusMask::all,
               "Create a StatusMask with all status bits set.")
            .def_static(
                    "none",
                    &StatusMask::none,
                    "Create a StatusMask with no status bits set.")
            .def_static(
                    "inconsistent_topic",
                    &StatusMask::inconsistent_topic,
                    "Create a StatusMask for a topic exists with the same name "
                    "but "
                    "different characteristics.")
            .def_static(
                    "offered_deadline_missed",
                    &StatusMask::offered_deadline_missed,
                    "Create a StatusMask for a Deadline that was not respected "
                    "by a "
                    "DataWriter for a specific instance.")
            .def_static(
                    "requested_deadline_missed",
                    &StatusMask::requested_deadline_missed,
                    "Create a StatusMask for a Deadline was not respected for "
                    "a "
                    "DataReader for a specific instance.")
            .def_static(
                    "offered_incompatible_qos",
                    &StatusMask::offered_incompatible_qos,
                    "Create a StatusMask for a QosPolicy value was "
                    "incompatible with "
                    "what was requested.")
            .def_static(
                    "requested_incompatible_qos",
                    &StatusMask::requested_incompatible_qos,
                    "Create a StatusMask for a QosPolicy value was "
                    "incompatible with "
                    "what is offered.")
            .def_static(
                    "sample_lost",
                    &StatusMask::sample_lost,
                    "Create a StatusMask for samples that are lost (i.e. never "
                    "received).")
            .def_static(
                    "sample_rejected",
                    &StatusMask::sample_rejected,
                    "Create a StatusMask for received samples that are "
                    "rejected.")
            .def_static(
                    "data_on_readers",
                    &StatusMask::data_on_readers,
                    "Create a StatusMask for new data available.")
            .def_static(
                    "data_available",
                    &StatusMask::data_available,
                    "Create a StatusMask for one or more new data samples "
                    "received.")
            .def_static(
                    "liveliness_lost",
                    &StatusMask::liveliness_lost,
                    "Create a StatusMask for a DataWriter not respecting "
                    "Liveliness, "
                    "thus DataReader entities will consider the writer as no "
                    "longer "
                    "alive.")
            .def_static(
                    "liveliness_changed",
                    &StatusMask::liveliness_changed,
                    "Create a StatusMask for the liveliness of one or more "
                    "DataWriters changing from the perspective of a "
                    "DataReader.")
            .def_static(
                    "publication_matched",
                    &StatusMask::publication_matched,
                    "Create a StatusMask for a DataWriter finding a DataReader "
                    "that "
                    "matches the Topic and has compatible QoS.")
            .def_static(
                    "subscription_matched",
                    &StatusMask::subscription_matched,
                    "Create a StatusMask for a DataReader finding a DataWriter "
                    "that "
                    "matches the Topic and has compatible QoS.")
            .def_static(
                    "reliable_writer_cache_changed",
                    &StatusMask::reliable_writer_cache_changed,
                    "Create a StatusMask for the number of unacknowledged "
                    "samples in "
                    "a reliable writer's cache changing such that it has "
                    "reached a "
                    "pre-defined trigger point.")
            .def_static(
                    "reliable_reader_activity_changed",
                    &StatusMask::reliable_reader_activity_changed,
                    "Create a StatusMask for one or more reliable readers "
                    "becoming "
                    "active or inactive.")
            .def_static(
                    "datawriter_cache",
                    &StatusMask::datawriter_cache,
                    "Create a StatusMask for the status of the writer's cache.")
            .def_static(
                    "datawriter_protocol",
                    &StatusMask::datawriter_protocol,
                    "Create a StatusMask for a writer's internal protocol "
                    "related "
                    "metrics.")
            .def_static(
                    "datareader_cache",
                    &StatusMask::datareader_cache,
                    "Create a StatusMask for the status of the reader's cache.")
            .def_static(
                    "datareader_protocol",
                    &StatusMask::datareader_protocol,
                    "Create a StatusMask for the status of a reader's internal "
                    "protocol related metrics.")
            .def_static(
                    "datawriter_application_acknowledgment",
                    &StatusMask::datawriter_application_acknowledgment,
                    "Create a StatusMask for a DataWriter receiving an "
                    "application-level acknowledgment for a sample.")
            .def_static(
                    "datawriter_instance_replaced",
                    &StatusMask::datawriter_instance_replaced,
                    "Create a StatusMask for a DataWriter instance being "
                    "replaced.")
            .def_static(
                    "service_request_accepted",
                    &StatusMask::service_request_accepted,
                    "Create a StatusMask for a service request that has been "
                    "received for a DataWriter")
#if rti_connext_version_gte(6, 0, 0)
            .def_static(
                    "sample_removed",
                    &StatusMask::sample_removed,
                    "Create a StatusMask for a sample that has been removed "
                    "from a "
                    "DataWriter.")
            .def_static(
                    "destination_unreachable",
                    &StatusMask::destination_unreachable,
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
