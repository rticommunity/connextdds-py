#include "PyConnext.hpp"
#include <dds/dds.hpp>

using namespace dds::topic::qos;

void init_namespace_dds_topic_qos(py::module& m, pyrti::ClassInitList& l)
{
    pyrti::process_inits<TopicQos>(m, l);
}