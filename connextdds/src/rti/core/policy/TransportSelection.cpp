#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<TransportSelection>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates the default policy."
        )
        .def(
            py::init<
                const std::vector<std::string>&
            >(),
            py::arg("enabled_transports"),
            "Creates an instance with the sepcified transport aliases."
        )
        .def_property(
            "enabled_transports",
            (std::vector<std::string> (TransportSelection::*)()const) &TransportSelection::enabled_transports,
            (TransportSelection& (TransportSelection::*)(const std::vector<std::string>&)) &TransportSelection::enabled_transports,
            "A sequence of transport aliases that specifies the transport "
            "instances available for use by the entity."
            "\n\n"
            "This property uses value semantics."
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
void process_inits<TransportSelection>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<TransportSelection>(m, "TransportSelection");
        }
    );
}

}
