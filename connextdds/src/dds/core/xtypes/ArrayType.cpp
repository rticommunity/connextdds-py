#include "PyConnext.hpp"
#include <dds/core/xtypes/CollectionTypes.hpp>

using namespace dds::core::xtypes;

template<>
void pyrti::init_class_defs(py::class_<ArrayType, CollectionType>& cls) {
    cls
        .def(
            py::init<const DynamicType&, uint32_t>(),
            py::arg("data_type"),
            py::arg("dimension"),
            "Creates a unidimensional array."
        )
        .def(
            py::init([](const DynamicType& dt, const std::vector<int32_t>& v) {
                return ArrayType(dt, v.begin(), v.end());
            }),
            py::arg("data_type"),
            py::arg("dimensions"),
            "Create a multidimensional array."
        )
        .def_property_readonly(
            "dimension_count",
            &ArrayType::dimension_count,
            "Number of dimensions."
        )
        .def(
            "dimension",
            [](const ArrayType& at, uint32_t index) {
                if (index >= at.dimension_count()) throw py::index_error();
                return at.dimension(index);
            },
            py::arg("index"),
            "Returns the size of the ith dimension"
        )
        .def_property_readonly(
            "total_element_count",
            &ArrayType::total_element_count,
            "Total element count across all dimensions."
        );
}

template<>
void pyrti::process_inits<ArrayType>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<ArrayType, CollectionType>(m, "ArrayType");
        }
    ); 
}