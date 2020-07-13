#include "PyConnext.hpp"
#include "PyVector.hpp"

namespace py = pybind11;

void init_constants(py::module&);
void init_dds_stl_bindings(py::module&);

// Init all bindings that don't depend on DDS for signatures
void init_misc_early(py::module& m, pyrti::ClassInitList& l)
{
    init_constants(m);
    init_dds_stl_bindings(m);
    pyrti::init_dds_vector_buffer_class<uint8_t>(m, "ByteVector", l);
    pyrti::init_dds_vector_buffer_class<int32_t>(m, "Int32Vector", l);
}

// Init all bindings that depend on DDS for signatures
void init_misc_late(py::module& m)
{
    m.def("sleep",
          &rti::util::sleep,
          py::arg("duration"),
          py::call_guard<py::gil_scoped_release>(),
          "Sleep for a specified duration");
}