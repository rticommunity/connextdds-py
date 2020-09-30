#include "PyConnext.hpp"
#include <dds/core/xtypes/CollectionTypes.hpp>

using namespace dds::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<CollectionType, DynamicType>& cls)
{
    cls.def_property_readonly(
               "content_type",
               [](const CollectionType& ct) {
                   auto dt = ct.content_type();
                   return py_cast_type(dt);
               },
               "Gets the type of elements of this collection.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<CollectionType>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<CollectionType, DynamicType>(m, "CollectionType");
    });
}

}  // namespace pyrti
