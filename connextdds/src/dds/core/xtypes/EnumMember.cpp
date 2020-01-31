#include "PyConnext.hpp"
#include <dds/core/xtypes/MemberType.hpp>

using namespace dds::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<EnumMember>& cls) {
    cls
        .def(
            py::init<const std::string&, int32_t>(),
            py::arg("name"),
            py::arg("ordinal"),
            "Create an EnumMember with a given name and ordinal value."
        )
        .def_property(
            "name",
            [](const EnumMember& e) {
                return e.name().to_std_string();
            },
            [](EnumMember& e, std::string& n) {
                e.name(n);
            },
            "The member name."
        )
        /*.def(
            "set_name",
            [](EnumMember& e, std::string& n) {
                return e.name(n);
            },
            "Set the union member name and return the member for call chaining.") */
        .def_property(
            "ordinal",
            (int32_t (EnumMember::*)() const) &EnumMember::ordinal,
            (EnumMember& (EnumMember::*)(int32_t)) &EnumMember::ordinal,
            "The member's ordinal."
        );
        /*.def(
            "set_ordinal",
            (EnumMember& (EnumMember::*)(int32_t)) &EnumMember::ordinal,
            "Set the member's ordinal."); */
}

template<>
void process_inits<EnumMember>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<EnumMember>(m, "EnumMember");
        }
    );  
}

}
