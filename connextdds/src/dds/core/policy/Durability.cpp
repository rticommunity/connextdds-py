#include "PyConnext.hpp"
#include "PySafeEnum.hpp"
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<Durability>& cls)
{
    cls.def(py::init<>(), "Create an empty Durability QoS policy.")
            .def(py::init<DurabilityKind>(),
                 py::arg("kind"),
                 "Create a Durability QoS policy with the given kind.")
            .def_property(
                    "kind",
                    (DurabilityKind(Durability::*)() const) & Durability::kind,
                    (Durability & (Durability::*) (DurabilityKind))
                            & Durability::kind,
                    "Get/set the Durability kind.")
            .def_property(
                    "direct_communication",
                    [](Durability& d) { return d->direct_communication(); },
                    [](Durability& d, bool v) {
                        return d->direct_communication(v);
                    },
                    "Get/set whether a DataReader should receive samples "
                    "directly "
                    "from a TRANSIENT or PERSISTENT DataWriter.")
            .def_static(
                    "volatile",
                    &Durability::Volatile,
                    "Create a Durability QoS policy with kind set to VOLATILE.")
            .def_static(
                    "transient_local",
                    &Durability::TransientLocal,
                    "Create a Durability QoS policy with kind set to "
                    "TRANSIENT_LOCAL.")
            .def_static(
                    "transient",
                    &Durability::Transient,
                    "Create a Durability QoS policy with kind set to "
                    "TRANSIENT.")
            .def_static(
                    "persistent",
                    &Durability::Persistent,
                    "Create a Durability QoS policy with kind set to "
                    "PERSISTENT.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

static void init_dds_durability_kind(py::module& m)
{
    init_dds_safe_enum<
            DurabilityKind_def>(m, "DurabilityKind", [](py::object& o) {
        py::enum_<DurabilityKind::type>(o, "Enum")
                .value("VOLATILE",
                       DurabilityKind::type::VOLATILE,
                       "[default] RTI Connext does not need to keep any "
                       "samples of data "
                       "instances on behalf of any DataReader that is unknown "
                       "by the "
                       "DataWriter at the time the instance is written."
                       "\n\n"
                       "In other words, RTI Connext will only attempt to "
                       "provide the "
                       "data to existing subscribers."
                       "\n\n"
                       "This option does not require RTI Persistence Service.")
                .value("TRANSIENT_LOCAL",
                       DurabilityKind::type::TRANSIENT_LOCAL,
                       "RTI Connext will attempt to keep some samples so that "
                       "they can "
                       "be delivered to any potential late-joining DataReader."
                       "\n\n"
                       "Which particular samples are kept depends on other QoS "
                       "such as "
                       "History and ResourceLimits. RTI Connext is only "
                       "required to keep "
                       "the data in memory of the DataWriter that wrote the "
                       "data."
                       "\n\n"
                       "Data is not required to survive the DataWriter."
                       "\n\n"
                       "For this setting to be effective, you must also set "
                       "the "
                       "Reliability.kind to ReliabilityKind.RELIABLE. "
                       "\n\n"
                       "This option does not require RTI Persistence Service.")
                .value("TRANSIENT",
                       DurabilityKind::type::TRANSIENT,
                       "RTI Connext will attempt to keep some samples so that "
                       "they can "
                       "be delivered to any potential late-joining DataReader."
                       "\n\n"
                       "Which particular samples are kept depends on other QoS "
                       "such as "
                       "History and ResourceLimits. RTI Connext is only "
                       "required to keep "
                       "the data in memory and not in permanent storage."
                       "\n\n"
                       "Data is not tied to the lifecycle of the DataWriter."
                       "\n\n"
                       "Data will survive the DataWriter."
                       "\n\n"
                       "This option requires RTI Persistence Service.")
                .value("PERSISTENT",
                       DurabilityKind::type::PERSISTENT,
                       "Data is kept on permanent storage, so that they can "
                       "outlive a "
                       "system session."
                       "\n\n"
                       "This option requires RTI Persistence Service.")
                .export_values();
    });
}

template<>
void process_inits<Durability>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        init_dds_durability_kind(m);
        return init_class<Durability>(m, "Durability");
    });
}

}  // namespace pyrti
