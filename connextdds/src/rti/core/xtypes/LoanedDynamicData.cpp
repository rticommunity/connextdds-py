#include "PyConnext.hpp"
#include <dds/core/xtypes/DynamicData.hpp>

using namespace rti::core::xtypes;

namespace pyrti {

template <> void init_class_defs(py::class_<LoanedDynamicData> &cls) {
  cls.def("return_loan", &LoanedDynamicData::return_loan,
          "Explicitly return a dynamic data loan.")
      .def_property_readonly(
          "data",
          (dds::core::xtypes::DynamicData & (LoanedDynamicData::*)()) &
              LoanedDynamicData::get,
          "Obtains the DynamicData object representing a member of a "
          "DynamicData object.")
      .def(
          "__enter__",
          [](LoanedDynamicData &ldd) -> LoanedDynamicData & { return ldd; },
          "Enter a context for the loaned field, loan returned on context "
          "exit.")
      .def(
          "__exit__",
          [](LoanedDynamicData &ldd, py::object, py::object, py::object) {
            ldd.return_loan();
          },
          "Exit the context for the loaned field, returning the resources.");
}

template <>
void process_inits<LoanedDynamicData>(py::module &m, ClassInitList &l) {
  l.push_back([m]() mutable {
    return init_class<LoanedDynamicData>(m, "LoanedDynamicData");
  });
}

} // namespace pyrti
