#include "PyConnext.hpp"
#include <dds/core/xtypes/MemberType.hpp>

using namespace dds::core::xtypes;
using namespace rti::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<AbstractConstructedType<UnionMember>, DynamicType>& cls) {
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
            "__getitem__",
            [](const AbstractConstructedType<UnionMember>& act, const std::string& name) {
                return act.member(name);
            }
        )
        .def(
            "__getitem__",
            [](const AbstractConstructedType<UnionMember>& act, uint32_t index) {
                return act.member(index);
            }
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
#if rti_connext_version_gte(6, 0, 0)
        .def(
            "cdr_serialized_sample_max_size", 
            &AbstractConstructedType<UnionMember>::cdr_serialized_sample_max_size, 
            "Gets the maximum serialized size of samples of this type."
        )
#endif
        .def_readonly_static(
            "INVALID_INDEX", 
            &AbstractConstructedType<UnionMember>::INVALID_INDEX, 
            "Indicates a member doesn't exist"
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

template<>
void process_inits<AbstractConstructedType<UnionMember>>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<AbstractConstructedType<UnionMember>, DynamicType>(m, "ACTUnionMember");
        }
    ); 
}

}
