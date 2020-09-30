#include "PyConnext.hpp"
#include "PyNamespaces.hpp"
#include <dds/dds.hpp>

using namespace dds::topic;

void init_namespace_dds_topic(py::module& m, pyrti::ClassInitList& l)
{
    pyrti::process_inits<AnyTopic>(m, l);
    pyrti::process_inits<AnyTopicListener>(m, l);
    pyrti::process_inits<BuiltinTopicKey>(m, l);
    pyrti::process_inits<Filter>(m, l);
    pyrti::process_inits<ParticipantBuiltinTopicData>(m, l);
    pyrti::process_inits<PublicationBuiltinTopicData>(m, l);
    pyrti::process_inits<SubscriptionBuiltinTopicData>(m, l);
    pyrti::process_inits<TopicBuiltinTopicData>(m, l);

    init_namespace_dds_topic_qos(m, l);
}
