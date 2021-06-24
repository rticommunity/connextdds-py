/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */

#include "PyConnext.hpp"
#include "PyNamespaces.hpp"
#include <rti/rti.hpp>

using namespace rti::core;

void init_namespace_rti_core(py::module& m, pyrti::ClassInitList& l, pyrti::DefInitVector& v)
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
#if rti_connext_version_gte(6, 1, 0)
#ifndef _MSC_VER
    pyrti::process_inits<DataReaderResourceLimitsInstanceReplacementSettings>(m, l);
#endif
    pyrti::process_inits<CompressionSettings>(m, l);
    pyrti::process_inits<CoherentSetInfo>(m, l);
    pyrti::process_inits<QosPrintFormat>(m, l);
#endif

    init_namespace_rti_core_cond(m, l, v);
    init_namespace_rti_core_policy(m, l, v);
    init_namespace_rti_core_status(m, l, v);
    init_namespace_rti_core_xtypes(m, l, v);
}
