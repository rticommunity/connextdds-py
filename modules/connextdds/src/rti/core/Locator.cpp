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
#include "PySeq.hpp"
#include <rti/core/Locator.hpp>
#include "PySafeEnum.hpp"
#include "PyVector.hpp"


using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<Locator>& cls)
{
    cls.def(py::init<LocatorKind::type, uint32_t, std::vector<uint8_t>>(),
            py::arg("kind"),
            py::arg("port"),
            py::arg("address"),
            "Construct a Locator with the provided kind, port, and address.")
            .def_property(
                    "kind",
                    (LocatorKind::type(Locator::*)() const) & Locator::kind,
                    (Locator & (Locator::*) (const LocatorKind::type))
                            & Locator::kind,
                    "Get/set the kind for this Locator.")
            .def_property(
                    "port",
                    (uint32_t(Locator::*)() const) & Locator::port,
                    (Locator & (Locator::*) (uint32_t)) & Locator::port,
                    "Get/set the port for this Locator.")
            .def_property(
                    "address",
                    (std::vector<uint8_t>(Locator::*)() const)
                            & Locator::address,
                    (Locator & (Locator::*) (const std::vector<uint8_t>&) )
                            & Locator::address,
                    "Get/set the address for this Locator.")
            .def(py::self == py::self, "Compare Locators for equality.")
            .def(py::self != py::self, "Compare Locators for inequality.")
            .def_property_readonly_static(
                    "invalid",
                    [](py::object&) {
                        return Locator::Invalid();
                    },
                    "Create an invalid Locator.")
            .doc() =
            "Type used to represent the addressing information needed to send "
            "a message to an RTPS Endpoint using one of the supported "
            "transports.";
}

template<>
void process_inits<Locator>(py::module& m, ClassInitList& l)
{
    init_dds_safe_enum<LocatorKind_def>(m, "LocatorKind", [](py::object& o) {
        py::enum_<LocatorKind::type>(o, "LocatorKind")
                .value("INVALID",
                       LocatorKind::type::INVALID,
                       "An invalid locator")
                .value("ANY",
                       LocatorKind::type::ANY,
                       "A special value for any kind of locator.")
                .value("UDPv4",
                       LocatorKind::type::UDPv4,
                       "A locator for a UDPv4 address.")
                .value("SHMEM",
                       LocatorKind::type::SHMEM,
                       "A locator for an address accessed via shared memory.")
                .value("SHMEM_510",
                       LocatorKind::type::SHMEM_510,
                       "A locator for an address accessed via shared memory "
                       "with "
                       "backwards compatibility for Connext 5.1.0 or earlier.")
                .value("INTRA",
                       LocatorKind::type::INTRA,
                       "A locator for the Connext INTRA transport.")
                .value("UDPv6",
                       LocatorKind::type::UDPv6,
                       "A locator for a UDPv6 address.")
                .value("UDPv6_510",
                       LocatorKind::type::UDPv6_510,
                       "A locator for a UDPv6 address with backwards "
                       "compatibility for "
                       "Connext 5.1.0 or earlier.")
                .value("DTLS",
                       LocatorKind::type::DTLS,
                       "A locator for an address that communicates over DTLS.")
                .value("WAN",
                       LocatorKind::type::WAN,
                       "A locator for an address that communicates using the "
                       "Secure WAN "
                       "plugin.")
                .value("TCPV4_LAN",
                       LocatorKind::type::TCPV4_LAN,
                       "A locator for an address that communicates using TCP "
                       "on a LAN.")
                .value("TCPV4_WAN",
                       LocatorKind::type::TCPV4_WAN,
                       "A locator for an address that communicates using TCP "
                       "over a WAN.")
                .value("TLSV4_LAN",
                       LocatorKind::type::TLSV4_LAN,
                       "A locator for an address that communicates using TLS "
                       "over a LAN.")
                .value("TLSV4_WAN",
                       LocatorKind::type::TLSV4_WAN,
                       "A locator for an address that communicates using TLS "
                       "over a WAN.")
                .value("RESERVED",
                       LocatorKind::type::RESERVED,
                       "Reserved locator kind.")
                .export_values();
    });

    l.push_back([m, &l]() mutable {
        auto func = init_class_with_seq<Locator>(m, "Locator");
        init_dds_vector_nonbuffer_class<Locator>(m, "LocatorVector", l);
        return func;
    });
}

}  // namespace pyrti
