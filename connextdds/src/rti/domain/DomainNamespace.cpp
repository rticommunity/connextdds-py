#include "PyConnext.hpp"
#include <rti/rti.hpp>

using namespace rti::domain;

void init_namespace_rti_domain(py::module &m, pyrti::ClassInitList &l) {
  pyrti::process_inits<DomainParticipantConfigParams>(m, l);
}