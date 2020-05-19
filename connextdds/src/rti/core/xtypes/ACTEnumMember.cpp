#include "PyConnext.hpp"
#include <dds/core/xtypes/MemberType.hpp>

using namespace dds::core::xtypes;
using namespace rti::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<AbstractConstructedType<EnumMember>, DynamicType>& cls) {
    cls
        .def(
            "extensibility_kind", 
            &AbstractConstructedType<EnumMember>::extensibility_kind, 
            "Gets the extensibility kind."
        )
        .def_property_readonly(
            "member_count", 
            &AbstractConstructedType<EnumMember>::member_count, 
            "Gets the number of members."
        )
        .def(
            "member",
            (const EnumMember& (AbstractConstructedType<EnumMember>::*)(AbstractConstructedType<EnumMember>::MemberIndex) const) &AbstractConstructedType<EnumMember>::member,
            "Gets a member by its index."
        )
        .def(
            "member",
            (const EnumMember& (AbstractConstructedType<EnumMember>::*)(const std::string&) const) &AbstractConstructedType<EnumMember>::member,
            "Gets a member by its name."
        )
        .def(
            "__getitem__",
            [](const AbstractConstructedType<EnumMember>& act, const std::string& name) {
                return act.member(name);
            }
        )
        .def(
            "__getitem__",
            [](const AbstractConstructedType<EnumMember>& act, uint32_t index) {
                return act.member(index);
            }
        )
        .def(
            "find_member_by_name", 
            &AbstractConstructedType<EnumMember>::find_member_by_name, 
            "Obtains the member index from its name."
        )
        .def(
            "members", 
            &AbstractConstructedType<EnumMember>::members, 
            "Gets a copy of all the members"
        )
#if rti_connext_version_gte(6, 0, 0)
        .def(
            "cdr_serialized_sample_max_size", 
            &AbstractConstructedType<EnumMember>::cdr_serialized_sample_max_size, 
            "Gets the maximum serialized size of samples of this type."
        )
#endif
        .def_readonly_static(
            "INVALID_INDEX", 
            &AbstractConstructedType<EnumMember>::INVALID_INDEX, 
            "Indicates a member doesn't exist"
        );
}

template<>
void process_inits<AbstractConstructedType<EnumMember>>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<AbstractConstructedType<EnumMember>, DynamicType>(m, "AbstractConstructedType<EnumMember>");
        }
    ); 
}

}
