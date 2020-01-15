#include "PyConnext.hpp"
#include <dds/core/xtypes/CollectionTypes.hpp>

using namespace dds::core::xtypes;

template<>
void pyrti::init_class_defs(py::class_<CollectionType, DynamicType>& cls) {
    cls
        .def(
            "content_type", 
            &CollectionType::content_type, 
            "Gets the type of elements of this collection."
        );
}

template<>
void pyrti::process_inits<CollectionType>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<CollectionType, DynamicType>(m, "CollectionType");
        }
    );  
}
