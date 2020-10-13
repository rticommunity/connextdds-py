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
