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
#include <rti/rti.hpp>

using namespace rti::core::status;

void init_namespace_rti_core_status(py::module& m, pyrti::ClassInitList& l, pyrti::DefInitVector& v)
{
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