#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>
#include "PySafeEnum.hpp"

using namespace dds::core::policy;

template<>
void pyrti::init_class_defs(py::class_<Ownership>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates an ownership policy set to shared."
        )
        .def(
            py::init<
                OwnershipKind
            >(),
            py::arg("kind"),
            "Creates an instance with the specified ownership kind."
        )
        .def_property(
            "kind",
            (OwnershipKind (Ownership::*)() const) &Ownership::kind,
            (Ownership& (Ownership::*)(OwnershipKind)) &Ownership::kind,
            "The ownership kind."
        )
        .def_static(
            "shared",
            &Ownership::Shared,
            "Creates a Ownership instance with shared kind."
        )
        .def_static(
            "exclusive",
            &Ownership::Exclusive,
            "Creates a Ownership instance with exclusive kind."
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

static
void init_dds_ownership_kind(py::module& m) {
    auto ok = init_dds_safe_enum<OwnershipKind_def>(m, "OwnershipKind");

    py::enum_<OwnershipKind::type>(ok, "OwnershipKind")
        .value(
            "SHARED",
            OwnershipKind::type::SHARED,
            "[default] Indicates shared ownership for each instance."
            "\n\n"
            "Multiple writers are allowed to update the same instance and all "
            "the updates are made available to the readers. In other words "
            "there is no concept of an owner for the instances."
            "\n\n"
            "This is the default behavior if the OWNERSHIP policy is not "
            "specified or supported."
        )
        .value(
            "EXCLUSIVE",
            OwnershipKind::type::EXCLUSIVE,
            "Indicates each instance can only be owned by one DataWriter, but "
            "the owner of an instance can change dynamically."
            "\n\n"
            "The selection of the owner is controlled by the setting of the "
            "OWNERSHIP_STRENGTH policy. The owner is always set to be the "
            "highest-strength DataWriter object among the ones currently "
            "active (as determined by the LIVELINESS)."
        )
        .export_values();
}

template<>
void pyrti::process_inits<Ownership>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            init_dds_ownership_kind(m);
            return pyrti::init_class<Ownership>(m, "Ownership");
        }
    );
}