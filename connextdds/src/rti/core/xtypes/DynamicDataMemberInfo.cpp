#include "PyConnext.hpp"
#include <rti/core/xtypes/DynamicDataMemberInfo.hpp>

using namespace rti::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<DynamicDataMemberInfo>& cls) {
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
            [](const DynamicDataMemberInfo& mi) {
                return mi.member_name().to_std_string();
            },
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
void process_inits<DynamicDataMemberInfo>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<DynamicDataMemberInfo>(m, "DynamicDataMemberInfo");
        }
    );
}

}
