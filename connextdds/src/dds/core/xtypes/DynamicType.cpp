#include "PyConnext.hpp"
#include <dds/core/xtypes/DynamicType.hpp>

using namespace dds::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<DynamicType>& cls) {
    cls
        .def_property_readonly(
            "kind",
            &DynamicType::kind,
            "Get the type kind."
        )
        .def_property_readonly(
            "name",
            &DynamicType::name,
            "Gets the name."
        )
        .def(
            "print_idl",
            &rti::core::xtypes::print_idl,
            "Prints the IDL representation of this type to the standard output."
        )
        .def(
            "is_primitive_type",
            [](const DynamicType& dt) {
                return dds::core::xtypes::is_primitive_type(dt);
            },
            "Determines if this DynamicType is a PrimitiveType"
        )
        .def(
            "is_constructed_type",
            [](const DynamicType& dt) {
                return dds::core::xtypes::is_constructed_type(dt);
            },
            "Determines if this DynamicType is a constructed type."
        )
        .def(
            "is_collection_type",
            [](const DynamicType& dt) {
                return dds::core::xtypes::is_collection_type(dt);
            },
            "Determins if this DynamicType is a CollectionType."
        )
        .def(
            "is_aggregation_type",
            [](const DynamicType& dt) {
                return dds::core::xtypes::is_aggregation_type(dt);
            },
            "Determines if this DynamicType is an aggregation type."
        )
        .def(
            "__str__",
            &rti::core::xtypes::print_idl
        )
        /*.def_static(
            "is_primitive_type",
            &is_primitive_type,
            py::arg("dynamic_type"),
            "Determines if the DynamicType is a PrimitiveType"
        )
        .def_static(
            "is_constructed_type",
            &is_primitive_type,
            py::arg("dynamic_type"),
            "Determines if the DynamicType is a constructed type."
        )
        .def_static(
            "is_collection_type",
            &is_collection_type,
            py::arg("dynamic_type"),
            "Determins if the DynamicType is a CollectionType."
        )
        .def_static(
            "is_aggregation_type",
            &is_aggregation_type,
            py::arg("dynamic_type"),
            "Determines if the DynamicType is an aggregation type."
        )*/
        .def(
            py::self == py::self,
            "Compare DataStateEx objects for equality."
        )
        .def(
            py::self != py::self,
            "Compare DataStateEx objects for inequality."
        );
}

template<>
void process_inits<DynamicType>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<DynamicType>(m, "DynamicType");
        }
    );  
}

}

