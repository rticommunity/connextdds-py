#include "PyConnext.hpp"
#include "PyQos.hpp"
#include <dds/topic/qos/TopicQos.hpp>

using namespace dds::topic::qos;
using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<TopicQos>& cls)
{
    cls.def(py::init<>(),
            "Create a TopicQos with the default value for each policy.")
            .def(py::init([](const PyIAnyTopic& topic) {
                     return topic.py_qos();
                 }),
                 py::arg("topic"),
                 "Create a TopicQos with settings equivalent to those of the "
                 "provided Topic.")
            .def(py::self == py::self, "Test for equality")
            .def(py::self != py::self, "Test for inequality.");

    add_qos_property<TopicQos, TopicData>(cls, "topic_data", "TopicData");
    add_qos_property<TopicQos, Durability>(cls, "durability", "Durability");
    add_qos_property<TopicQos, DurabilityService>(
            cls,
            "durability_service",
            "DurabilityService");
    add_qos_property<TopicQos, Deadline>(cls, "deadline", "Deadline");
    add_qos_property<TopicQos, LatencyBudget>(
            cls,
            "latency_budget",
            "LatencyBudget");
    add_qos_property<TopicQos, Liveliness>(cls, "liveliness", "Liveliness");
    add_qos_property<TopicQos, Reliability>(cls, "reliability", "Reliability");
    add_qos_property<TopicQos, DestinationOrder>(
            cls,
            "destination_order",
            "DestinationOrder");
    add_qos_property<TopicQos, History>(cls, "history", "History");
    add_qos_property<TopicQos, ResourceLimits>(
            cls,
            "resource_limits",
            "ResourceLimits");
    add_qos_property<TopicQos, TransportPriority>(
            cls,
            "transport_priority",
            "TransportPriority");
    add_qos_property<TopicQos, Lifespan>(cls, "lifespan", "Lifespan");
    add_qos_property<TopicQos, Ownership>(cls, "ownership", "Ownership");
#if rti_connext_version_gte(6, 0, 0)
    add_qos_property<TopicQos, DataRepresentation>(
            cls,
            "data_representation",
            "DataRepresentation");
#endif
}

template<>
void process_inits<TopicQos>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable { return init_class<TopicQos>(m, "TopicQos"); });
}

}  // namespace pyrti
