#include "PyConnext.hpp"
#include "PyNamespaces.hpp"
#include <rti/rti.hpp>

using namespace rti::core;

void init_namespace_rti_core(py::module& m, pyrti::ClassInitList& l)
{
    pyrti::process_inits<pyrti::PyBuiltinProfiles>(m, l);
    pyrti::process_inits<AllocationSettings>(m, l);
    pyrti::process_inits<ChannelSettings>(m, l);
    pyrti::process_inits<ContentFilterProperty>(m, l);
    pyrti::process_inits<Cookie>(m, l);
    pyrti::process_inits<EndpointGroup>(m, l);
    pyrti::process_inits<Guid>(m, l);
    pyrti::process_inits<Locator>(m, l);
    pyrti::process_inits<LocatorFilterElement>(m, l);
    pyrti::process_inits<LongDouble>(m, l);
    pyrti::process_inits<ProductVersion>(m, l);
    pyrti::process_inits<ProtocolVersion>(m, l);
    pyrti::process_inits<QosProviderParams>(m, l);
    pyrti::process_inits<RtpsReliableReaderProtocol>(m, l);
    pyrti::process_inits<SampleFlag>(m, l);
    pyrti::process_inits<SampleIdentity>(m, l);
    pyrti::process_inits<SequenceNumber>(m, l);
    pyrti::process_inits<ThreadSettings>(m, l);
    pyrti::process_inits<TransportInfo>(m, l);
    pyrti::process_inits<TransportMulticastSettings>(m, l);
    pyrti::process_inits<VendorId>(m, l);

    init_namespace_rti_core_cond(m, l);
    init_namespace_rti_core_policy(m, l);
    init_namespace_rti_core_status(m, l);
    init_namespace_rti_core_xtypes(m, l);
}
