#include "PyConnext.hpp"
#include <rti/rti.hpp>

using namespace rti::core::status;

void init_namespace_rti_core_status(py::module& m, pyrti::ClassInitList& l) {
    pyrti::process_inits<DataReaderCacheStatus>(m, l);
    pyrti::process_inits<DataReaderProtocolStatus>(m, l);
    pyrti::process_inits<DataWriterCacheStatus>(m, l);
    pyrti::process_inits<DataWriterProtocolStatus>(m, l);
    pyrti::process_inits<DomainParticipantProtocolStatus>(m, l);
    pyrti::process_inits<EventCount32>(m, l);
    pyrti::process_inits<EventCount64>(m, l);
    pyrti::process_inits<ReliableReaderActivityChangedStatus>(m, l);
    pyrti::process_inits<ReliableWriterCacheChangedStatus>(m, l);
    pyrti::process_inits<SampleLostState>(m, l);
    pyrti::process_inits<ServiceRequestAcceptedStatus>(m, l);
}