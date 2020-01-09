#include "PyConnext.hpp"
#include <rti/core/LocatorFilter.hpp>

using namespace rti::core;

template<>
void pyrti::init_class_defs(py::class_<LocatorFilterElement>& cls) {
    cls
        .def(
            py::init<
                const std::string&,
                const std::vector<Locator>&
            >(),
            "Creates an instance with the provided filter_expression and "
            "locators."
        )
        .def_property(
            "locators",
            (std::vector<Locator> (LocatorFilterElement::*)() const) &LocatorFilterElement::locators,
            (LocatorFilterElement& (LocatorFilterElement::*)(const std::vector<Locator>&)) &LocatorFilterElement::locators,
            "The locators associated with this filter."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "filter_expression",
            (std::string (LocatorFilterElement::*)() const) &LocatorFilterElement::filter_expression,
            (LocatorFilterElement& (LocatorFilterElement::*)(const std::string&)) &LocatorFilterElement::filter_expression,
            "The filter expression."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        ); 
}

template<>
void pyrti::process_inits<LocatorFilterElement>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<LocatorFilterElement>(m, "LocatorFilterElement");
        }
    );
}
