#include "PyConnext.hpp"
#include <dds/core/status/Status.hpp>

using namespace rti::core::status;

namespace pyrti {

template <>
void init_class_defs(py::class_<DomainParticipantProtocolStatus> &cls) {
  cls.def_property_readonly(
         "corrupted_rtps_message_count",
         &DomainParticipantProtocolStatus::corrupted_rtps_message_count,
         "The number of corrupted RTPS messages detected by the domain "
         "participant.")
      .def_property_readonly(
          "corrupted_rtps_message_count_change",
          &DomainParticipantProtocolStatus::corrupted_rtps_message_count_change,
          "The incremental change in the number of corrupted RTPS messages "
          "detected by the domain participant since the last time the status "
          "was read.")
      .def_property_readonly(
          "last_corrupted_message_timestamp",
          &DomainParticipantProtocolStatus::last_corrupted_message_timestamp,
          "The timestamp when the last corrupted RTPS message was detected by "
          "the domain participant.")
      .doc() = "Protocol status of a DomainParticipant";
}

template <>
void process_inits<DomainParticipantProtocolStatus>(py::module &m,
                                                    ClassInitList &l) {
  l.push_back([m]() mutable {
    return init_class<DomainParticipantProtocolStatus>(
        m, "DomainParticipantProtocolStatus");
  });
}

} // namespace pyrti
