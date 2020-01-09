#include "PyConnext.hpp"
#include <dds/core/xtypes/CollectionTypes.hpp>

using namespace dds::core::xtypes;
using namespace rti::core::xtypes;

template<>
void pyrti::init_class_defs(py::class_<WStringType, UnidimensionalCollectionTypeImpl>& cls) {
    cls
        .def(
            py::init<uint32_t>(),
            py::arg("bounds"),
            "Creates a bounded wide string."
        );
}

template<>
void pyrti::process_inits<WStringType>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<WStringType, UnidimensionalCollectionTypeImpl>(m, "WStringType");
        }
    ); 
}