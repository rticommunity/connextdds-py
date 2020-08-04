#include "PyConnext.hpp"
#include <rti/rti.hpp>

using namespace rti::sub::status;

void init_namespace_rti_sub_status(py::module &m, pyrti::ClassInitList &l) {
  pyrti::process_inits<DataStateEx>(m, l);
}