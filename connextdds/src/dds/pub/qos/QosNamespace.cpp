#include "PyConnext.hpp"
#include "PyNamespaces.hpp"
#include <dds/dds.hpp>

using namespace dds::pub::qos;

void init_namespace_dds_pub_qos(py::module& m, pyrti::ClassInitList& l)
{
    pyrti::process_inits<DataWriterQos>(m, l);
    pyrti::process_inits<PublisherQos>(m, l);
}
