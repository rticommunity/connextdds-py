#include "PyConnext.hpp"
#include <dds/core/xtypes/AliasType.hpp>

using namespace dds::core::xtypes;

template<>
void pyrti::init_class_defs(py::class_<AliasType, DynamicType>& cls) {
    cls
        .def(
            py::init<const std::string&, const DynamicType&, bool>(),
            py::arg("name"),
            py::arg("type_name"),
            py::arg("is_pointer") = false,
            "Creates an alias with a name and a related type."
        )
        .def(
            "related_type",
            &AliasType::related_type,
            "Gets the related type."
        )
        .def_property_readonly(
            "is_pointer", &AliasType::is_pointer,
            "Gets whether this alias makes related_type a pointer"
        )
        .doc() = "Represents a typedef.";
}

template<>
void pyrti::process_inits<AliasType>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<AliasType, DynamicType>(m, "AliasType");
        }
    ); 
}
