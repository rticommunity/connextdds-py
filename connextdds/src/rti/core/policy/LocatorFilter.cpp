#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<LocatorFilter>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<
                         const std::vector<rti::core::LocatorFilterElement>&,
                         const std::string&>(),
                 py::arg("locator_filters"),
                 py::arg_v(
                         "filter_name",
                         rti::topic::stringmatch_filter_name(),
                         "FilterType.stringmatch_filter_name"),
                 "Creates an instance with a sequence of filters and a filter "
                 "name.")
            .def_property(
                    "locator_filters",
                    (std::vector<rti::core::LocatorFilterElement>(
                            LocatorFilter::*)() const)
                            & LocatorFilter::locator_filters,
                    (LocatorFilter
                     & (LocatorFilter::*) (const std::vector<
                                           rti::core::LocatorFilterElement>&) )
                            & LocatorFilter::locator_filters,
                    "The locator filters."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property(
                    "filter_name",
                    (std::string(LocatorFilter::*)() const)
                            & LocatorFilter::filter_name,
                    (LocatorFilter & (LocatorFilter::*) (const std::string&) )
                            & LocatorFilter::filter_name,
                    "The filter name.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<LocatorFilter>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<LocatorFilter>(m, "LocatorFilter");
    });
}

}  // namespace pyrti
