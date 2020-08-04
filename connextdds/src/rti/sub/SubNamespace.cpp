#include "PyConnext.hpp"
#include "PyNamespaces.hpp"
#include <rti/rti.hpp>

using namespace rti::sub;

void init_namespace_rti_sub(py::module &m, pyrti::ClassInitList &l) {
  pyrti::process_inits<AckResponseData>(m, l);
  pyrti::process_inits<TopicQuery>(m, l);

  init_namespace_rti_sub_status(m, l);
}