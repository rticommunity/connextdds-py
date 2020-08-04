#include "PyConnext.hpp"
#include <rti/core/ProtocolVersion.hpp>

using namespace rti::core;

namespace pyrti {

template <> void init_class_defs(py::class_<ProtocolVersion> &cls) {
  cls.def(py::init<>(), "Creates an invalid protocol version.")
      .def(py::init<uint8_t, uint8_t>(), py::arg("major"), py::arg("minor"),
           "Creates a version with the given major and minor values.")
      .def_property_readonly("major_version", &ProtocolVersion::major_version,
                             "The major version number.")
      .def_property_readonly("minor_version", &ProtocolVersion::minor_version,
                             "The minor version number.")
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.")
      .def_static("current", &ProtocolVersion::current,
                  "The most recent protocol version.");
}

template <>
void process_inits<ProtocolVersion>(py::module &m, ClassInitList &l) {
  l.push_back([m]() mutable {
    return init_class<ProtocolVersion>(m, "ProtocolVersion");
  });
}

} // namespace pyrti
