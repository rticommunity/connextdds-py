#include "PyConnext.hpp"
#include <dds/pub/SuspendedPublication.hpp>

using namespace dds::pub;

template<>
void pyrti::init_class_defs(py::class_<SuspendedPublication>& cls) {
    cls
        .def(
            py::init<
                const Publisher&
            >(),
            py::arg("publisher"),
            "Suspends the publications of the publisher."
        )
        .def(
            "resume",
            &SuspendedPublication::resume,
            "Indicates that the application has completed these changes."
        )
        .def(
            "__enter__",
            [](SuspendedPublication& s) {
                return s;
            },
            "Context manage the SuspendedPublication."
        )
        .def(
            "__exit__",
            [](SuspendedPublication& s) {
                s.resume();
            }
        );
}

template<>
void pyrti::process_inits<SuspendedPublication>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<SuspendedPublication>(m, "SuspendedPublication");
        }
    ); 
}
