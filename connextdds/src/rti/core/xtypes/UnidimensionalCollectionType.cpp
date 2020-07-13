#include "PyConnext.hpp"
#include <dds/core/xtypes/CollectionTypes.hpp>

using namespace rti::core::xtypes;
using namespace dds::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(
        py::class_<UnidimensionalCollectionTypeImpl, CollectionType>& cls)
{
    cls.def_property_readonly(
               "bounds",
               &UnidimensionalCollectionTypeImpl::bounds,
               "Gets the maximum length of this collection.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<UnidimensionalCollectionTypeImpl>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<UnidimensionalCollectionTypeImpl, CollectionType>(
                m,
                "UnidimensionalCollectionType");
    });
}

}  // namespace pyrti
