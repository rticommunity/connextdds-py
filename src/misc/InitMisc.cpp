#include "PyConnext.hpp"

namespace py = pybind11;

void init_constants(py::module&);
void init_dds_stl_bindings(py::module&);

void init_misc(py::module& m, pyrti::ClassInitList& l) {
    init_constants(m);
    init_dds_stl_bindings(m);
}