#include "PyConnext.hpp"
#include <rti/rti.hpp>

using namespace rti::topic;

void init_namespace_rti_topic(py::module& m, pyrti::ClassInitList& l) {
    pyrti::process_inits<ContentFilterBase>(m, l);
    pyrti::process_inits<ExpressionProperty>(m, l);
    pyrti::process_inits<FilterSampleInfo>(m, l);
    pyrti::process_inits<PrintFormatProperty>(m, l);
    pyrti::process_inits<ServiceRequest>(m, l);
}