#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template <> void init_class_defs(py::class_<EntityName> &cls) {
  cls.def(py::init<>(), "Creates the default policy (no name).")
      .def(py::init<const std::string &>(), py::arg("name"),
           "Creates an instance that specifies an entity name.")
      .def_property(
          "name",
          (rti::core::optional_value<std::string>(EntityName::*)() const) &
              EntityName::name,
          (EntityName &
           (EntityName::*)(const rti::core::optional_value<std::string> &)) &
              EntityName::name,
          "Sets the entity name.")
      .def_property(
          "role_name",
          (rti::core::optional_value<std::string>(EntityName::*)() const) &
              EntityName::role_name,
          (EntityName &
           (EntityName::*)(const rti::core::optional_value<std::string> &)) &
              EntityName::role_name,
          "Sets the role name.")
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.");
}

template <> void process_inits<EntityName>(py::module &m, ClassInitList &l) {
  l.push_back(
      [m]() mutable { return init_class<EntityName>(m, "EntityName"); });
}

} // namespace pyrti
