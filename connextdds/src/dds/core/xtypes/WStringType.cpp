#include "PyConnext.hpp"
#include <dds/core/xtypes/CollectionTypes.hpp>

using namespace dds::core::xtypes;
using namespace rti::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<WStringType, UnidimensionalCollectionTypeImpl>& cls) {
    cls
        .def(
            py::init<uint32_t>(),
            py::arg("bounds"),
            "Creates a bounded wide string."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

template<>
void process_inits<WStringType>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<WStringType, UnidimensionalCollectionTypeImpl>(m, "WStringType");
        }
    ); 
}

}
