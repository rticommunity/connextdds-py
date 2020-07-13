#include <dds/core/LengthUnlimited.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_constants(py::module& m)
{
    m.attr("LENGTH_UNLIMITED") = dds::core::LENGTH_UNLIMITED;
}
