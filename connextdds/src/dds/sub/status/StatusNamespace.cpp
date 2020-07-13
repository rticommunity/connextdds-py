#include "PyConnext.hpp"
#include <dds/dds.hpp>

using namespace dds::sub::status;

void init_namespace_dds_sub_status(py::module& m, pyrti::ClassInitList& l)
{
    pyrti::process_inits<DataState>(m, l);
}
