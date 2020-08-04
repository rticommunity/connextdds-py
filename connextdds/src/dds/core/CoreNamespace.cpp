#include "PyConnext.hpp"
#include "PyNamespaces.hpp"
#include <dds/dds.hpp>

using namespace dds::core;

void init_namespace_dds_core(py::module &m, pyrti::ClassInitList &l) {
  pyrti::process_inits<InstanceHandle>(m, l);
  pyrti::process_inits<BytesTopicType>(m, l);
  pyrti::process_inits<KeyedBytesTopicType>(m, l);
  pyrti::process_inits<StringTopicType>(m, l);
  pyrti::process_inits<KeyedStringTopicType>(m, l);
  pyrti::process_inits<Duration>(m, l);
  pyrti::process_inits<Entity>(m, l);
  pyrti::process_inits<Exception>(m, l);
  pyrti::process_inits<QosProvider>(m, l);
  pyrti::process_inits<Time>(m, l);

  init_namespace_dds_core_cond(m, l);
  init_namespace_dds_core_policy(m, l);
  init_namespace_dds_core_status(m, l);
  init_namespace_dds_core_xtypes(m, l);
}