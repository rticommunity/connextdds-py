#include "PyConnext.hpp"

namespace py = pybind11;

void init_constants(py::module&);
void init_dds_stl_bindings(py::module&);

void init_misc(py::module& m, pyrti::ClassInitList& l) {
    init_constants(m);
    init_dds_stl_bindings(m);
    m.def(
        "sleep",
        &rti::util::sleep,
        py::arg("duration"),
        py::call_guard<py::gil_scoped_release>(),
        "Sleep for a specified duration"
    );
}