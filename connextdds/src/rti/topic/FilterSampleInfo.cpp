#include "PyConnext.hpp"
#include <rti/topic/FilterSampleInfo.hpp>

using namespace rti::topic;

namespace pyrti {

template <> void init_class_defs(py::class_<FilterSampleInfo> &cls) {
  cls.def_property_readonly(
         "related_sample_identity", &FilterSampleInfo::related_sample_identity,
         "The Identity of another sample related to this one.")
      .def_property_readonly(
          "priority", &FilterSampleInfo::priority,
          "Get a positive integer designating the relative priority of the "
          "sample, used to determine the transmission order of pending "
          "transmissions.")
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.");
}

template <>
void process_inits<FilterSampleInfo>(py::module &m, ClassInitList &l) {
  l.push_back([m]() mutable {
    return init_class<FilterSampleInfo>(m, "FilterSampleInfo");
  });
}

} // namespace pyrti
