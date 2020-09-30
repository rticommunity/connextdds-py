#include "PyConnext.hpp"
#include <dds/dds.hpp>

using namespace dds::sub::qos;

void init_namespace_dds_sub_qos(py::module& m, pyrti::ClassInitList& l)
{
    pyrti::process_inits<DataReaderQos>(m, l);
    pyrti::process_inits<SubscriberQos>(m, l);
}
