#include "PyConnext.hpp"
#include "PySeq.hpp"
#include <dds/topic/BuiltinTopic.hpp>
#include "PyInitType.hpp"
#include "PyInitOpaqueTypeContainers.hpp"

using namespace dds::topic;

INIT_OPAQUE_TYPE_CONTAINERS(dds::topic::TopicBuiltinTopicData);

namespace pyrti {

template<>
void init_dds_typed_topic_template(
        py::class_<
                PyTopic<dds::topic::TopicBuiltinTopicData>,
                PyITopicDescription<dds::topic::TopicBuiltinTopicData>,
                PyIAnyTopic>& cls)
{
    init_dds_typed_topic_base_template(cls);
}

template<>
void init_class_defs(py::class_<TopicBuiltinTopicData>& cls)
{
    cls.def(py::init<>(), "Create a default TopicBuiltinTopicData.")
            .def_property_readonly(
                    "key",
                    &TopicBuiltinTopicData::key,
                    "Get the DCPS key to distinguish entries.")
            .def_property_readonly(
                    "name",
                    [](const TopicBuiltinTopicData& data) {
                        return data.name().to_std_string();
                    },
                    "Get the name of the Topic.")
            .def_property_readonly(
                    "type_name",
                    [](const TopicBuiltinTopicData& data) {
                        return data.type_name().to_std_string();
                    },
                    "Get the name of the type attached to the Topic.")
            .def_property_readonly(
                    "durability",
                    &TopicBuiltinTopicData::durability,
                    "Get the Durability policy of the corresponding Topic.")
            .def_property_readonly(
                    "durability_service",
                    &TopicBuiltinTopicData::durability_service,
                    "Get the DurabilityService policy of the corresponding "
                    "Topic.")
            .def_property_readonly(
                    "deadline",
                    &TopicBuiltinTopicData::deadline,
                    "Get the Deadline policy of the corresponding Topic.")
            .def_property_readonly(
                    "latency_budget",
                    &TopicBuiltinTopicData::latency_budget,
                    "Get the LatencyBudget policy of the corresponding Topic.")
            .def_property_readonly(
                    "liveliness",
                    &TopicBuiltinTopicData::liveliness,
                    "Get the Liveliness policy of the corresponding Topic.")
            .def_property_readonly(
                    "reliability",
                    &TopicBuiltinTopicData::reliability,
                    "Get the Reliability policy of the corresponding Topic.")
            .def_property_readonly(
                    "transport_priority",
                    &TopicBuiltinTopicData::transport_priority,
                    "Get the TransportPriority policy of the corresponding "
                    "Topic.")
            .def_property_readonly(
                    "lifespan",
                    &TopicBuiltinTopicData::lifespan,
                    "Get the Lifespan policy of the corresponding Topic.")
            .def_property_readonly(
                    "destination_order",
                    &TopicBuiltinTopicData::destination_order,
                    "Get the DestinationOrder policy of the corresponding "
                    "Topic.")
            .def_property_readonly(
                    "history",
                    &TopicBuiltinTopicData::history,
                    "Get the History policy of the corresponding Topic.")
            .def_property_readonly(
                    "resource_limits",
                    &TopicBuiltinTopicData::resource_limits,
                    "Get the ResourceLimits policy of the corresponding Topic.")
            .def_property_readonly(
                    "ownership",
                    &TopicBuiltinTopicData::ownership,
                    "Get the Ownership policy of the corresponding Topic.")
            .def_property_readonly(
                    "topic_data",
                    &TopicBuiltinTopicData::topic_data,
                    "Get the TopicData policy of the corresponding Topic.")
#if rti_connext_version_gte(6, 0, 0)
            .def_property_readonly(
                    "representation",
                    &TopicBuiltinTopicData::representation,
                    "Get the DataRepresentation policy of the corresponding "
                    "Topic.")
#endif
            .def_property_readonly_static(
                    "topic_name",
                    [](py::object&) { return dds::topic::topic_topic_name(); },
                    "Topic builtin topic name.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<TopicBuiltinTopicData>(py::module& m, ClassInitList& l)
{
    l.push_back([m, &l]() mutable {
        return init_type_class<TopicBuiltinTopicData>(
                m,
                l,
                "TopicBuiltinTopicData");
    });
}

}  // namespace pyrti
