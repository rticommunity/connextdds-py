#include "PyConnext.hpp"
#include <dds/core/xtypes/CollectionTypes.hpp>

using namespace dds::core::xtypes;
using namespace rti::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<SequenceType, UnidimensionalCollectionTypeImpl>& cls) {
    cls
        .def(
            py::init<const DynamicType&>(),
            "Creates an unbounded collection with an element type.",
            py::arg("data_type"))
        .def(
            py::init<const DynamicType&, uint32_t>(),
            py::arg("data_type"),
            py::arg("bounds"),
            "Creates a bounded collection with an element type."
        );
}

template<>
void process_inits<SequenceType>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<SequenceType, UnidimensionalCollectionTypeImpl>(m, "SequenceType");
        }
    );
}

}
