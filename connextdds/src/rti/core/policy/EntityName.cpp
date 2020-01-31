#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<EntityName>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates the default policy (no name)."
        )
        .def(
            py::init<
                const std::string&
            >(),
            py::arg("name"),
            "Creates an instance that specifies an entity name."
        )
        .def_property(
            "name",
            [](const EntityName& en) {
                if (en.name().is_set()) {
                    return py::cast(en.name().get());
                }
                else {
                    return py::cast(nullptr);
                }
            },
            [](EntityName& en, py::object& value) {
                if (value.is_none()) {
                    return en.name(rti::core::optional_value<std::string>());
                }
                else {
                    return en.name(rti::core::optional_value<std::string>(py::cast<std::string>(value)));
                }
            },
            "Sets the entity name."
        )
        .def_property(
            "role_name",
            [](const EntityName& en) {
                if (en.name().is_set()) {
                    return py::cast(en.role_name().get());
                }
                else {
                    return py::cast(nullptr);
                }
            },
            [](EntityName& en, py::object& value) {
                if (value.is_none()) {
                    return en.role_name(rti::core::optional_value<std::string>());
                }
                else {
                    return en.role_name(rti::core::optional_value<std::string>(py::cast<std::string>(value)));
                }
            },
            "Sets the role name."
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
void process_inits<EntityName>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<EntityName>(m, "EntityName");
        }
    );
}

}
