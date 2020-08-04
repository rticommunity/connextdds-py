#include "PyConnext.hpp"
#include "PyNamespaces.hpp"
#include <dds/dds.hpp>

using namespace dds::domain;

void init_namespace_dds_domain(py::module &m, pyrti::ClassInitList &l) {
  pyrti::process_inits<DomainParticipant>(m, l);
  pyrti::process_inits<DomainParticipantListener>(m, l);

  init_namespace_dds_domain_qos(m, l);
}
