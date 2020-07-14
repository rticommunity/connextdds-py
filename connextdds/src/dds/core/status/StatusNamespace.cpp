#include "PyConnext.hpp"
#include <dds/dds.hpp>

using namespace dds::core::status;

void init_namespace_dds_core_status(py::module& m, pyrti::ClassInitList& l)
{
    pyrti::process_inits<InconsistentTopicStatus>(m, l);
    pyrti::process_inits<LivelinessChangedStatus>(m, l);
    pyrti::process_inits<LivelinessLostStatus>(m, l);
    pyrti::process_inits<OfferedDeadlineMissedStatus>(m, l);
    pyrti::process_inits<OfferedIncompatibleQosStatus>(m, l);
    pyrti::process_inits<PublicationMatchedStatus>(m, l);
    pyrti::process_inits<RequestedDeadlineMissedStatus>(m, l);
    pyrti::process_inits<RequestedIncompatibleQosStatus>(m, l);
    pyrti::process_inits<SampleLostStatus>(m, l);
    pyrti::process_inits<SampleRejectedState>(m, l);
    pyrti::process_inits<SampleRejectedStatus>(m, l);
    pyrti::process_inits<StatusMask>(m, l);
    pyrti::process_inits<SubscriptionMatchedStatus>(m, l);
}
