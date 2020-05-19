#include "PyConnext.hpp"
#include <rti/topic/ContentFilter.hpp>

using namespace rti::topic;

namespace pyrti {

/*template<class CFBBase=ContentFilterBase>
class PyContentFilterBaseTrampoline: public ContentFilterBase {
    using CFBBase::CFBBase;

private:
    virtual void to_native(
        const rti::topic::FilterHolder& filter_holder,
        struct DDS_ContentFilter *c_filter) {}
};*/

template<>
void init_class_defs(py::class_<ContentFilterBase>& cls) {
    /*cls
        .def(
            py::init<>()
        ); */
}

template<>
void process_inits<ContentFilterBase>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<ContentFilterBase>(m, "ContentFilterBase");
        }
    );
}

}
