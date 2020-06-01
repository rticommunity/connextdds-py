#include "PyConnext.hpp"
#include "PySeq.hpp"
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
        .def_property(
            "ordinal",
            (int32_t (EnumMember::*)() const) &EnumMember::ordinal,
            (EnumMember& (EnumMember::*)(int32_t)) &EnumMember::ordinal,
            "The member's ordinal."
        );
}

template<>
void process_inits<EnumMember>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class_with_seq<EnumMember>(m, "EnumMember");
        }
    );  
}

}
