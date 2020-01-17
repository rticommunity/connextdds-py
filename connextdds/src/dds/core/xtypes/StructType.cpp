#include "PyConnext.hpp"
#include <dds/core/xtypes/StructType.hpp>

using namespace dds::core::xtypes;
using namespace rti::core::xtypes;

template<>
void pyrti::init_class_defs(py::class_<StructType, AbstractConstructedType<Member>>& cls) {
    cls
        .def(
             py::init<const std::string>(),
             py::arg("name"),
             "Creates an empty StructType."
        )
        .def(
             py::init(
                [](const std::string& name, ExtensibilityKind extensibility) {
                    StructType st(name);
                    st.extensibility_kind(extensibility);
                    return st;
                }
            ),
             py::arg("name"),
             py::arg("extensibility"),
             "Creates an empty StructType with a specified extensibility."
        )
        .def(
             py::init<const std::string&, const StructType&>(),
             py::arg("name"),
             py::arg("parent"),
             "Create an empty StructType with a base type."
        )
        .def(
             py::init(
                [](const std::string& name, const StructType& parent, ExtensibilityKind extensibility) {
                    StructType st(name, parent);
                    st.extensibility_kind(extensibility);
                    return st;
                }
            ),
             py::arg("name"),
             py::arg("parent"),
             py::arg("extensibility"),
             "Creates an empty StructType with a base type and specified extensibility."
        )
        .def(
             py::init<const std::string&, const std::vector<Member>&>(),
             py::arg("name"),
             py::arg("members"),
             "Create a StructType with the provided members."
        )
        .def(
             py::init(
                [](const std::string& name, const std::vector<Member>& members, ExtensibilityKind extensibility) {
                    StructType st(name, members);
                    st.extensibility_kind(extensibility);
                    return st;
                }
            ),
             py::arg("name"),
             py::arg("members"),
             py::arg("extensibility"),
             "Creates a StructType with the provided members and specified extensibility."
        )
        .def(
             py::init<const std::string&, const StructType&, const std::vector<Member>&>(),
             py::arg("name"),
             py::arg("parent"),
             py::arg("members"),
             "Create a StructType with a base type and the provided members."
        )
        .def(
             py::init([](const std::string& name, const StructType& parent, const std::vector<Member> members, ExtensibilityKind extensibility) {
                 StructType st(name, parent, members);
                 st.extensibility_kind(extensibility);
                 return st;
             }),
             py::arg("name"),
             py::arg("parent"),
             py::arg("members"),
             py::arg("extensibility"),
             "Creates a StructType with a base types, the provided members, and the specified extensibility."
        )
        .def(
            py::init(
                [](DynamicType& dt) {
                    return static_cast<StructType&>(dt);
                }
            ),
            py::arg("type"),
            "Cast a DynamicType to a StructType"
        )
        .def_property(
            "extensibility_kind",
            (ExtensibilityKind (StructType::*)()const) &StructType::extensibility_kind,
            (StructType& (StructType::*)(ExtensibilityKind)) &StructType::extensibility_kind,
            "Struct's extensibility kind."
        )
        /*.def(
            "set_extensibility_kind",
            (StructType& (StructType::*)(ExtensibilityKind)) &StructType::extensibility_kind,
            "Set the struct's extensibility kind."
        )*/
        .def_property_readonly(
            "has_parent",
            &StructType::has_parent,
            "Indicates if this type has a base type."
        )
        .def_property_readonly(
            "parent",
            &StructType::parent,
            "Retrieve the base type."
        )
        .def(
            "find_member_by_id",
            &StructType::find_member_by_id,
            py::arg("member_id"),
            "Get the index of the member with a specfic ID."
        )
        .def(
            "add_member", 
            (StructType& (StructType::*)(const Member&)) &StructType::add_member,
            py::arg("member"), 
            "Adds a member at the end."
        )
        .def(
            "add_members",
            (StructType& (StructType::*)(const std::vector<Member>&)) &StructType::add_members,
            py::arg("members"),
            "Adds all members of the sequence to the end."
        );
}

template<>
void pyrti::process_inits<StructType>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<StructType, AbstractConstructedType<Member>>(m, "StructType");
        }
    ); 
}