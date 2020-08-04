#include "PyConnext.hpp"
#include <rti/core/status/Status.hpp>

using namespace rti::core::status;

namespace pyrti {

template <> void init_class_defs(py::class_<EventCount64> &cls) {
  cls.def_property_readonly("total", &EventCount64::total, "The total count.")
      .def_property_readonly("change", &EventCount64::change,
                             "The incremental count.");
}

template <> void process_inits<EventCount64>(py::module &m, ClassInitList &l) {
  l.push_back(
      [m]() mutable { return init_class<EventCount64>(m, "EventCount64"); });
}

} // namespace pyrti
