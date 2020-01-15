#include "PyConnext.hpp"
#include <dds/core/xtypes/CollectionTypes.hpp>

using namespace dds::core::xtypes;
using namespace rti::core::xtypes;

template<>
void pyrti::init_class_defs(py::class_<StringType, UnidimensionalCollectionTypeImpl>& cls) {
    cls
        .def(
            py::init<uint32_t>(),
            py::arg("bounds"), 
            "Creates a bounded string."
        );
}

template<>
void pyrti::process_inits<StringType>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<StringType, UnidimensionalCollectionTypeImpl>(m, "StringType");
        }
    ); 
}