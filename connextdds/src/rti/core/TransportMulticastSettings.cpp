#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<TransportMulticastSettings>& cls) {
    cls
        .def(
            py::init<
                const std::vector<std::string>&,
                const std::string&, 
                int32_t
            >(),
            py::arg("transports"),
            py::arg("receive_address"),
            py::arg("receive_port"),
            "Creates an instance with the specified transport aliases, "
            "receive address and receive port."
        )
        .def_property(
            "transports",
            (std::vector<std::string> (TransportMulticastSettings::*)() const) &TransportMulticastSettings::transports,
            (TransportMulticastSettings& (TransportMulticastSettings::*)(const std::vector<std::string>&)) &TransportMulticastSettings::transports,
            "A sequence of transport aliases that specifies the transports on "
            "which to receive multicast traffic for the entity."
            "\n\n"
            "This property's getter returns a deep copy."
        )
        .def_property(
            "receive_address",
            (std::string (TransportMulticastSettings::*)() const) &TransportMulticastSettings::receive_address,
            (TransportMulticastSettings& (TransportMulticastSettings::*)(const std::string&)) &TransportMulticastSettings::receive_address,
            "The multicast group address on which the entity can receive data."
        )
        .def_property(
            "receive_port",
            (int32_t (TransportMulticastSettings::*)() const) &TransportMulticastSettings::receive_port,
            (TransportMulticastSettings& (TransportMulticastSettings::*)(int32_t)) &TransportMulticastSettings::receive_port,
            "The multicast port on which the entity can receive data."
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
void process_inits<TransportMulticastSettings>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<TransportMulticastSettings>(m, "TransportMulticastSettings");
        }
    );
}

}
