#include "PyConnext.hpp"
#include <dds/core/xtypes/MemberType.hpp>

using namespace dds::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<Member>& cls) {
    cls
        .def(
            py::init<const std::string&, const DynamicType&>(),
            py::arg("name"),
            py::arg("data_type")
        )
        .def(
            py::init([](const std::string& name, const DynamicType& type, int32_t id, bool is_key, bool is_optional, bool is_pointer) {
                Member m(name, type);
                if (id != -1) m.id(id);
                m.key(is_key).optional(is_optional).pointer(is_pointer);
                return m;
            }),
            py::arg("name"),
            py::arg("data_type"),
            py::arg("id") = -1,
            py::arg("is_key") = false,
            py::arg("is_optional") = false,
            py::arg("is_pointer") = false,
            "Create a Member with specified properties"
        )
        .def_property(
            "name",
            [](const Member& member) {
                return member.name().to_std_string();
            },
            [](Member& member, std::string& value) {
                member.name(std::string(value));
            },
            "The member name."
        )
        .def_property(
            "is_key",
            &Member::is_key,
            &Member::key,
            "Member key field status."
        )
        .def_property(
            "id",
            &Member::get_id,
            &Member::id,
            "The member ID."
        )
        .def_property(
            "pointer",
            &Member::is_pointer,
            &Member::pointer,
            "Member pointer status"
        )
        .def_property(
            "optional",
            &Member::is_optional,
            &Member::optional,
            "Member optional status"
        )
        .def_property_readonly(
            "bitset",
            &Member::is_bitset,
            "Checks if member is a bitset"
        )
        .def(
            "set_name",
            [](Member& member, std::string& value) {
                return member.name(std::string(value));
            },
            "Set the member name."
        )
        /*.def("set_id", &Member::id, "Set the member ID.")
        .def("set_key_flag", &Member::key, "Set member key status.")
        .def("set_pointer_flag", &Member::pointer, "Set member pointer flag.")
        .def("set_optional_flag", &Member::optional, "Sets the member optional flag") */
        .def_property_readonly(
            "type",
            [](const Member& member) {
                auto dt = member.type();
                return py_cast_type(dt);
            },
            "Gets the member type."
        );
}

template<>
void process_inits<Member>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<Member>(m, "Member");
        }
    );  
}

}
