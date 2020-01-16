#include "PyConnext.hpp"
#include <rti/core/BuiltinProfiles.hpp>

namespace rti { namespace core { namespace builtin_profiles { namespace qos_lib_exp{ }}}}

using namespace rti::core::builtin_profiles::qos_lib;
using namespace rti::core::builtin_profiles::qos_lib_exp;

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyBuiltinProfiles>& cls) {
    cls
        .def_property_readonly_static(
            "library_name",
            [](py::object&) {
                return rti::core::builtin_profiles::qos_lib::library_name();
            },
            "The builtin QoS library name."
        )
        .def_property_readonly_static(
            "baseline",
            [](py::object&) {
                return baseline();
            },
            "The builtin QoS baseline profile name."
        )
        .def_property_readonly_static(
            "baseline_5_0_0",
            [](py::object&) {
                return baseline_5_0_0();
            },
            "The builtin QoS baseline version 5.0.0 profile name."
        )
        .def_property_readonly_static(
            "baseline_5_1_0",
            [](py::object&) {
                return baseline_5_1_0();
            },
            "The builtin QoS baseline version 5.1.0 profile name."
        )
        .def_property_readonly_static(
            "baseline_5_2_0",
            [](py::object&) {
                return baseline_5_2_0();
            },
            "The builtin QoS baseline version 5.2.0 profile name."
        )
        .def_property_readonly_static(
            "baseline_5_3_0",
            [](py::object&) {
                return baseline_5_3_0();
            },
            "The builtin QoS baseline version 5.3.0 profile name."
        )
        .def_property_readonly_static(
            "baseline_6_0_0",
            [](py::object&) {
                return baseline_6_0_0();
            },
            "The builtin QoS baseline version 6.0.0 profile name."
        )
        .def_property_readonly_static(
            "generic_common",
            [](py::object&) {
                return generic_common();
            },
            "The profile name for a generic common participant."
        )
        .def_property_readonly_static(
            "generic_monitoring_common",
            [](py::object&) {
                return generic_monitoring_common();
            },
            "The profile name for a generic common participant with "
            "monitoring enabled."
        )
        .def_property_readonly_static(
            "generic_connext_micro_compatibility",
            [](py::object&) {
                return generic_connext_micro_compatibility();
            },
            "The profile name for compatibility with Connext Micro."
        )
        .def_property_readonly_static(
            "generic_connext_micro_compatibility_2_4_9",
            [](py::object&) {
                return generic_connext_micro_compatibility_2_4_9();
            },
            "The profile name for compatibility with Connext Micro v2.4.9."
        )
        .def_property_readonly_static(
            "generic_connext_micro_compatibility_2_4_3",
            [](py::object&) {
                return generic_connext_micro_compatibility_2_4_3();
            },
            "The profile name for compatibility with Connext Micro v2.4.3."
        )
        .def_property_readonly_static(
            "generic_other_dds_vendor_compatibility",
            [](py::object&) {
                return generic_other_dds_vendor_compatibility();
            },
            "The profile name forcompatibility with other vendor DDS products."
        )
        .def_property_readonly_static(
            "generic_other_dds_vendor_compatibility",
            [](py::object&) {
                return generic_510_transport_compatibility();
            },
            "The profile name for compatibility SHMEM and UDPv6 transports in "
            "v5.1.0."
        )
        .def_property_readonly_static(
            "generic_security",
            [](py::object&) {
                return generic_security();
            },
            "The profile name for loading DDS Secure plugins."
        )
        .def_property_readonly_static(
            "generic_strict_reliable",
            [](py::object&) {
                return generic_strict_reliable();
            },
            "The profile name for generic strict reliable communication."
        )
        .def_property_readonly_static(
            "generic_keep_last_reliable",
            [](py::object&) {
                return generic_keep_last_reliable();
            },
            "The profile name for generic keep-last reliable communication."
        )
        .def_property_readonly_static(
            "generic_best_effort",
            [](py::object&) {
                return generic_best_effort();
            },
            "The profile name for generic best-effort communication."
        )
        .def_property_readonly_static(
            "generic_strict_reliable_high_throughput",
            [](py::object&) {
                return generic_strict_reliable_high_throughput();
            },
            "The profile name for reliable high-throughput communication."
        )
        .def_property_readonly_static(
            "generic_strict_reliable_low_latency",
            [](py::object&) {
                return generic_strict_reliable_low_latency();
            },
            "The profile name for reliable low-latency communication."
        )
        .def_property_readonly_static(
            "generic_participant_large_data",
            [](py::object&) {
                return generic_participant_large_data();
            },
            "The profile name for particpants sending large data."
        )
        .def_property_readonly_static(
            "generic_participant_large_data_monitoring",
            [](py::object&) {
                return generic_participant_large_data_monitoring();
            },
            "The profile name for particpants sending large data with "
            "monitoring."
        )
        .def_property_readonly_static(
            "generic_strict_reliable_large_data",
            [](py::object&) {
                return generic_strict_reliable_large_data();
            },
            "The profile name for sending large data with strict reliability."
        )
        .def_property_readonly_static(
            "generic_keep_last_reliable_large_data",
            [](py::object&) {
                return generic_keep_last_reliable_large_data();
            },
            "The profile name for sending keep-last reliable large data."
        )
        .def_property_readonly_static(
            "generic_strict_reliable_large_data_fast_flow",
            [](py::object&) {
                return generic_strict_reliable_large_data_fast_flow();
            },
            "The profile name for sending large data with strict reliability "
            "and fast flow."
        )
        .def_property_readonly_static(
            "generic_strict_reliable_large_data_medium_flow",
            [](py::object&) {
                return generic_strict_reliable_large_data_medium_flow();
            },
            "The profile name for sending large data with strict reliability "
            "and medium flow."
        )
        .def_property_readonly_static(
            "generic_strict_reliable_large_data_slow_flow",
            [](py::object&) {
                return generic_strict_reliable_large_data_slow_flow();
            },
            "The profile name for sending large data with strict reliability "
            "and slow flow."
        )
        .def_property_readonly_static(
            "generic_keep_last_reliable_large_data_fast_flow",
            [](py::object&) {
                return generic_keep_last_reliable_large_data_fast_flow();
            },
            "The profile name for sending keep-last large reliable data "
            "with fast flow."
        )
        .def_property_readonly_static(
            "generic_keep_last_reliable_large_data_medium_flow",
            [](py::object&) {
                return generic_keep_last_reliable_large_data_medium_flow();
            },
            "The profile name for sending keep-last large reliable data "
            "with medium flow."
        )
        .def_property_readonly_static(
            "generic_keep_last_reliable_large_data_slow_flow",
            [](py::object&) {
                return generic_keep_last_reliable_large_data_slow_flow();
            },
            "The profile name for sending keep-last large reliable data "
            "with slow flow."
        )
        .def_property_readonly_static(
            "generic_keep_last_reliable_transient_local",
            [](py::object&) {
                return generic_keep_last_reliable_transient_local();
            },
            "The profile name for keeping data that persists as long as the "
            "DataWriter is alive."
        )
        .def_property_readonly_static(
            "generic_keep_last_reliable_transient",
            [](py::object&) {
                return generic_keep_last_reliable_transient();
            },
            "The profile name for using Persistence Service."
        )
        .def_property_readonly_static(
            "generic_keep_last_reliable_persistent",
            [](py::object&) {
                return generic_keep_last_reliable_persistent();
            },
            "The profile name for using Persistence Service to persist data "
            "to disk."
        )
        .def_property_readonly_static(
            "generic_auto_tuning",
            [](py::object&) {
                return generic_auto_tuning();
            },
            "The profile name that enables Turbo Mode batching and auto "
            "throttle experimental features."
        )
        .def_property_readonly_static(
            "generic_minimal_memory_footprint",
            [](py::object&) {
                return generic_minimal_memory_footprint();
            },
            "The profile name minimizes memory footprint."
        )
        .def_property_readonly_static(
            "pattern_periodic_data",
            [](py::object&) {
                return pattern_periodic_data();
            },
            "The profile name for a periodic communication pattern."
        )
        .def_property_readonly_static(
            "pattern_streaming",
            [](py::object&) {
                return pattern_streaming();
            },
            "The profile name for a streaming communication pattern."
        )
        .def_property_readonly_static(
            "pattern_reliable_streaming",
            [](py::object&) {
                return pattern_reliable_streaming();
            },
            "The profile name for a reliable streaming communication pattern."
        )
        .def_property_readonly_static(
            "pattern_event",
            [](py::object&) {
                return pattern_event();
            },
            "The profile name for a pattern modeling event communication."
        )
        .def_property_readonly_static(
            "pattern_alarm_event",
            [](py::object&) {
                return pattern_alarm_event();
            },
            "The profile name for a pattern modeling alarm event "
            "communication."
        )
        .def_property_readonly_static(
            "pattern_status",
            [](py::object&) {
                return pattern_status();
            },
            "The profile name for a pattern modeling status communication."
        )
        .def_property_readonly_static(
            "pattern_alarm_status",
            [](py::object&) {
                return pattern_alarm_status();
            },
            "The profile name for a pattern modeling alarm status "
            "communication."
        )
        .def_property_readonly_static(
            "pattern_last_value_cache",
            [](py::object&) {
                return pattern_last_value_cache();
            },
            "The profile name for a pattern modeling a last value cache."
        );
}

template<>
void pyrti::process_inits<pyrti::PyBuiltinProfiles>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyBuiltinProfiles>(m, "BuiltinProfiles");
        }
    );
}