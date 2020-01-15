#include "PyConnext.hpp"
#include <dds/domain/qos/DomainParticipantQos.hpp>
#include "PyQos.hpp"

using namespace dds::domain::qos;
using namespace dds::core::policy;
using namespace rti::core::policy;

template<>
void pyrti::init_class_defs(py::class_<DomainParticipantQos>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a DomainParticipantQos with the default value for each "
            "policy."
        );
    pyrti::add_qos_property<DomainParticipantQos, UserData>(cls, "user_data", "UserData");
    pyrti::add_qos_property<DomainParticipantQos, EntityFactory>(cls, "entity_factory", "EntityFactory");
    pyrti::add_qos_property<DomainParticipantQos, WireProtocol>(cls, "wire_protocol", "WireProtocol");
    pyrti::add_qos_property<DomainParticipantQos, TransportBuiltin>(cls, "transport_builtin", "TransportBuiltin");
    pyrti::add_qos_property<DomainParticipantQos, Discovery>(cls, "discovery", "Discovery");
    pyrti::add_qos_property<DomainParticipantQos, DomainParticipantResourceLimits>(cls, "domain_participant_resource_limits", "DomainParticipantResourceLimits");
    pyrti::add_qos_property<DomainParticipantQos, Event>(cls, "event", "Event");
    pyrti::add_qos_property<DomainParticipantQos, ReceiverPool>(cls, "receiver_pool", "ReceiverPool");
    pyrti::add_qos_property<DomainParticipantQos, Database>(cls, "database", "Database");
    pyrti::add_qos_property<DomainParticipantQos, DiscoveryConfig>(cls, "discovery_config", "DiscoveryConfig");
    // Doc bug? Exclusive area listed as a QoS property, but is not part of this QoS.
    //pyrti::add_qos_property<DomainParticipantQos, ExclusiveArea>(cls, "exclusive_area", "ExclusiveArea");
    pyrti::add_qos_property<DomainParticipantQos, Property>(cls, "property", "Property");
    pyrti::add_qos_property<DomainParticipantQos, EntityName>(cls, "entity_name", "EntityName");
    pyrti::add_qos_property<DomainParticipantQos, TransportMulticastMapping>(cls, "transport_multicast_mapping", "TransportMulticastMapping");
    pyrti::add_qos_property<DomainParticipantQos, Service>(cls, "service", "Service");
}

template<>
void pyrti::process_inits<DomainParticipantQos>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<DomainParticipantQos>(m, "DomainParticipantQos");
        }
    ); 
}

