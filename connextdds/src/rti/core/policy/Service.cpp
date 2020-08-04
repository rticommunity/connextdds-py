#include "PyConnext.hpp"
#include "PySafeEnum.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template <> void init_class_defs(py::class_<Service> &cls) {
  cls.def(py::init<>(), "Creates the default policy (no service).")
      .def(py::init<ServiceKind>(), py::arg("kind"),
           "Creates an instance with the specified service kind.")
      .def_property("kind", (ServiceKind(Service::*)() const) & Service::kind,
                    (Service & (Service::*)(ServiceKind)) & Service::kind,
                    "The service kind.")
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.");
}

template <> void process_inits<Service>(py::module &m, ClassInitList &l) {
  init_dds_safe_enum<ServiceKind_def>(m, "ServiceKind", [](py::object &o) {
    py::enum_<ServiceKind::type>(o, "Enum")
        .value("NO_SERVICE", ServiceKind::type::NO_SERVICE,
               "There is no service associated to the Entity.")
        .value("PERSISTENCE", ServiceKind::type::PERSISTENCE,
               "The Entity is associated to RTI Persistence Service.")
        .value("QUEUING", ServiceKind::type::QUEUING,
               "The Entity is associated to RTI Queuing Service.")
        .value("ROUTING", ServiceKind::type::ROUTING,
               "The Entity is associated to RTI Routing Service.")
        .value("RECORDING", ServiceKind::type::RECORDING,
               "The Entity is associated to RTI Recording Service.")
        .value("REPLAY", ServiceKind::type::REPLAY,
               "The Entity is associated to RTI Replay Service.")
        .value("DATABASE_INTEGRATION", ServiceKind::type::DATABASE_INTEGRATION,
               "The Entity is associated to RTI Database Integration Service.")
        .value("WEB_INTEGRATION", ServiceKind::type::WEB_INTEGRATION,
               "The Entity is associated to RTI Web Integration Service.")
        .export_values();
  });

  l.push_back([m]() mutable { return init_class<Service>(m, "Service"); });
}

} // namespace pyrti
