#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

namespace pyrti {

template <> void init_class_defs(py::class_<LatencyBudget> &cls) {
  cls.def(py::init<>(), "Creates a latency budget with zero duration.")
      .def(py::init<const dds::core::Duration &>(), py::arg("duration"),
           "Creates an instance with the specified duration.")
      .def_property(
          "duration",
          (const dds::core::Duration (LatencyBudget::*)() const) &
              LatencyBudget::duration,
          (LatencyBudget & (LatencyBudget::*)(const dds::core::Duration &)) &
              LatencyBudget::duration,
          "The duration of the maximum acceptable delay."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.");
}

template <> void process_inits<LatencyBudget>(py::module &m, ClassInitList &l) {
  l.push_back(
      [m]() mutable { return init_class<LatencyBudget>(m, "LatencyBudget"); });
}

} // namespace pyrti
