#include "PyConnext.hpp"
#include "PyNamespaces.hpp"

void init_namespace_dds(py::module& m, pyrti::ClassInitList& l)
{
    init_namespace_dds_core(m, l);
    init_namespace_dds_domain(m, l);
    init_namespace_dds_pub(m, l);
    init_namespace_dds_sub(m, l);
    init_namespace_dds_topic(m, l);
}
