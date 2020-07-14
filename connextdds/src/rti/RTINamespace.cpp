#include "PyConnext.hpp"
#include "PyNamespaces.hpp"
#include <rti/rti.hpp>

void init_namespace_rti(py::module& m, pyrti::ClassInitList& l)
{
    init_namespace_rti_config(m, l);
    init_namespace_rti_core(m, l);
    init_namespace_rti_domain(m, l);
    init_namespace_rti_pub(m, l);
    init_namespace_rti_sub(m, l);
    init_namespace_rti_topic(m, l);
}
