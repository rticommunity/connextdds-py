#include "PyConnext.hpp"
#include "PySeq.hpp"
#include <dds/core/xtypes/MemberType.hpp>

using namespace dds::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<UnionMember>& cls) {
    cls
        .def(
            py::init<const std::string&, const DynamicType&, UnionMember::DiscriminatorType>(),
            py::arg("name"),
            py::arg("dynamic_type"),
            py::arg("label"),
            "Creates a union member with a name, type, and selected by a single label."
        )
        .def(
            py::init<const std::string&, const DynamicType&, const std::vector<UnionMember::DiscriminatorType>&>(),
            py::arg("name"),
            py::arg("dynamic_type"),
            py::arg("labels"),
            "Create a union member with a name, type, and selected by one or more labels."
        )
        .def_property(
            "name",
            [](const UnionMember& u) {
                return u.name().to_std_string();
            },
            [](UnionMember& u, const std::string n) {
                u.name(n);
            },
            "The union member's name."
        )
        .def_property_readonly(
            "type",
            &UnionMember::type,
            "Gets the union member's type."
        )
        .def_property_readonly(
            "has_id",
            &UnionMember::has_id,
            "Indicates if union member has an ID annotation."
        )
        .def_property(
            "id",
            &UnionMember::get_id,
            &UnionMember::id,
            "Returns the ID annotation of this member, or INVALID_ID if this member has no ID."
        )
        .def_property(
            "pointer",
            &UnionMember::is_pointer,
            &UnionMember::pointer,
            "Boolean flag for pointer status of union member."
        )
        .def_property_readonly(
            "label_count",
            &UnionMember::label_count,
            "Gets the number of labels that select this member."
        )
        .def_property(
            "labels",
            (UnionMember::LabelSeq (UnionMember::*)() const) &UnionMember::labels,
            (UnionMember& (UnionMember::*)(const UnionMember::LabelSeq&)) &UnionMember::labels,
            "The labels that select this member."
        )
        .def_readonly_static(
            "DEFAULT_LABEL",
            &UnionMember::DEFAULT_LABEL,
            "Special value for the default union label."
        )
        .def_readonly_static(
            "INVALID_ID",
            &UnionMember::INVALID_ID,
            "The special ID of a member without the ID annotation."
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
void process_inits<UnionMember>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class_with_seq<UnionMember>(m, "UnionMember");
        }
    );  
}

}

