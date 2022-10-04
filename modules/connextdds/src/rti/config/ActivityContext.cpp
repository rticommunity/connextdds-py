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

#if rti_connext_version_gte(6, 1, 0, 0)

#include <rti/config/ActivityContext.hpp>
#include <PyMaskType.hpp>

using namespace rti::config::activity_context;

namespace pyrti {

template<>
void init_class_defs(py::class_<AttributeKindMask>& cls)
{
    cls.def_property_readonly_static(
                "ALL",
                [](py::object&) {
                    return AttributeKindMask::all();
                },
                "Provide all the possible attributes of the resouce "
                "of the activity context.")
            .def_property_readonly_static(
                "NONE",
                [](py::object&) {
                    return AttributeKindMask::none();
                },
                "No attributes of the resource provided for the "
                "activity context.")
            .def_property_readonly_static(
                "DEFAULT",
                [](py::object&) {
                    return AttributeKindMask::default_mask();
                },
                "Default mask.")
            .def_property_readonly_static(
                "ENTITY_NAME",
                [](py::object&) {
                    return AttributeKindMask::entity_name();
                },
                "Provide the entity name to the resource of the Activity "
                "Context. The name attribute is specified by \"N\".")
            .def_property_readonly_static(
                "DOMAIN_ID",
                [](py::object&) {
                    return AttributeKindMask::domain_id();
                },
                "Provide the domain ID to the resource of the Activity "
                "Context. The name attribute is specified by \"D\".")
            .def_property_readonly_static(
                "ENTITY_KIND",
                [](py::object&) {
                    return AttributeKindMask::entity_kind();
                },
                "Provide the entity kind to the resource of the Activity "
                "Context. The name attribute is specified by \"E\".")
            .def_property_readonly_static(
                "TYPE",
                [](py::object&) {
                    return AttributeKindMask::type();
                },
                "Provide the data type to the resource of the Activity "
                "Context. The name attribute is specified by \"C\".")
            .def_property_readonly_static(
                "TOPIC",
                [](py::object&) {
                    return AttributeKindMask::topic();
                },
                "Provide the Topicto the resource of the Activity "
                "Context. The name attribute is specified by \"T\".")
            .def_property_readonly_static(
                "GUID_PREFIX",
                [](py::object&) {
                    return AttributeKindMask::guid_prefix();
                },
                "Provide the entity GUID prefix to the resource of the Activity "
                "Context.");
}


template<>
void process_inits<AttributeKindMask>(py::module& m, ClassInitList& l)
{
     l.push_back([m]() mutable {
        auto cls = init_mask_type<AttributeKindMask, uint64_t>(
                m,
                "ActivityContextMask",
                "Create an ActivityContextMask with no bits set.");

        m.def(
            "set_activity_context",
            &set_attribute_mask,
            py::arg("attribute_mask"),
            "Set the activity context mask."
        );

        return [cls]() mutable {
            init_class_defs<AttributeKindMask>(cls);
        };
    });
}

}  // namespace pyrti

void init_activity_context(py::module& m, pyrti::ClassInitList& l, pyrti::DefInitVector&) {
    pyrti::process_inits<AttributeKindMask>(m, l);
}

#endif