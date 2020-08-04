#include "PyConnext.hpp"
#include "PyNamespaces.hpp"
#include <dds/dds.hpp>
#include <dds/pub/SuspendedPublication.hpp>

using namespace dds::pub;

void init_namespace_dds_pub(py::module &m, pyrti::ClassInitList &l) {
  pyrti::process_inits<AnyDataWriter>(m, l);
  pyrti::process_inits<AnyDataWriterListener>(m, l);
  pyrti::process_inits<CoherentSet>(m, l);
  pyrti::process_inits<Publisher>(m, l);
  pyrti::process_inits<PublisherListener>(m, l);
  pyrti::process_inits<SuspendedPublication>(m, l);

  init_namespace_dds_pub_qos(m, l);
}
