#include "PyConnext.hpp"
#include <rti/core/xtypes/DynamicDataMemberInfo.hpp>

using namespace rti::core::xtypes;

namespace pyrti {


static 
bool member_info_equal(const DynamicDataMemberInfo& mi, const DynamicDataMemberInfo& other) {
    bool retval = mi.member_index() == other.member_index() &&
                  mi.member_name() == other.member_name() &&
                  mi.member_kind() == other.member_kind() &&
                  mi.element_count() == other.element_count() &&
                  mi.representation_count() == other.representation_count();
    
    if (retval && 
     ((mi.member_kind().underlying() == dds::core::xtypes::TypeKind::ARRAY_TYPE) ||
     (mi.member_kind().underlying() == dds::core::xtypes::TypeKind::SEQUENCE_TYPE))) {
        retval = mi.element_kind() == other.element_kind();
    }
    return retval;
}


template<>
void init_class_defs(py::class_<DynamicDataMemberInfo>& cls)
{
    cls.def(py::init<>(), "Create a DynamicDataMemberInfo object.")
            .def_property_readonly(
                    "index",
                    &DynamicDataMemberInfo::member_index,
                    "The member index")
            .def_property_readonly(
                    "name",
                    [](const DynamicDataMemberInfo& mi) {
                        return mi.member_name().to_std_string();
                    },
                    "The member name")
            .def_property_readonly(
                    "kind",
                    &DynamicDataMemberInfo::member_kind,
                    "The member type kind")
            .def_property_readonly(
                    "element_count",
                    &DynamicDataMemberInfo::element_count,
                    "The number of elements in the member.")
            .def_property_readonly(
                    "element_kind",
                    &DynamicDataMemberInfo::element_kind,
                    "The type kind of the elements in the member")
            .def(
                "__eq__",
                [](const DynamicDataMemberInfo& mi, const DynamicDataMemberInfo& other) {
                    return member_info_equal(mi, other);
                },
                py::is_operator(),
                "Test for equality.")
            .def(
                "__ne__",
                [](const DynamicDataMemberInfo& mi, const DynamicDataMemberInfo& other) {
                    return !member_info_equal(mi, other);
                },
                py::is_operator(),
                "Test for inequality.");
}

template<>
void process_inits<DynamicDataMemberInfo>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DynamicDataMemberInfo>(m, "DynamicDataMemberInfo");
    });
}

}  // namespace pyrti
