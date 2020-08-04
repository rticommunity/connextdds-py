#include "PyConnext.hpp"
#include <rti/core/status/Status.hpp>

using namespace rti::core::status;

namespace pyrti {

template <> void init_class_defs(py::class_<EventCount32> &cls) {
  cls.def_property_readonly("total", &EventCount32::total, "The total count.")
      .def_property_readonly("change", &EventCount32::change,
                             "The incremental count.");
}

template <> void process_inits<EventCount32>(py::module &m, ClassInitList &l) {
  l.push_back(
      [m]() mutable { return init_class<EventCount32>(m, "EventCount32"); });
}

} // namespace pyrti
