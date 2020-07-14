#include "PyConnext.hpp"
#include <rti/rti.hpp>

using namespace rti::config;

void init_namespace_rti_config(py::module& m, pyrti::ClassInitList& l)
{
    pyrti::process_inits<Logger>(m, l);
}