#include "PyConnext.hpp"
#include <dds/core/xtypes/CollectionTypes.hpp>

using namespace dds::core::xtypes;
using namespace rti::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(
        py::class_<StringType, UnidimensionalCollectionTypeImpl>& cls)
{
    cls.def(py::init<uint32_t>(),
            py::arg("bounds"),
            "Creates a bounded string.")
            .def(py::init([]() {
                    return StringType(UnidimensionalCollectionTypeImpl::UNBOUNDED);
                }))
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<StringType>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<StringType, UnidimensionalCollectionTypeImpl>(
                m,
                "StringType");
    });
}

}  // namespace pyrti
