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

using namespace dds::topic;

void init_namespace_dds_topic(py::module& m, pyrti::ClassInitList& l, pyrti::DefInitVector& v)
{
    pyrti::process_inits<AnyTopic>(m, l);
    pyrti::process_inits<AnyTopicListener>(m, l);
    pyrti::process_inits<BuiltinTopicKey>(m, l);
    pyrti::process_inits<Filter>(m, l);
    pyrti::process_inits<ParticipantBuiltinTopicData>(m, l);
    pyrti::process_inits<PublicationBuiltinTopicData>(m, l);
    pyrti::process_inits<SubscriptionBuiltinTopicData>(m, l);
    pyrti::process_inits<TopicBuiltinTopicData>(m, l);

    init_namespace_dds_topic_qos(m, l, v);
}
