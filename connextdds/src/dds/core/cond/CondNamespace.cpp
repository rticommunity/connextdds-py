#include "PyConnext.hpp"
#include <dds/dds.hpp>

using namespace dds::core::cond;

void init_namespace_dds_core_cond(py::module &m, pyrti::ClassInitList &l) {
  pyrti::process_inits<Condition>(m, l);
  pyrti::process_inits<GuardCondition>(m, l);
  pyrti::process_inits<StatusCondition>(m, l);
  pyrti::process_inits<WaitSet>(m, l);
}