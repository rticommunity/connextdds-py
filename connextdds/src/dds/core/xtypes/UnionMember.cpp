#include "PyConnext.hpp"
#include <dds/core/xtypes/MemberType.hpp>

using namespace dds::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<UnionMember>& cls) {
    cls
        .def(
            py::init<const std::string&, const DynamicType&, const std::vector<UnionMember::DiscriminatorType>&>(),
            py::arg("name"),
            py::arg("dynamic_type"),
            py::arg("labels"),
            "Create a union member with a name, type, and selected by one or more labels."
        )
        .def(
            py::init<const std::string&, const DynamicType&, UnionMember::DiscriminatorType>(),
            py::arg("name"),
            py::arg("dynamic_type"),
            py::arg("label"),
            "Creates a union member with a name, type, and selected by a single label."
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
        /*.def(
            "set_name",
            [](UnionMember& u, const std::string n) {
                return u.name(n);
            },
            "Set the union member's name, returning the member for chained calls.") */
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
        /*.def(
            "set_id",
            &UnionMember::id,
            "Set the ID annotation of this union member and return the member for call chaining.") */
        .def_property(
            "pointer",
            &UnionMember::is_pointer,
            &UnionMember::pointer,
            "Boolean flag for pointer status of union member."
        )
        /*.def(
            "set_pointer",
            &UnionMember::pointer,
            "Set boolean flag for pointer status of this union member and return the member for call chaining.") */
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
        );
        /*.def(
            "set_labels",
            (UnionMember& (UnionMember::*)(const UnionMember::LabelSeq&)) &UnionMember::labels,
            "Set the labels that select this member, returning the member for chained calls."); */
}

template<>
void process_inits<UnionMember>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<UnionMember>(m, "UnionMember");
        }
    );  
}

}

