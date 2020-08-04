#include "PyConnext.hpp"
#include <rti/topic/ExpressionProperty.hpp>

using namespace rti::topic;

namespace pyrti {

template <> void init_class_defs(py::class_<ExpressionProperty> &cls) {
  cls.def(py::init<>(),
          "Create a default ExpressionProperty with key_only_filter = false "
          "and writer_side_filter_optimization = false.")
      .def(py::init<bool, bool>(), py::arg("key_only_filter"),
           py::arg("writer_side_filter_optimization"),
           "Create an ExpressionProperty with the provided key_only_filter, "
           "and writer_side_filter_optimization.")
      .def_property("key_only_filter",
                    (bool (ExpressionProperty::*)() const) &
                        ExpressionProperty::key_only_filter,
                    (ExpressionProperty & (ExpressionProperty::*)(bool)) &
                        ExpressionProperty::key_only_filter,
                    "The value for key_only_filter, indicating if the filter "
                    "expression is based only on key fields. In this case, RTI "
                    "Connext itself can cache the filtering results.")
      .def_property(
          "writer_side_filter_optimization",
          (bool (ExpressionProperty::*)() const) &
              ExpressionProperty::writer_side_filter_optimization,
          (ExpressionProperty & (ExpressionProperty::*)(bool)) &
              ExpressionProperty::writer_side_filter_optimization,
          "The value for writer_side_filter_optimization, indicating if the "
          "filter implementation can cache the filtering result for the "
          "provided expression.")
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.");
}

template <>
void process_inits<ExpressionProperty>(py::module &m, ClassInitList &l) {
  l.push_back([m]() mutable {
    return init_class<ExpressionProperty>(m, "ExpressionProperty");
  });
}

} // namespace pyrti
