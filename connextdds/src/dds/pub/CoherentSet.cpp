#include "PyConnext.hpp"
#include <dds/pub/CoherentSet.hpp>

using namespace dds::pub;

namespace pyrti {

template<>
void init_class_defs(py::class_<CoherentSet>& cls) {
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
void process_inits<CoherentSet>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<CoherentSet>(m, "CoherentSet");
        }
    );   
}

}
