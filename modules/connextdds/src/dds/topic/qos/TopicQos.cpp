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
#include <dds/topic/qos/TopicQos.hpp>
#include "PyQos.hpp"

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
#if rti_connext_version_gte(6, 1, 0)
    add_qos_string_conversions(cls);
#endif
}

template<>
void process_inits<TopicQos>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable { return init_class<TopicQos>(m, "TopicQos"); });
}

}  // namespace pyrti
