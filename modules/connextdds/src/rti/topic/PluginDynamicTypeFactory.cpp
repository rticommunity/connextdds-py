/*
 * (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */

#include "PyConnext.hpp"
#include <rti/core/xtypes/PluginDynamicTypeFactory.hpp>
#include <dds/core/Duration.hpp>
#include <rti/core/constants.hpp>

using namespace rti::core::xtypes;
using timestamp = std::chrono::time_point<std::chrono::system_clock>;

namespace pyrti {

struct PluginDynamicTypeHolder {
    DynamicTypeImpl *type;
};

template<>
void init_class_defs(py::class_<PluginDynamicTypeHolder>& cls)
{
    cls.def("clone", [](const PluginDynamicTypeHolder& self) {
        DynamicTypeImpl copy = *self.type;
        return py_cast_type(copy);
    });
}

template<>
void init_class_defs(py::class_<PluginDynamicTypeFactory>& cls)
{
    cls  // define PluginDynamicTypeFactory python class
            .def("create_struct",
                 [](PluginDynamicTypeFactory& self,
                    const std::string& name,
                    dds::core::xtypes::ExtensibilityKind extensibility,
                    int32_t type_size,
                    const std::vector<int32_t>& member_offsets)
                         -> PluginDynamicTypeHolder {
                     return PluginDynamicTypeHolder { self.create_struct(
                             name,
                             extensibility,
                             type_size,
                             member_offsets) };
                 })
            .def("add_member_previous",
                 &PluginDynamicTypeFactory::add_member,
                 py::arg("type"),
                 py::arg("name"),
                 py::arg("member_type"),
                 py::arg("id") = dds::core::xtypes::Member::INVALID_ID,
                 py::arg("is_key") = false,
                 py::arg("is_optional") = false,
                 py::arg("is_external") = false)
            .def(
                    "add_member",
                    [](PluginDynamicTypeFactory& factory,
                       PluginDynamicTypeHolder& type_holder,
                       const std::string& name,
                       const dds::core::xtypes::DynamicType& member_type,
                       int32_t id,
                       bool is_key,
                       bool is_optional,
                       bool is_external) {
                        return factory.add_member(
                                static_cast<StructTypeImpl*>(type_holder.type),
                                name,
                                member_type,
                                id,
                                is_key,
                                is_optional,
                                is_external);
                    },
                    py::arg("type"),
                    py::arg("name"),
                    py::arg("member_type"),
                    py::arg("id") = dds::core::xtypes::Member::INVALID_ID,
                    py::arg("is_key") = false,
                    py::arg("is_optional") = false,
                    py::arg("is_external") = false)
            .def(
                    "add_member",
                    [](PluginDynamicTypeFactory& factory,
                       PluginDynamicTypeHolder& type_holder,
                       const std::string& name,
                       const PluginDynamicTypeHolder& member_type_holder,
                       int32_t id,
                       bool is_key,
                       bool is_optional,
                       bool is_external) {
                        return factory.add_member(
                                static_cast<StructTypeImpl*>(type_holder.type),
                                name,
                                *member_type_holder.type,
                                id,
                                is_key,
                                is_optional,
                                is_external);
                    },
                    py::arg("type"),
                    py::arg("name"),
                    py::arg("member_type"),
                    py::arg("id") = dds::core::xtypes::Member::INVALID_ID,
                    py::arg("is_key") = false,
                    py::arg("is_optional") = false,
                    py::arg("is_external") = false)
            .def_property_readonly_static(
                    "instance",
                    [](py::object&) -> PluginDynamicTypeFactory& {
                        return PluginDynamicTypeFactory::instance();
                    })
            .def_static("delete_instance", []() {
                PluginDynamicTypeFactory::delete_instance();
                DDS_TypeCodeFactory_finalize_instance();
            });
}

template<>
void process_inits<PluginDynamicTypeFactory>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<PluginDynamicTypeFactory>(
                m,
                "_PluginDynamicTypeFactory");
    });

    l.push_back([m]() mutable {
        return init_class<PluginDynamicTypeHolder>(
                m,
                "_PluginDynamicTypeHolder");
    });
}

}  // namespace pyrti
