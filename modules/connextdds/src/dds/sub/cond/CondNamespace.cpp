#include "PyConnext.hpp"
#include <dds/dds.hpp>

using namespace dds::sub::cond;

void init_namespace_dds_sub_cond(py::module& m, pyrti::ClassInitList& l)
{
    pyrti::process_inits<QueryCondition>(m, l);
    pyrti::process_inits<ReadCondition>(m, l);
}
