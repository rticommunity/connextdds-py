#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>
#include "PySafeEnum.hpp"
#include "PyMaskType.hpp"

using namespace rti::core;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<RtpsReservedPortKindMask>& cls) {
    cls
        .def_static(
            "all",
            &RtpsReservedPortKindMask::all,
            "All bits are set."
        )
        .def_static(
            "none",
            &RtpsReservedPortKindMask::none,
            "No bits are set."
        )
        .def_static(
            "default_mask",
            &RtpsReservedPortKindMask::default_mask,
            "The default value of WireProtocol.rtps_reserved_port_mask."
        )
        .def_static(
            "builtin_unicast",
            &RtpsReservedPortKindMask::builtin_unicast,
            "Select the metatraffic unicast port."
        )
        .def_static(
            "builtin_multicast",
            &RtpsReservedPortKindMask::builtin_multicast,
            "Select the metatraffic multicast port."
        )
        .def_static(
            "user_unicast",
            &RtpsReservedPortKindMask::user_unicast,
            "Select the usertraffic unicast port."
        )
        .def_static(
            "user_multicast",
            &RtpsReservedPortKindMask::user_multicast,
            "Select the usertraffic multicast port."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

template<>
void init_class_defs(py::class_<RtpsWellKnownPorts>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates an instance that contains the default RTPS well-known "
            "ports."
        )
        .def(
            py::init<
                int32_t,
                int32_t,
                int32_t,
                int32_t,
                int32_t,
                int32_t,
                int32_t
            >(),
            py::arg("port_base"),
            py::arg("domain_id_gain"),
            py::arg("participant_id_gain"),
            py::arg("builtin_multicast_port_offset"),
            py::arg("builtin_unicast_port_offset"),
            py::arg("user_multicast_port_offset"),
            py::arg("user_unicast_port_offset"),
            "Creates an instance with the specified ports."
        )
        .def_property(
            "port_base",
            (int32_t (RtpsWellKnownPorts::*)() const) &RtpsWellKnownPorts::port_base,
            (RtpsWellKnownPorts& (RtpsWellKnownPorts::*)(int32_t)) &RtpsWellKnownPorts::port_base,
            "The port base offset."
        )
        .def_property(
            "domain_id_gain",
            (int32_t (RtpsWellKnownPorts::*)() const) &RtpsWellKnownPorts::domain_id_gain,
            (RtpsWellKnownPorts& (RtpsWellKnownPorts::*)(int32_t)) &RtpsWellKnownPorts::domain_id_gain,
            "Tunable domain gain parameter."
        )
        .def_property(
            "participant_id_gain",
            (int32_t (RtpsWellKnownPorts::*)() const) &RtpsWellKnownPorts::participant_id_gain,
            (RtpsWellKnownPorts& (RtpsWellKnownPorts::*)(int32_t)) &RtpsWellKnownPorts::participant_id_gain,
            "Tunable participant gain parameter."
        )
        .def_property(
            "builtin_multicast_port_offset",
            (int32_t (RtpsWellKnownPorts::*)() const) &RtpsWellKnownPorts::builtin_multicast_port_offset,
            (RtpsWellKnownPorts& (RtpsWellKnownPorts::*)(int32_t)) &RtpsWellKnownPorts::builtin_multicast_port_offset,
            "Additional offset for metatraffic multicast port."
        )
        .def_property(
            "builtin_unicast_port_offset",
            (int32_t (RtpsWellKnownPorts::*)() const) &RtpsWellKnownPorts::builtin_unicast_port_offset,
            (RtpsWellKnownPorts& (RtpsWellKnownPorts::*)(int32_t)) &RtpsWellKnownPorts::builtin_unicast_port_offset,
            "Additional offset for metatraffic unicast port."
        )
        .def_property(
            "user_multicast_port_offset",
            (int32_t (RtpsWellKnownPorts::*)() const) &RtpsWellKnownPorts::user_multicast_port_offset,
            (RtpsWellKnownPorts& (RtpsWellKnownPorts::*)(int32_t)) &RtpsWellKnownPorts::user_multicast_port_offset,
            "Additional offset for usertraffic multicast port."
        )
        .def_property(
            "user_unicast_port_offset",
            (int32_t (RtpsWellKnownPorts::*)() const) &RtpsWellKnownPorts::user_unicast_port_offset,
            (RtpsWellKnownPorts& (RtpsWellKnownPorts::*)(int32_t)) &RtpsWellKnownPorts::user_unicast_port_offset,
            "Additional offset for usertraffic unicast port."
        )
        .def_static(
            "interoperable",
            &RtpsWellKnownPorts::Interoperable,
            "Returns an instance containing the port mapping compliant with "
            "the OMG DDS Interoperability wire protocol."
        )
        .def_static(
            "backwards_compatible",
            &RtpsWellKnownPorts::BackwardsCompatible,
            "Returns an instance containing the port mapping compatible with "
            "previous versions of RTI Connext."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

template<>
void init_class_defs(py::class_<WireProtocol>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates the default policy."
        )
        .def_property(
            "participant_id",
            (int32_t (WireProtocol::*)() const) &WireProtocol::participant_id,
            (WireProtocol& (WireProtocol::*)(int32_t)) &WireProtocol::participant_id,
            "A value used to distinguish among different participants "
            "belonging to the same domain on the same host."
        )
        .def_property(
            "rtps_host_id",
            (uint32_t (WireProtocol::*)() const) &WireProtocol::rtps_host_id,
            (WireProtocol& (WireProtocol::*)(uint32_t)) &WireProtocol::rtps_host_id,
            "The RTPS Host ID of the domain participant."
        )
        .def_property(
            "rtps_app_id",
            (uint32_t (WireProtocol::*)() const) &WireProtocol::rtps_app_id,
            (WireProtocol& (WireProtocol::*)(uint32_t)) &WireProtocol::rtps_app_id,
            "The RTPS App ID of the domain participant."
        )
        .def_property(
            "rtps_instance_id",
            (uint32_t (WireProtocol::*)() const) &WireProtocol::rtps_instance_id,
            (WireProtocol& (WireProtocol::*)(uint32_t)) &WireProtocol::rtps_instance_id,
            "The RTPS Instance ID of the domain participant."
        )
        .def_property(
            "rtps_well_known_ports",
            (RtpsWellKnownPorts (WireProtocol::*)() const) &WireProtocol::rtps_well_known_ports,
            (WireProtocol& (WireProtocol::*)(const RtpsWellKnownPorts&)) &WireProtocol::rtps_well_known_ports,
            "The RTPS well-known port mappings."
            "\n\n"
            "This property's getter returns a deep copy."
        )
        .def_property(
            "rtps_reserved_port_mask",
            (RtpsReservedPortKindMask (WireProtocol::*)() const) &WireProtocol::rtps_reserved_port_mask,
            (WireProtocol& (WireProtocol::*)(const RtpsReservedPortKindMask&)) &WireProtocol::rtps_reserved_port_mask,
            "Specifies which well-known ports to reserve when enabling the "
            "participant."
            "\n\n"
            "This property's getter returns a deep copy."
        )
        .def_property(
            "rtps_auto_id_kind",
            (WireProtocolAutoKind (WireProtocol::*)() const) &WireProtocol::rtps_auto_id_kind,
            (WireProtocol& (WireProtocol::*)(WireProtocolAutoKind)) &WireProtocol::rtps_auto_id_kind,
            "Kind of auto mechanism used to calculate the GUID prefix."
        )
        .def_property(
            "compute_crc",
            (bool (WireProtocol::*)() const) &WireProtocol::compute_crc,
            (WireProtocol& (WireProtocol::*)(bool)) &WireProtocol::compute_crc,
            "Adds RTPS CRC submessage to every message when this field is set "
            "to true."
        )
        .def_property(
            "check_crc",
            (bool (WireProtocol::*)() const) &WireProtocol::check_crc,
            (WireProtocol& (WireProtocol::*)(bool)) &WireProtocol::check_crc,
            "Checks if the received RTPS message is valid by comparing the "
            "computed CRC with the received RTPS CRC submessage when this "
            "field is set to true."
        )
        .def_readonly_static(
            "RTPS_AUTO_ID",
            &WireProtocol::RTPS_AUTO_ID,
            "Indicates that RTI Connext should choose an appropriate host, "
            "app, instance or object ID automatically."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

template<>
void process_inits<WireProtocol>(py::module& m, ClassInitList& l) {
    init_dds_safe_enum<WireProtocolAutoKind_def>(m, "WireProtocolAutoKind",
        [](py::object& o) {
            py::enum_<WireProtocolAutoKind::type>(o, "Enum")
                .value(
                    "RTPS_AUTO_ID_FROM_IP",
                    WireProtocolAutoKind::type::RTPS_AUTO_ID_FROM_IP,
                    "Select the IPv4 based algorithm."
                )
                .value(
                    "RTPS_AUTO_ID_FROM_MAC",
                    WireProtocolAutoKind::type::RTPS_AUTO_ID_FROM_MAC,
                    "Select the MAC based algorithm."
                )
                .value(
                    "RTPS_AUTO_ID_FROM_UUID",
                    WireProtocolAutoKind::type::RTPS_AUTO_ID_FROM_UUID,
                    "Select the UUID based algorithm."
                )
                .export_values();
        }
    );

    l.push_back(
        [m]() mutable {
            auto cls = init_mask_type<RtpsReservedPortKindMask, uint64_t>(m, "RtpsReservedPortKindMask", "Create a RtpsReservedPortKindMask equivalent to RtpsReservedPortKindMask.none()");
            return [cls]() mutable {
                init_class_defs<RtpsReservedPortKindMask>(cls);
            };
        }
    );

    l.push_back(
        [m]() mutable {
            return init_class<RtpsWellKnownPorts>(m, "RtpsWellKnownPorts");
        }
    );

    l.push_back(
        [m]() mutable {
            return init_class<WireProtocol>(m, "WireProtocol");
        }
    );
}

}
