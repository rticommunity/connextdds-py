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
#include <dds/domain/qos/DomainParticipantQos.hpp>
#include "PyQos.hpp"

using namespace dds::domain::qos;
using namespace dds::core::policy;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<DomainParticipantQos>& cls)
{
    cls.def(py::init<>(),
            "Create a DomainParticipantQos with the default value for each "
            "policy.")
            .def(py::init([](const PyDomainParticipant& dp) {
                     return dp.qos();
                 }),
                 py::arg("participant"),
                 "Create a DomainParticipantQos with settings equivalent to "
                 "those "
                 "of the provided DomainParticipant object.")
            .def(py::self == py::self, "Test for equality")
            .def(py::self != py::self, "Test for inequality.");

    add_qos_property<DomainParticipantQos, UserData>(
            cls,
            "user_data",
            "UserData");
    add_qos_property<DomainParticipantQos, EntityFactory>(
            cls,
            "entity_factory",
            "EntityFactory");
    add_qos_property<DomainParticipantQos, WireProtocol>(
            cls,
            "wire_protocol",
            "WireProtocol");
    add_qos_property<DomainParticipantQos, TransportBuiltin>(
            cls,
            "transport_builtin",
            "TransportBuiltin");
    add_qos_property<DomainParticipantQos, Discovery>(
            cls,
            "discovery",
            "Discovery");
    add_qos_property<DomainParticipantQos, DomainParticipantResourceLimits>(
            cls,
            "resource_limits",
            "DomainParticipantResourceLimits");
    add_qos_property<DomainParticipantQos, Event>(cls, "event", "Event");
    add_qos_property<DomainParticipantQos, ReceiverPool>(
            cls,
            "receiver_pool",
            "ReceiverPool");
    add_qos_property<DomainParticipantQos, Database>(
            cls,
            "database",
            "Database");
    add_qos_property<DomainParticipantQos, DiscoveryConfig>(
            cls,
            "discovery_config",
            "DiscoveryConfig");
    // Doc bug? Exclusive area listed as a QoS property, but is not part of this
    // QoS.
    // add_qos_property<DomainParticipantQos, ExclusiveArea>(cls,
    // "exclusive_area", "ExclusiveArea");
    add_qos_property<DomainParticipantQos, Property>(
            cls,
            "property",
            "Property");
    add_qos_property<DomainParticipantQos, EntityName>(
            cls,
            "participant_name",
            "EntityName");
    add_qos_property<DomainParticipantQos, TransportMulticastMapping>(
            cls,
            "transport_multicast_mapping",
            "TransportMulticastMapping");
    add_qos_property<DomainParticipantQos, Service>(cls, "service", "Service");
#if rti_connext_version_gte(6, 1, 0)
    add_qos_string_conversions(cls);
#endif
}

template<>
void process_inits<DomainParticipantQos>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DomainParticipantQos>(m, "DomainParticipantQos");
    });
}

}  // namespace pyrti
