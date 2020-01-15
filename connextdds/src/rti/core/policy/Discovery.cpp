#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

template<>
void pyrti::init_class_defs(py::class_<Discovery>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates the default policy."
        )
        .def_property(
            "enabled_transports",
            (std::vector<std::string> (Discovery::*)() const) &Discovery::enabled_transports,
            (Discovery& (Discovery::*)(const std::vector<std::string>&)) &Discovery::enabled_transports,
            "The transports (by their aliases) available for the discovery "
            "mechanism."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "initial_peers",
            (std::vector<std::string> (Discovery::*)() const) &Discovery::initial_peers,
            (Discovery& (Discovery::*)(const std::vector<std::string>&)) &Discovery::initial_peers,
            "The initial list of peers that the discovery mechanism will "
            "contact to announce this DomainParticipant."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "multicast_receive_addresses",
            (std::vector<std::string> (Discovery::*)() const) &Discovery::multicast_receive_addresses,
            (Discovery& (Discovery::*)(const std::vector<std::string>&)) &Discovery::multicast_receive_addresses,
            "The multicast group addresses on which discovery-related "
            "meta-traffic can be received by the DomainParticipant."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "metatraffic_transport_priority",
            (int32_t (Discovery::*)() const) &Discovery::metatraffic_transport_priority,
            (Discovery& (Discovery::*)(int32_t)) &Discovery::metatraffic_transport_priority,
            "The transport priority to use for the Discovery meta-traffic."
        )
        .def_property(
            "accept_unknown_peers",
            (bool (Discovery::*)() const) &Discovery::accept_unknown_peers,
            (Discovery& (Discovery::*)(bool)) &Discovery::accept_unknown_peers,
            "Whether to accept a new participant that is not in the initial "
            "peers list."
        )
        .def_property(
            "enable_endpoint_discovery",
            (bool (Discovery::*)() const) &Discovery::enable_endpoint_discovery,
            (Discovery& (Discovery::*)(bool)) &Discovery::enable_endpoint_discovery,
            "Whether to automatically enable endpoint discovery for all the "
            "remote participants."
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
void pyrti::process_inits<Discovery>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<Discovery>(m, "Discovery");
        }
    );
}
