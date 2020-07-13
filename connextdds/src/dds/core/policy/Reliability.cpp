#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>
#include "PySafeEnum.hpp"

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<Reliability>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates a best-effort policy."
        )
        .def(
            py::init<
                ReliabilityKind,
                const dds::core::Duration&
            >(),
            py::arg("kind"),
            py::arg_v("max_blocking_time", dds::core::Duration::from_millisecs(100), "dds.Duration.from_milliseconds(100)"),
            "Creates an instance with the specified reliability kind an "
            "optionally a specific maximum blocking time."
        )
        .def_property(
            "kind",
            (ReliabilityKind (Reliability::*)() const) &Reliability::kind,
            (Reliability& (Reliability::*)(ReliabilityKind)) &Reliability::kind,
            "The reliability kind."
        )
        .def_property(
            "max_blocking_time",
            (dds::core::Duration (Reliability::*)() const) &Reliability::max_blocking_time,
            (Reliability& (Reliability::*)(const dds::core::Duration&)) &Reliability::max_blocking_time,
            "The maximum time a DataWriter may block on a call to write()."
            "\n\n"
            "This property's getter returns a deep copy."
        )
        .def_property(
            "acknowledgment_kind",
            [](const Reliability& r) {
                return r->acknowledgment_kind();
            },
            [](Reliability& r, rti::core::policy::AcknowledgmentKind& ak) {
                return r->acknowledgment_kind(ak);
            },
            "The kind of reliable acknowledgment."
        )
        .def_static(
            "reliable",
            &Reliability::Reliable,
            py::arg_v("max_blocking_time", dds::core::Duration::from_millisecs(100), "dds.Duration.from_milliseconds(100)"),
            "Creates a policy with ReliabilityKind.RELIABLE and optionally a "
            "max blocking time."
        )
        .def_static(
            "best_effort",
            &Reliability::BestEffort,
            "Creates a policy with ReliabilityKind.BEST_EFFORT."
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
void init_dds_reliability_kind(py::module& m) {
    init_dds_safe_enum<ReliabilityKind_def>(m, "ReliabilityKind",
        [](py::object& o) {
             py::enum_<ReliabilityKind::type>(o, "Enum")
                .value(
                    "BEST_EFFORT",
                    ReliabilityKind::type::BEST_EFFORT,
                    "Indicates that it is acceptable to not retry propagation of any "
                    "samples."
                    "\n\n"
                    "Presumably new values for the samples are generated often enough "
                    "that it is not necessary to re-send or acknowledge any samples."
                    "\n\n"
                    "[default] for DataReader and Topic."
                )
                .value(
                    "RELIABLE",
                    ReliabilityKind::type::RELIABLE,
                    "Specifies that RTI Connext will attempt to deliver all samples "
                    "in its history. Missed samples may be retried."
                    "\n\n"
                    "In steady-state (no modifications communicated via the "
                    "DataWriter), RTI Connext guarantees that all samples in the "
                    "DataWriter history will eventually be delivered to all the "
                    "DataReader objects (subject to timeouts that indicate loss of "
                    "communication with a particular Subscriber)."
                    "\n\n"
                    "Outside steady state, the HISTORY and RESOURCE_LIMITS policies "
                    "will determine how samples become part of the history and "
                    "whether samples can be discarded from it."
                    "\n\n"
                    "[default] for DataWriter."
                )
                .export_values();
        }
    );
}

template<>
void process_inits<Reliability>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            init_dds_reliability_kind(m);
            return init_class<Reliability>(m, "Reliability");
        }
    );
}

}
