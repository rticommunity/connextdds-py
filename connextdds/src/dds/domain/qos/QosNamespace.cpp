#include "PyConnext.hpp"
#include <dds/dds.hpp>

using namespace dds::domain::qos;

void init_namespace_dds_domain_qos(py::module &m, pyrti::ClassInitList &l) {
  pyrti::process_inits<DomainParticipantFactoryQos>(m, l);
  pyrti::process_inits<DomainParticipantQos>(m, l);
}
