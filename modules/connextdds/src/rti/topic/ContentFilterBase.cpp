#include "PyConnext.hpp"
#include <rti/topic/ContentFilter.hpp>

using namespace rti::topic;

namespace pyrti {


template<>
void init_class_defs(py::class_<ContentFilterBase>& cls)
{
}

template<>
void process_inits<ContentFilterBase>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<ContentFilterBase>(m, "ContentFilterBase");
    });
}

}  // namespace pyrti
