#include "PyConnext.hpp"
#include <rti/core/xtypes/DynamicDataMemberInfo.hpp>

using namespace rti::core::xtypes;

template<>
void pyrti::init_class_defs(py::class_<DynamicDataMemberInfo>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a DynamicDataMemberInfo object."
        )
        .def_property_readonly(
            "index",
            &DynamicDataMemberInfo::member_index,
            "The member index"
        )
        .def_property_readonly(
            "name",
            &DynamicDataMemberInfo::member_name,
            "The member name"
        )
        .def_property_readonly(
            "kind",
            &DynamicDataMemberInfo::member_kind,
            "The member type kind"
        )
        .def_property_readonly(
            "element_count",
            &DynamicDataMemberInfo::element_count,
            "The number of elements in the member."
        )
        .def_property_readonly(
            "element_kind",
            &DynamicDataMemberInfo::element_kind,
            "The type kind of the elements in the member"
        );
}

template<>
void pyrti::process_inits<DynamicDataMemberInfo>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<DynamicDataMemberInfo>(m, "DynamicDataMemberInfo");
        }
    );
}
