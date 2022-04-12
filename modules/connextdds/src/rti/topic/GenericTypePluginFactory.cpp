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
#include <dds/core/Duration.hpp>
#include <rti/core/constants.hpp>

#include "IdlTypeSupport.hpp"
#include "PyDynamicTypeMap.hpp"

using namespace rti::core::xtypes;
using namespace rti::topic::cdr;
using timestamp = std::chrono::time_point<std::chrono::system_clock>;

namespace pyrti {

template<>
void init_class_defs(py::class_<GenericTypePluginFactory>& cls)
{
    using namespace dds::core::xtypes;

    cls.def("create_struct",
            [](GenericTypePluginFactory& self,
               const std::string& name,
               ExtensibilityKind extensibility,
               uint32_t type_size,
               const std::vector<uint32_t>& member_offsets) {
                return TypePlugin { self.create_struct(
                                            name,
                                            extensibility,
                                            type_size,
                                            member_offsets),
                                    nullptr };
            });

    cls.def("create_struct",
            [](GenericTypePluginFactory& self,
               const std::string& name,
               const DynamicType& parent,
               ExtensibilityKind extensibility,
               uint32_t type_size,
               const std::vector<uint32_t>& member_offsets) {
                if (parent.kind() != TypeKind::STRUCTURE_TYPE) {
                    throw py::value_error("base type must be an idl.struct type");
                }
                return TypePlugin { self.create_struct(
                                            name,
                                            static_cast<const StructType&>(
                                                    parent),
                                            extensibility,
                                            type_size,
                                            member_offsets),
                                    nullptr };
            });

    cls.def("create_union",
            [](GenericTypePluginFactory& self,
               const std::string& name,
               const DynamicType& discriminator,
               ExtensibilityKind extensibility,
               uint32_t type_size,
               const std::vector<uint32_t>& member_offsets) {
                return TypePlugin { self.create_union(
                                            name,
                                            discriminator,
                                            extensibility,
                                            type_size,
                                            member_offsets),
                                    nullptr };
            });

    cls.def("create_alias",
            [](GenericTypePluginFactory& self,
               const std::string& name,
               const DynamicType& related_type,
               uint32_t type_size) {
                return TypePlugin { self.create_alias(
                                            name,
                                            related_type,
                                            type_size),
                                    nullptr };
            });

    cls.def("create_alias",
            [](GenericTypePluginFactory& self,
               const std::string& name,
               const TypePlugin& related_type_holder,
               uint32_t type_size) {
                return TypePlugin { self.create_alias(
                                            name,
                                            *related_type_holder.type,
                                            type_size),
                                    nullptr };
            });

    cls.def("create_sequence",
            [](GenericTypePluginFactory& self,
               const DynamicType& element_type,
               uint32_t bound) {
                return TypePlugin {
                    self.create_sequence(element_type, bound),
                    nullptr
                };
            });

    cls.def("create_array",
            [](GenericTypePluginFactory& self,
               const DynamicType& element_type,
               uint32_t dimension) {
                return TypePlugin { self.create_array(element_type, dimension),
                                    nullptr };
            });

    cls.def("create_array",
            [](GenericTypePluginFactory& self,
               TypePlugin& type_holder,
               uint32_t dimension) {
                return TypePlugin {
                    self.create_array(*type_holder.type, dimension),
                    nullptr
                };
            });

    cls.def("create_enum",
            [](GenericTypePluginFactory& self,
               const std::string& name,
               ExtensibilityKind extensibility,
               const std::vector<EnumMember>& members) {
                return TypePlugin {
                    self.create_enum(name, extensibility, members),
                    nullptr
                };
            });

    cls.def(
            "add_member",
            [](GenericTypePluginFactory& factory,
               TypePlugin& type_holder,
               const std::string& name,
               const DynamicType& member_type,
               int32_t id,
               bool is_key,
               bool is_optional,
               bool is_external) {
                return factory.add_member(
                        *static_cast<StructTypeImpl*>(type_holder.type),
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
            py::arg("id") = Member::INVALID_ID,
            py::arg("is_key") = false,
            py::arg("is_optional") = false,
            py::arg("is_external") = false);

    cls.def(
            "add_member",
            [](GenericTypePluginFactory& factory,
               TypePlugin& type_holder,
               const std::string& name,
               const TypePlugin& member_type_holder,
               int32_t id,
               bool is_key,
               bool is_optional,
               bool is_external) {
                return factory.add_member(
                        *static_cast<StructTypeImpl*>(type_holder.type),
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
            py::arg("id") = Member::INVALID_ID,
            py::arg("is_key") = false,
            py::arg("is_optional") = false,
            py::arg("is_external") = false);

    cls.def(
            "add_union_member",
            [](GenericTypePluginFactory& factory,
               TypePlugin& type_holder,
               const std::string& name,
               const DynamicType& member_type,
               const UnionMember::LabelSeq& labels,
               int32_t id,
               bool is_external) {
                return factory.add_member(
                        *static_cast<UnionTypeImpl*>(type_holder.type),
                        name,
                        member_type,
                        labels,
                        id,
                        is_external);
            },
            py::arg("type"),
            py::arg("name"),
            py::arg("member_type"),
            py::arg("labels"),
            py::arg("id") = Member::INVALID_ID,
            py::arg("is_external") = false);

    cls.def(
            "add_union_member",
            [](GenericTypePluginFactory& factory,
               TypePlugin& type_holder,
               const std::string& name,
               const TypePlugin& member_type_holder,
               const UnionMember::LabelSeq& labels,
               int32_t id,
               bool is_external) {
                return factory.add_member(
                        *static_cast<UnionTypeImpl*>(type_holder.type),
                        name,
                        *member_type_holder.type,
                        labels,
                        id,
                        is_external);
            },
            py::arg("type"),
            py::arg("name"),
            py::arg("member_type"),
            py::arg("labels"),
            py::arg("id") = Member::INVALID_ID,
            py::arg("is_external") = false);

    cls.def(
            "create_type_plugin",
            [](GenericTypePluginFactory& factory,
               TypePlugin& type_holder) {
                CTypePlugin& plugin =
                        factory.create_type_plugin(*type_holder.type);
                type_holder.type_plugin = &plugin;
            },
            py::arg("type"));

    cls.def_property_readonly_static(
            "instance",
            [](py::object&) -> GenericTypePluginFactory& {
                return GenericTypePluginFactory::instance();
            });

    cls.def_property_readonly_static(
            "public_instance",
            [](py::object&) -> GenericTypePluginFactory& {
                return GenericTypePluginFactory::no_accessor_instance();
            });

    cls.def_static("delete_instance", []() {
        GenericTypePluginFactory::delete_instance();
        PyDynamicTypeMap::finalize();
        DDS_TypeCodeFactory_finalize_instance();
    });
}

template<>
void process_inits<GenericTypePluginFactory>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<GenericTypePluginFactory>(
                m,
                "_GenericTypePluginFactory");
    });

    l.push_back([m]() mutable {
        return init_class<TypePlugin>(
                m,
                "_TypePlugin");
    });
}

}  // namespace pyrti
