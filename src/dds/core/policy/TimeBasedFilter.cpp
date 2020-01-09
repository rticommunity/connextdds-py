#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

template<>
void pyrti::init_class_defs(py::class_<TimeBasedFilter>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates the default time-based filter."
        )
        .def(
            py::init<
                const dds::core::Duration&
            >(),
            py::arg("min_separation"),
            "Creates a policy with the specified minimum separation."
        )
        .def_property(
            "minimum_separation",
            (const dds::core::Duration (TimeBasedFilter::*)() const) &TimeBasedFilter::minimum_separation,
            (TimeBasedFilter& (TimeBasedFilter::*)(const dds::core::Duration&)) &TimeBasedFilter::minimum_separation,
            "The minimum separation between subsequent samples."
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
void pyrti::process_inits<TimeBasedFilter>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<TimeBasedFilter>(m, "TimeBasedFilter");
        }
    ); 
}