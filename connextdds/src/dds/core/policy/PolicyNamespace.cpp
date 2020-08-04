#include "PyConnext.hpp"
#include <dds/dds.hpp>

using namespace dds::core::policy;

void init_namespace_dds_core_policy(py::module &m, pyrti::ClassInitList &l) {
#if rti_connext_version_gte(6, 0, 0)
  pyrti::process_inits<DataRepresentation>(m, l);
  pyrti::process_inits<DataTag>(m, l);
#endif
  pyrti::process_inits<Deadline>(m, l);
  pyrti::process_inits<DestinationOrder>(m, l);
  pyrti::process_inits<Durability>(m, l);
  pyrti::process_inits<DurabilityService>(m, l);
  pyrti::process_inits<EntityFactory>(m, l);
  pyrti::process_inits<GroupData>(m, l);
  pyrti::process_inits<History>(m, l);
  pyrti::process_inits<LatencyBudget>(m, l);
  pyrti::process_inits<Lifespan>(m, l);
  pyrti::process_inits<Liveliness>(m, l);
  pyrti::process_inits<Ownership>(m, l);
  pyrti::process_inits<OwnershipStrength>(m, l);
  pyrti::process_inits<Partition>(m, l);
  pyrti::process_inits<Presentation>(m, l);
  pyrti::process_inits<QosPolicyCount>(m, l);
  pyrti::process_inits<ReaderDataLifecycle>(m, l);
  pyrti::process_inits<Reliability>(m, l);
  pyrti::process_inits<ResourceLimits>(m, l);
  pyrti::process_inits<TimeBasedFilter>(m, l);
  pyrti::process_inits<TopicData>(m, l);
  pyrti::process_inits<TransportPriority>(m, l);
  pyrti::process_inits<TypeConsistencyEnforcement>(m, l);
  pyrti::process_inits<UserData>(m, l);
  pyrti::process_inits<WriterDataLifecycle>(m, l);
}