#include "PyConnext.hpp"
#include <rti/rti.hpp>

using namespace rti::core::cond;

void init_namespace_rti_core_cond(py::module& m, pyrti::ClassInitList& l)
{
    pyrti::process_inits<WaitSetProperty>(m, l);
}