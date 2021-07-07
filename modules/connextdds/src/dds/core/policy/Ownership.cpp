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
void init_class_defs(py::class_<Ownership>& cls)
{
    cls.def(py::init<>(), "Creates an ownership policy set to shared.")
            .def(py::init<OwnershipKind>(),
                 py::arg("kind"),
                 "Creates an instance with the specified ownership kind.")
            .def_property(
                    "kind",
                    (OwnershipKind(Ownership::*)() const) & Ownership::kind,
                    (Ownership & (Ownership::*) (OwnershipKind))
                            & Ownership::kind,
                    "The ownership kind.")
            .def_property_readonly_static(
                    "shared",
                    [](py::object&) {
                        return Ownership::Shared();
                    },
                    "Creates a Ownership instance with shared kind.")
            .def_property_readonly_static(
                    "exclusive",
                    [](py::object&) {
                        return Ownership::Exclusive();
                    },
                    "Creates a Ownership instance with exclusive kind.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

static void init_dds_ownership_kind(py::module& m)
{
    init_dds_safe_enum<
            OwnershipKind_def>(m, "OwnershipKind", [](py::object& o) {
        py::enum_<OwnershipKind::type>(o, "OwnershipKind")
                .value("SHARED",
                       OwnershipKind::type::SHARED,
                       "[default] Indicates shared ownership for each instance."
                       "\n\n"
                       "Multiple writers are allowed to update the same "
                       "instance and all "
                       "the updates are made available to the readers. In "
                       "other words "
                       "there is no concept of an owner for the instances."
                       "\n\n"
                       "This is the default behavior if the OWNERSHIP policy "
                       "is not "
                       "specified or supported.")
                .value("EXCLUSIVE",
                       OwnershipKind::type::EXCLUSIVE,
                       "Indicates each instance can only be owned by one "
                       "DataWriter, but "
                       "the owner of an instance can change dynamically."
                       "\n\n"
                       "The selection of the owner is controlled by the "
                       "setting of the "
                       "OWNERSHIP_STRENGTH policy. The owner is always set to "
                       "be the "
                       "highest-strength DataWriter object among the ones "
                       "currently "
                       "active (as determined by the LIVELINESS).")
                .export_values();
    });
}

template<>
void process_inits<Ownership>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        init_dds_ownership_kind(m);
        return init_class<Ownership>(m, "Ownership");
    });
}

}  // namespace pyrti
