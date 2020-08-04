#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

namespace pyrti {

template <> void init_class_defs(py::class_<EntityFactory> &cls) {
  cls.def(py::init<>(), "Creates the default policy.")
      .def(py::init<bool>(), py::arg("auto_enable"),
           "Specifies whether the entity acting as a factory automatically "
           "enables the instances it creates.")
      .def_property(
          "autoenable_created_entities",
          (bool (EntityFactory::*)() const) &
              EntityFactory::autoenable_created_entities,
          (EntityFactory & (EntityFactory::*)(bool)) &
              EntityFactory::autoenable_created_entities,
          "Whether the entity acting as a factory automatically enables the "
          "instances it creates.")
      .def_static("auto_enable", &EntityFactory::AutoEnable,
                  "Creates EntityFactory(true)")
      .def_static("manually_enable", &EntityFactory::ManuallyEnable,
                  "Creates EntityFactory(false)")
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.");
}

template <> void process_inits<EntityFactory>(py::module &m, ClassInitList &l) {
  l.push_back(
      [m]() mutable { return init_class<EntityFactory>(m, "EntityFactory"); });
}

} // namespace pyrti
