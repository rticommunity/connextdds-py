#include "PyConnext.hpp"
#include <rti/rti.hpp>

using namespace rti::pub;

void init_namespace_rti_pub(py::module &m, pyrti::ClassInitList &l) {
  pyrti::process_inits<AcknowledgmentInfo>(m, l);
  pyrti::process_inits<FlowController>(m, l);
  pyrti::process_inits<WriteParams>(m, l);
}