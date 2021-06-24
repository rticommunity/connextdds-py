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

using namespace dds::core;

void init_namespace_dds_core(py::module& m, pyrti::ClassInitList& l, pyrti::DefInitVector& v)
{
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

    init_namespace_dds_core_cond(m, l, v);
    init_namespace_dds_core_policy(m, l, v);
    init_namespace_dds_core_status(m, l, v);
    init_namespace_dds_core_xtypes(m, l, v);
}