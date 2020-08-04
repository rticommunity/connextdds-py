#include "PyConnext.hpp"
#include "PySeq.hpp"
#include <rti/core/LocatorFilter.hpp>

using namespace rti::core;

namespace pyrti {

template <> void init_class_defs(py::class_<LocatorFilterElement> &cls) {
  cls.def(py::init<const std::string &, const std::vector<Locator> &>(),
          "Creates an instance with the provided filter_expression and "
          "locators.")
      .def_property("locators",
                    (std::vector<Locator>(LocatorFilterElement::*)() const) &
                        LocatorFilterElement::locators,
                    (LocatorFilterElement &
                     (LocatorFilterElement::*)(const std::vector<Locator> &)) &
                        LocatorFilterElement::locators,
                    "The locators associated with this filter."
                    "\n\n"
                    "This property's getter returns a deep copy.")
      .def_property("filter_expression",
                    (std::string(LocatorFilterElement::*)() const) &
                        LocatorFilterElement::filter_expression,
                    (LocatorFilterElement &
                     (LocatorFilterElement::*)(const std::string &)) &
                        LocatorFilterElement::filter_expression,
                    "The filter expression.")
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.");
}

template <>
void process_inits<LocatorFilterElement>(py::module &m, ClassInitList &l) {
  l.push_back([m]() mutable {
    return init_class_with_seq<LocatorFilterElement>(m, "LocatorFilterElement");
  });
}

} // namespace pyrti
