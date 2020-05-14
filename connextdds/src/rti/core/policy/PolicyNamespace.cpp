#include "PyConnext.hpp"
#include <rti/rti.hpp>

using namespace rti::core::policy;

void init_namespace_rti_core_policy(py::module& m, pyrti::ClassInitList& l) {
    pyrti::process_inits<AcknowledgmentKind>(m, l);
    pyrti::process_inits<AsynchronousPublisher>(m, l);
    pyrti::process_inits<Availability>(m, l);
    pyrti::process_inits<Batch>(m, l);
    pyrti::process_inits<BuiltinTopicReaderResourceLimits>(m, l);
    pyrti::process_inits<Database>(m, l);
    pyrti::process_inits<DataReaderProtocol>(m, l);
    pyrti::process_inits<DataReaderResourceLimits>(m, l);
#if rti_connext_version_gte(6, 0, 0)
    pyrti::process_inits<DataWriterTransferMode>(m, l);
#endif
    pyrti::process_inits<DestinationOrderScopeKind>(m, l);
    pyrti::process_inits<Discovery>(m, l);
    pyrti::process_inits<DiscoveryConfig>(m, l);
    pyrti::process_inits<DomainParticipantResourceLimits>(m, l);
    pyrti::process_inits<EntityName>(m, l);
    pyrti::process_inits<Event>(m, l);
    pyrti::process_inits<ExclusiveArea>(m, l);
    pyrti::process_inits<LocatorFilter>(m, l);
    pyrti::process_inits<MultiChannel>(m, l);
    pyrti::process_inits<Property>(m, l);
    pyrti::process_inits<PublishMode>(m, l);
    pyrti::process_inits<ReceiverPool>(m, l);
    pyrti::process_inits<RefilterKind>(m, l);
    pyrti::process_inits<RtpsReliableWriterProtocol>(m, l);
    pyrti::process_inits<Service>(m, l);
    pyrti::process_inits<SystemResourceLimits>(m, l);
    pyrti::process_inits<TopicQueryDispatch>(m, l);
    pyrti::process_inits<TransportBuiltin>(m, l);
    pyrti::process_inits<TransportMulticast>(m, l);
    pyrti::process_inits<TransportSelection>(m, l);
    pyrti::process_inits<TransportUnicast>(m, l);
    pyrti::process_inits<TypeSupport>(m, l);
    pyrti::process_inits<WireProtocol>(m, l);
}