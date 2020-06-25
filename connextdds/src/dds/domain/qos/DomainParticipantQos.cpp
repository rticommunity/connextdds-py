#include "PyConnext.hpp"
#include <dds/domain/qos/DomainParticipantQos.hpp>
#include "PyQos.hpp"

using namespace dds::domain::qos;
using namespace dds::core::policy;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<DomainParticipantQos>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a DomainParticipantQos with the default value for each "
            "policy."
        )
        .def(
            py::init(
                [](const PyDomainParticipant& dp) {
                    return dp.qos();
                }
            ),
            py::arg("participant"),
            "Create a DomainParticipantQos with settings equivalent to those "
            "of the provided DomainParticipant object."
        )
        .def(
            py::self == py::self,
            "Test for equality"
        )
        .def (
            py::self != py::self,
            "Test for inequality."
        );

    add_qos_property<DomainParticipantQos, UserData>(cls, "user_data", "UserData");
    add_qos_property<DomainParticipantQos, EntityFactory>(cls, "entity_factory", "EntityFactory");
    add_qos_property<DomainParticipantQos, WireProtocol>(cls, "wire_protocol", "WireProtocol");
    add_qos_property<DomainParticipantQos, TransportBuiltin>(cls, "transport_builtin", "TransportBuiltin");
    add_qos_property<DomainParticipantQos, Discovery>(cls, "discovery", "Discovery");
    add_qos_property<DomainParticipantQos, DomainParticipantResourceLimits>(cls, "resource_limits", "DomainParticipantResourceLimits");
    add_qos_property<DomainParticipantQos, Event>(cls, "event", "Event");
    add_qos_property<DomainParticipantQos, ReceiverPool>(cls, "receiver_pool", "ReceiverPool");
    add_qos_property<DomainParticipantQos, Database>(cls, "database", "Database");
    add_qos_property<DomainParticipantQos, DiscoveryConfig>(cls, "discovery_config", "DiscoveryConfig");
    // Doc bug? Exclusive area listed as a QoS property, but is not part of this QoS.
    //add_qos_property<DomainParticipantQos, ExclusiveArea>(cls, "exclusive_area", "ExclusiveArea");
    add_qos_property<DomainParticipantQos, Property>(cls, "property", "Property");
    add_qos_property<DomainParticipantQos, EntityName>(cls, "participant_name", "EntityName");
    add_qos_property<DomainParticipantQos, TransportMulticastMapping>(cls, "transport_multicast_mapping", "TransportMulticastMapping");
    add_qos_property<DomainParticipantQos, Service>(cls, "service", "Service");
}

template<>
void process_inits<DomainParticipantQos>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<DomainParticipantQos>(m, "DomainParticipantQos");
        }
    ); 
}

}

