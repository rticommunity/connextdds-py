#include "PyConnext.hpp"
#include "PySeq.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core;
using namespace rti::core::policy;

namespace pyrti {

template <> void init_class_defs(py::class_<TransportUnicastSettings> &cls) {
  cls.def(py::init<>(), "Creates the default policy.")
      .def(py::init<const std::vector<std::string> &, int32_t>(),
           py::arg("transports"), py::arg("receive_port") = 0,
           "Creates the default policy.")
      .def_property(
          "transports",
          (std::vector<std::string>(TransportUnicastSettings::*)() const) &
              TransportUnicastSettings::transports,
          (TransportUnicastSettings &
           (TransportUnicastSettings::*)(const std::vector<std::string> &)) &
              TransportUnicastSettings::transports,
          "A sequence of transport aliases that specifies the unicast "
          "interfaces on which to receive unicast traffic for the entity."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "receive_port",
          (int32_t(TransportUnicastSettings::*)() const) &
              TransportUnicastSettings::receive_port,
          (TransportUnicastSettings & (TransportUnicastSettings::*)(int32_t)) &
              TransportUnicastSettings::receive_port,
          "The unicast port on which the entity can receive data.")
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.");
}

template <> void init_class_defs(py::class_<TransportUnicast> &cls) {
  cls.def(py::init<>(), "Creates the default policy.")
      .def(py::init<const std::vector<TransportUnicastSettings> &>(),
           py::arg("settings"),
           "Creates an instance with the specified settings.")
      .def_property(
          "settings",
          (std::vector<TransportUnicastSettings>(TransportUnicast::*)() const) &
              TransportUnicast::settings,
          (TransportUnicast &
           (TransportUnicast::*)(const std::vector<TransportUnicastSettings>
                                     &)) &
              TransportUnicast::settings,
          "The unicast settings."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.");
}

template <>
void process_inits<TransportUnicast>(py::module &m, ClassInitList &l) {
  l.push_back([m]() mutable {
    return init_class_with_seq<TransportUnicastSettings>(
        m, "TransportUnicastSettings");
  });

  l.push_back([m]() mutable {
    return init_class<TransportUnicast>(m, "TransportUnicast");
  });
}

} // namespace pyrti
