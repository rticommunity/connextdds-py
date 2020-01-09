#include "PyConnext.hpp"
#include <dds/pub/CoherentSet.hpp>

using namespace dds::pub;

template<>
void pyrti::init_class_defs(py::class_<CoherentSet>& cls) {
    cls
        .def(
            py::init<
                const Publisher&
            >(),
            py::arg("publisher"),
            "Creating a CoherentSet object indicates that the application will "
            "begin a coherent set of modifications using DataWriter objects "
            "attached to the Publisher."
        )
        .def(
            "end",
            &CoherentSet::end,
            "Explicitly terminate a coherent set initiated by the CoherentSet "
            "constructor."
        )
        .def(
            "__enter__",
            [](CoherentSet& c) {
                return c;
            },
            "Context manage the CoherentSet."
        )
        .def(
            "__exit__",
            [](CoherentSet& c) {
                c.end();
            }
        );
}

template<>
void pyrti::process_inits<CoherentSet>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<CoherentSet>(m, "CoherentSet");
        }
    );   
}
