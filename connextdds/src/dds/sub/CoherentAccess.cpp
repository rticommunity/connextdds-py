#include "PyConnext.hpp"
#include <dds/sub/CoherentAccess.hpp>

using namespace dds::sub;

namespace pyrti {

template <> void init_class_defs(py::class_<CoherentAccess> &cls) {
  cls.def(py::init([](const PySubscriber &s) {
            return std::unique_ptr<CoherentAccess>(new CoherentAccess(s));
          }),
          py::arg("subscriber"),
          "Creating a CoherentAccess object indicates that the application "
          "is about to access the data samples in any of the DataReader "
          "objects attached to the provided Subscriber.")
      .def("end", &CoherentAccess::end,
           "Explicitly indicate that the application has finished accessing "
           "the data samples in DataReader objects managed by the Subscriber.")
      .def(
          "__enter__", [](CoherentAccess *ca) { return ca; },
          "Start a context managed coherent access block.")
      .def(
          "__exit__",
          [](CoherentAccess *ca, py::object, py::object, py::object) {
            ca->end();
          },
          "End a context managed coherent access block.");
}

template <>
void process_inits<CoherentAccess>(py::module &m, ClassInitList &l) {
  l.push_back([m]() mutable {
    return init_class<CoherentAccess>(m, "CoherentAccess");
  });
}

} // namespace pyrti
