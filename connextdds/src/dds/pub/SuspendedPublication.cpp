#include "PyConnext.hpp"
#include <dds/pub/SuspendedPublication.hpp>

using namespace dds::pub;

namespace pyrti {

template<>
void init_class_defs(py::class_<SuspendedPublication>& cls) {
    cls
        .def(
            py::init<
                const PyPublisher&
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
            [](SuspendedPublication& s, py::object, py::object, py::object) {
                s.resume();
            }
        );
}

template<>
void process_inits<SuspendedPublication>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<SuspendedPublication>(m, "SuspendedPublication");
        }
    ); 
}

}
