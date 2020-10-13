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
#include <dds/dds.hpp>
#include "PyDataReader.hpp"

using namespace dds::sub;

void init_namespace_dds_sub(py::module& m, pyrti::ClassInitList& l)
{
    pyrti::process_inits<AnyDataReader>(m, l);
    pyrti::process_inits<AnyDataReaderListener>(m, l);
    pyrti::process_inits<CoherentAccess>(m, l);
    pyrti::process_inits<GenerationCount>(m, l);
    pyrti::process_inits<pyrti::PyIDataReader>(m, l);
    pyrti::process_inits<Query>(m, l);
    pyrti::process_inits<Rank>(m, l);
    pyrti::process_inits<SampleInfo>(m, l);
    pyrti::process_inits<Subscriber>(m, l);
    pyrti::process_inits<SubscriberListener>(m, l);

    init_namespace_dds_sub_cond(m, l);
    init_namespace_dds_sub_qos(m, l);
    init_namespace_dds_sub_status(m, l);
}
