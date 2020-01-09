#include "PyConnext.hpp"
#include <dds/core/xtypes/MemberType.hpp>

using namespace dds::core::xtypes;
using namespace rti::core::xtypes;

template<>
void pyrti::init_class_defs(py::class_<AbstractConstructedType<UnionMember>, DynamicType>& cls) {
    cls
        .def(
            "extensibility_kind", 
            &AbstractConstructedType<UnionMember>::extensibility_kind, 
            "Gets the extensibility kind."
        )
        .def_property_readonly(
            "member_count", 
            &AbstractConstructedType<UnionMember>::member_count, 
            "Gets the number of members."
        )
        .def(
            "member",
            (const UnionMember& (AbstractConstructedType<UnionMember>::*)(AbstractConstructedType<UnionMember>::MemberIndex) const) &AbstractConstructedType<UnionMember>::member,
            "Gets a member by its index."
        )
        .def(
            "member",
            (const UnionMember& (AbstractConstructedType<UnionMember>::*)(const std::string&) const) &AbstractConstructedType<UnionMember>::member,
            "Gets a member by its name."
        )
        .def(
            "find_member_by_name", 
            &AbstractConstructedType<UnionMember>::find_member_by_name, 
            "Obtains the member index from its name."
        )
        .def(
            "members", 
            &AbstractConstructedType<UnionMember>::members, 
            "Gets a copy of all the members"
        )
        .def(
            "cdr_serialized_sample_max_size", 
            &AbstractConstructedType<UnionMember>::cdr_serialized_sample_max_size, 
            "Gets the maximum serialized size of samples of this type."
        )
        .def_readonly_static(
            "INVALID_INDEX", 
            &AbstractConstructedType<UnionMember>::INVALID_INDEX, 
            "Indicates a member doesn't exist"
        );
}

template<>
void pyrti::process_inits<AbstractConstructedType<UnionMember>>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<AbstractConstructedType<UnionMember>, DynamicType>(m, "ACTUnionMember");
        }
    ); 
}