#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>
#include "PySafeEnum.hpp"

using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<TypeSupport>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def_property(
                    "cdr_padding_kind",
                    (CdrPaddingKind(TypeSupport::*)() const)
                            & TypeSupport::cdr_padding_kind,
                    (TypeSupport & (TypeSupport::*) (CdrPaddingKind))
                            & TypeSupport::cdr_padding_kind,
                    "Determines whether or not the padding bytes will be set "
                    "to zero "
                    "during CDR serialization.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<TypeSupport>(py::module& m, ClassInitList& l)
{
    init_dds_safe_enum<
            CdrPaddingKind_def>(m, "CdrPaddingKind", [](py::object& o) {
        py::enum_<CdrPaddingKind::type>(o, "Enum")
                .value("ZERO",
                       CdrPaddingKind::type::ZERO,
                       "Set padding bytes to zero during CDR serialization.")
                .value("NOT_SET",
                       CdrPaddingKind::type::NOT_SET,
                       "Don't set padding bytes to any value.")
                .value("AUTO",
                       CdrPaddingKind::type::AUTO,
                       "Set the value automatically, depending on the Entity."
                       "\n\n"
                       "When set on a DomainParticipant the default behavior "
                       "is NOT_SET"
                       "\n\n"
                       "When set on a DataWriter or DataReader the behavior is "
                       "to "
                       "inherit the value from the DomainParticipant.")
                .export_values();
    });

    l.push_back([m]() mutable {
        return init_class<TypeSupport>(m, "TypeSupport");
    });
}

}  // namespace pyrti
