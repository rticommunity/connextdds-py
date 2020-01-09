#include "PyConnext.hpp"
#include <dds/topic/qos/TopicQos.hpp>
#include "PyQos.hpp"

using namespace dds::topic::qos;
using namespace dds::core::policy;

template<>
void pyrti::init_class_defs(py::class_<TopicQos>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a TopicQos with the default value for each policy."
        );
    pyrti::add_qos_property<TopicQos, TopicData>(cls, "topic_data", "TopicData");
    pyrti::add_qos_property<TopicQos, Durability>(cls, "durability", "Durability");
    pyrti::add_qos_property<TopicQos, DurabilityService>(cls, "durability_service", "DurabilityService");
    pyrti::add_qos_property<TopicQos, Deadline>(cls, "deadline", "Deadline");
    pyrti::add_qos_property<TopicQos, LatencyBudget>(cls, "latency_budget", "LatencyBudget");
    pyrti::add_qos_property<TopicQos, Liveliness>(cls, "liveliness", "Liveliness");
    pyrti::add_qos_property<TopicQos, Reliability>(cls, "reliability", "Reliability");
    pyrti::add_qos_property<TopicQos, DestinationOrder>(cls, "destination_order", "DestinationOrder");
    pyrti::add_qos_property<TopicQos, History>(cls, "history", "History");
    pyrti::add_qos_property<TopicQos, ResourceLimits>(cls, "resource_limits", "ResourceLimits");
    pyrti::add_qos_property<TopicQos, TransportPriority>(cls, "transport_priority", "TransportPriority");
    pyrti::add_qos_property<TopicQos, Lifespan>(cls, "lifespan", "Lifespan");
    pyrti::add_qos_property<TopicQos, Ownership>(cls, "ownership", "Ownership");
    pyrti::add_qos_property<TopicQos, DataRepresentation>(cls, "data_representation", "DataRepresentation");
}

template<>
void pyrti::process_inits<TopicQos>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<TopicQos>(m, "TopicQos");
        }
    ); 
}
