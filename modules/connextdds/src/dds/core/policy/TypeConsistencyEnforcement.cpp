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
#include <dds/core/policy/CorePolicy.hpp>
#include "PySafeEnum.hpp"

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<TypeConsistencyEnforcement>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<TypeConsistencyEnforcementKind>(),
                 py::arg("kind"),
                 "Creates an instance with a specific enforcement kind.")
            .def_property(
                    "kind",
                    (TypeConsistencyEnforcementKind(
                            TypeConsistencyEnforcement::*)() const)
                            & TypeConsistencyEnforcement::kind,
                    (TypeConsistencyEnforcement
                     & (TypeConsistencyEnforcement::*) (TypeConsistencyEnforcementKind))
                            & TypeConsistencyEnforcement::kind,
                    "The enforcement kind.")
#if rti_connext_version_gte(6, 0, 0, 0)
            .def_property(
                    "ignore_sequence_bounds",
                    (bool (TypeConsistencyEnforcement::*)() const)
                            & TypeConsistencyEnforcement::
                                    ignore_sequence_bounds,
                    (TypeConsistencyEnforcement
                     & (TypeConsistencyEnforcement::*) (bool) )
                            & TypeConsistencyEnforcement::
                                    ignore_sequence_bounds,
                    "Controls whether sequence bounds are ignored.")
            .def_property(
                    "ignore_string_bounds",
                    (bool (TypeConsistencyEnforcement::*)() const)
                            & TypeConsistencyEnforcement::ignore_string_bounds,
                    (TypeConsistencyEnforcement
                     & (TypeConsistencyEnforcement::*) (bool) )
                            & TypeConsistencyEnforcement::ignore_string_bounds,
                    "Controls whether string bounds are ignored.")
            .def_property(
                    "ignore_member_names",
                    (bool (TypeConsistencyEnforcement::*)() const)
                            & TypeConsistencyEnforcement::ignore_member_names,
                    (TypeConsistencyEnforcement
                     & (TypeConsistencyEnforcement::*) (bool) )
                            & TypeConsistencyEnforcement::ignore_member_names,
                    "Controls whether member names are ignored.")
            .def_property(
                    "prevent_type_widening",
                    (bool (TypeConsistencyEnforcement::*)() const)
                            & TypeConsistencyEnforcement::prevent_type_widening,
                    (TypeConsistencyEnforcement
                     & (TypeConsistencyEnforcement::*) (bool) )
                            & TypeConsistencyEnforcement::prevent_type_widening,
                    "Controls whether type widening is prevented.")
            .def_property(
                    "force_type_validation",
                    (bool (TypeConsistencyEnforcement::*)() const)
                            & TypeConsistencyEnforcement::force_type_validation,
                    (TypeConsistencyEnforcement
                     & (TypeConsistencyEnforcement::*) (bool) )
                            & TypeConsistencyEnforcement::force_type_validation,
                    "Controls whether type validation is forced.")
            .def_property(
                    "ignore_enum_literal_names",
                    (bool (TypeConsistencyEnforcement::*)() const)
                            & TypeConsistencyEnforcement::
                                    ignore_enum_literal_names,
                    (TypeConsistencyEnforcement
                     & (TypeConsistencyEnforcement::*) (bool) )
                            & TypeConsistencyEnforcement::
                                    ignore_enum_literal_names,
                    "Controls whether enumeration literal names are ignored.")
            .def_property_readonly_static(
                    "auto_type_coercion",
                    [](py::object&) {
                        return TypeConsistencyEnforcement::AutoTypeCoercion();
                    },
                    "Creates an instance with "
                    "TypeConsistencyEnforcementKind.AUTO_TYPE_COERCION.")
#endif
            .def_property_readonly_static(
                    "allow_type_coercion",
                    [](py::object&) {
                        return TypeConsistencyEnforcement::AllowTypeCoercion();
                    },
                    "Creates an instance with "
                    "TypeConsistencyEnforcementKind.ALLOW_TYPE_COERCION.")
            .def_property_readonly_static(
                    "disallow_type_coercion",
                    [](py::object&) {
                        return TypeConsistencyEnforcement::DisallowTypeCoercion();
                    },
                    "Creates an instance with "
                    "TypeConsistencyEnforcementKind.DISALLOW_TYPE_COERCION.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

static void init_type_consistency_enforcement_kind(py::module& m)
{
    init_dds_safe_enum<TypeConsistencyEnforcementKind_def>(
            m,
            "TypeConsistencyEnforcementKind",
            [](py::object& o) {
                py::enum_<TypeConsistencyEnforcementKind::type>(o, "TypeConsistencyEnforcementKind")
                        .value("DISALLOW_TYPE_COERCION",
                               TypeConsistencyEnforcementKind::type::
                                       DISALLOW_TYPE_COERCION,
                               "The DataWriter and the DataReader must support "
                               "the same data "
                               "type in order for them to communicate.")
                        .value("ALLOW_TYPE_COERCION",
                               TypeConsistencyEnforcementKind::type::
                                       ALLOW_TYPE_COERCION,
                               "The DataWriter and the DataReader need not "
                               "support the same data "
                               "type in order for them to communicate as long "
                               "as the "
                               "DataReader's type is assignable from the "
                               "DataWriter's type.")
#if rti_connext_version_gte(6, 0, 0, 0)
                        .value("AUTO_TYPE_COERCION",
                               TypeConsistencyEnforcementKind::type::
                                       AUTO_TYPE_COERCION,
                               "[default] This AUTO value will be applied as "
                               "TypeConsistencyKind.DISALLOW_TYPE_COERCION "
                               "when the data type "
                               "is annotated with @transfer_mode(SHMEM_REF) "
                               "while using C, "
                               "Traditional C++, or Modern C++ APIs. In all "
                               "other cases, this "
                               "AUTO value will be applied as "
                               "TypeConsistencyKind.ALLOW_TYPE_COERCION.")
#endif
                        .export_values();
            });
}

template<>
void process_inits<TypeConsistencyEnforcement>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        init_type_consistency_enforcement_kind(m);
        return init_class<TypeConsistencyEnforcement>(
                m,
                "TypeConsistencyEnforcement");
    });
}

}  // namespace pyrti
