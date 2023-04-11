/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */

#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<EntityName>& cls)
{
    cls.def(py::init<>(), "Creates the default policy (no name).")
            .def(py::init<const std::string&>(),
                    py::arg("name"),
                    "Creates an instance that specifies an entity name.")
            .def_property(
                    "name",
                    (rti::core::optional_value<std::string>(EntityName::*)()
                                    const)
                            & EntityName::name,
                    [](EntityName& e,
                            const rti::core::optional_value<std::string>&
                                    name) {
                        if (!name.has_value()) {
                            e.name().reset();
                        } else {
                            e.name(name.value());
                        }
                        return e;
                    },
                    "Sets the entity name.")
            .def_property(
                    "role_name",
                    (rti::core::optional_value<std::string>(EntityName::*)()
                                    const)
                            & EntityName::role_name,
                    [](EntityName& e,
                            const rti::core::optional_value<std::string>&
                                    role_name) {
                        if (!role_name.has_value()) {
                            e.role_name().reset();
                        } else {
                            e.role_name(role_name.value());
                        }
                        return e;
                    },
                    "Sets the role name.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<EntityName>(py::module& m, ClassInitList& l)
{
    l.push_back(
            [m]() mutable { return init_class<EntityName>(m, "EntityName"); });
}

}  // namespace pyrti
