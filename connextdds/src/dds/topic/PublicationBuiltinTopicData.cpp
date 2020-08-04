#include "PyConnext.hpp"
#include "PyInitOpaqueTypeContainers.hpp"
#include "PyInitType.hpp"
#include "PySeq.hpp"
#include <dds/topic/BuiltinTopic.hpp>

using namespace dds::topic;

INIT_OPAQUE_TYPE_CONTAINERS(dds::topic::PublicationBuiltinTopicData);

namespace pyrti {

template <>
void init_dds_typed_topic_template(
    py::class_<PyTopic<dds::topic::PublicationBuiltinTopicData>,
               PyITopicDescription<dds::topic::PublicationBuiltinTopicData>,
               PyIAnyTopic> &cls) {
  init_dds_typed_topic_base_template(cls);
}

template <> void init_class_defs(py::class_<PublicationBuiltinTopicData> &cls) {
  cls.def(py::init<>(), "Create a default PublicationBuiltinTopicData.")
      .def_property_readonly("key", &PublicationBuiltinTopicData::key,
                             "The DCPS key to distinguish entries.")
      .def_property_readonly(
          "participant_key", &PublicationBuiltinTopicData::participant_key,
          "The DCPS key of the DomainParticipant to which the "
          "DataWriter belongs.")
      .def_property_readonly("topic_name",
                             &PublicationBuiltinTopicData::topic_name,
                             "The name of the related Topic.")
      .def_property_readonly(
          "type_name",
          [](const PublicationBuiltinTopicData &data) {
            return data.type_name().to_std_string();
          },
          "The name of the type attached to the Topic.")
      .def_property_readonly(
          "durability", &PublicationBuiltinTopicData::durability,
          "The Durability policy of the corresponding DataWriter.")
      .def_property_readonly(
          "durability_service",
          &PublicationBuiltinTopicData::durability_service,
          "The DurabilityService policy of the corresponding DataWriter.")
      .def_property_readonly(
          "deadline", &PublicationBuiltinTopicData::deadline,
          "The Deadline policy of the corresponding DataWriter.")
      .def_property_readonly(
          "latency_budget", &PublicationBuiltinTopicData::latency_budget,
          "The LatencyBudget policy of the corresponding DataWriter.")
      .def_property_readonly(
          "liveliness", &PublicationBuiltinTopicData::liveliness,
          "The Liveliness policy of the corresponding DataWriter.")
      .def_property_readonly(
          "reliability", &PublicationBuiltinTopicData::reliability,
          "The Reliability policy of the corresponding DataWriter.")
      .def_property_readonly(
          "lifespan", &PublicationBuiltinTopicData::lifespan,
          "The Lifespan policy of the corresponding DataWriter.")
      .def_property_readonly(
          "user_data", &PublicationBuiltinTopicData::user_data,
          "The UserData policy of the corresponding DataWriter.")
      .def_property_readonly(
          "ownership", &PublicationBuiltinTopicData::ownership,
          "The Ownership policy of the corresponding DataWriter.")
      .def_property_readonly(
          "ownership_strength",
          &PublicationBuiltinTopicData::ownership_strength,
          "The OwnershipStrength policy of the corresponding DataWriter.")
      .def_property_readonly(
          "destination_order", &PublicationBuiltinTopicData::destination_order,
          "The DestinationOrder policy of the corresponding DataWriter.")
      .def_property_readonly(
          "presentation", &PublicationBuiltinTopicData::presentation,
          "The Presentation policy of the corresponding DataWriter.")
      .def_property_readonly(
          "partition", &PublicationBuiltinTopicData::partition,
          "The Partition policy of the corresponding DataWriter.")
      .def_property_readonly(
          "topic_data", &PublicationBuiltinTopicData::topic_data,
          "The TopicData policy of the corresponding DataWriter.")
      .def_property_readonly(
          "group_data", &PublicationBuiltinTopicData::group_data,
          "The GroupData policy of the corresponding DataWriter.")
#if rti_connext_version_gte(6, 0, 0)
      .def_property_readonly(
          "representation", &PublicationBuiltinTopicData::representation,
          "The Representation policy of the corresponding DataWriter.")
      // BUG: data_tag() is not compatible with default delegate
      .def_property_readonly(
          "data_tag",
          [](const PublicationBuiltinTopicData &p) { return p->data_tags(); },
          "The DataTag policy of the corresponding DataWriter.")
#endif
      .def_property_readonly(
          "type",
          [](const PublicationBuiltinTopicData &p) { return p->type(); },
          "The type.")
      .def_property_readonly(
          "publisher_key",
          [](const PublicationBuiltinTopicData &p) {
            return p->publisher_key();
          },
          "The DCPS key of the Publisher to which the DataWriter "
          "belongs.")
      .def_property_readonly(
          "property",
          [](const PublicationBuiltinTopicData &p) { return p->property(); },
          "The propagated name-value properties of the corresponding "
          "DataWriter.")
      .def_property_readonly(
          "unicast_locators",
          [](const PublicationBuiltinTopicData &p) {
            return p->unicast_locators();
          },
          "The custom unicast locators that the endpoint can specify.")
      .def_property_readonly(
          "virtual_guid",
          [](const PublicationBuiltinTopicData &p) {
            return p->virtual_guid();
          },
          "The virtual Guid associated to the DataWriter.")
      .def_property_readonly(
          "rtps_protocol_version",
          [](const PublicationBuiltinTopicData &p) {
            return p->rtps_protocol_version();
          },
          "The version number of the RTPS wire protocol used.")
      .def_property_readonly(
          "rtps_vendor_id",
          [](const PublicationBuiltinTopicData &p) {
            return p->rtps_vendor_id();
          },
          "The ID of the vendor implementing the RTPS wire protocol.")
      .def_property_readonly(
          "product_version",
          [](const PublicationBuiltinTopicData &p) {
            return p->product_version();
          },
          "The current version for RTI Connext.")
      .def_property_readonly(
          "locator_filter",
          [](const PublicationBuiltinTopicData &p) {
            return p->locator_filter();
          },
          "The locator filters of the corresponding DataWriter.")
      .def_property_readonly(
          "disable_positive_acks",
          [](const PublicationBuiltinTopicData &p) {
            return p->disable_positive_acks();
          },
          "See whether or not a matching DataReader will send positive "
          "acknowledgments for reliability.")
      .def_property_readonly(
          "publication_name",
          [](const PublicationBuiltinTopicData &p) {
            return p->publication_name();
          },
          "The publication name and role name.")
      .def_property_readonly(
          "service",
          [](const PublicationBuiltinTopicData &p) { return p->service(); },
          "The Service policy")
      .def_property_readonly_static(
          "topic_name",
          [](py::object &) { return dds::topic::publication_topic_name(); },
          "Publication builtin topic name.")
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.");
}

template <>
void process_inits<PublicationBuiltinTopicData>(py::module &m,
                                                ClassInitList &l) {
  l.push_back([m, &l]() mutable {
    return init_type_class<PublicationBuiltinTopicData>(
        m, l, "PublicationBuiltinTopicData");
  });
}

} // namespace pyrti
