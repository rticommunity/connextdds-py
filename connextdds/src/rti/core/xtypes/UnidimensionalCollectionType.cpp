#include "PyConnext.hpp"
#include <dds/core/xtypes/CollectionTypes.hpp>

using namespace rti::core::xtypes;
using namespace dds::core::xtypes;

template<>
void pyrti::init_class_defs(py::class_<UnidimensionalCollectionTypeImpl, CollectionType>& cls) {
    cls
        .def_property_readonly(
            "bounds",
            &UnidimensionalCollectionTypeImpl::bounds,
            "Gets the maximum length of this collection."
        );
}

template<>
void pyrti::process_inits<UnidimensionalCollectionTypeImpl>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<UnidimensionalCollectionTypeImpl, CollectionType>(m, "UnidimensionalCollectionType");
        }
    ); 
}