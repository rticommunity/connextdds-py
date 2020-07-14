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
