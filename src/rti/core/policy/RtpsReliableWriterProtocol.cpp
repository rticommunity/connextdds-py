#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

template<>
void pyrti::init_class_defs(py::class_<RtpsReliableWriterProtocol>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a default RtpsReliableWriterProtocol policy."
        )
        .def_property(
            "low_watermark",
            (int32_t (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::low_watermark,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(int32_t)) &RtpsReliableWriterProtocol::low_watermark,
            "When the number of unacknowledged samples in the current send "
            "window of a reliable writer meets or falls below this threshold, "
            "the StatusMask.reliable_writer_cache_changed is considered to "
            "have changed."
        )
        .def_property(
            "high_watermark",
            (int32_t (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::high_watermark,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(int32_t)) &RtpsReliableWriterProtocol::high_watermark,
            "When the number of unacknowledged samples in the current send "
            "window of a reliable writer meets or exceeds this threshold, "
            "StatusMask.reliable_writer_cache_changed is considered to have "
            "changed."
        )
        .def_property(
            "heartbeat_period",
            (dds::core::Duration (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::heartbeat_period,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(const dds::core::Duration&)) &RtpsReliableWriterProtocol::heartbeat_period,
            "The period at which to send heartbeats."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "fast_heartbeat_period",
            (dds::core::Duration (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::fast_heartbeat_period,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(const dds::core::Duration&)) &RtpsReliableWriterProtocol::fast_heartbeat_period,
            "An alternative heartbeat period used when a reliable writer "
            "needs to flush its unacknowledged samples more quickly."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "late_joiner_heartbeat_period",
            (dds::core::Duration (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::late_joiner_heartbeat_period,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(const dds::core::Duration&)) &RtpsReliableWriterProtocol::late_joiner_heartbeat_period,
            "An alternative heartbeat period used when a reliable reader "
            "joins late and needs to be caught up on cached samples of a "
            "reliable writer more quickly than the normal heartbeat rate."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "virtual_heartbeat_period",
            (dds::core::Duration (RtpsReliableWriterProtocol::*)()) &RtpsReliableWriterProtocol::virtual_heartbeat_period,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(const dds::core::Duration&)) &RtpsReliableWriterProtocol::virtual_heartbeat_period,
            "The period at which to send virtual heartbeats. Virtual "
            "heartbeats inform the reliable reader about the range of samples "
            "currently present, for each virtual GUID, in the reliable "
            "writer's queue."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "samples_per_virtual_heartbeat",
            (int32_t (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::samples_per_virtual_heartbeat,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(int32_t)) &RtpsReliableWriterProtocol::samples_per_virtual_heartbeat,
            "The number of samples that a reliable writer has to publish "
            "before sending a virtual heartbeat."
        )
        .def_property(
            "max_heartbeat_retries",
            (int32_t (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::max_heartbeat_retries,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(int32_t)) &RtpsReliableWriterProtocol::max_heartbeat_retries,
            "The maximum number of periodic heartbeat retries before marking "
            "a remote reader as inactive."
        )
        .def_property(
            "inactivate_nonprogressing_readers",
            (bool (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::inactivate_nonprogressing_readers,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(bool)) &RtpsReliableWriterProtocol::inactivate_nonprogressing_readers,
            "Whether to treat remote readers as inactive when their NACKs do "
            "not progress."
        )
        .def_property(
            "heartbeats_per_max_samples",
            (int32_t (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::heartbeats_per_max_samples,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(int32_t)) &RtpsReliableWriterProtocol::heartbeats_per_max_samples,
            "The number of heartbeats per current send window."
        )
        .def_property(
            "min_nack_response_delay",
            (dds::core::Duration (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::min_nack_response_delay,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(const dds::core::Duration&)) &RtpsReliableWriterProtocol::min_nack_response_delay,
            "The minimum delay to respond to a NACK."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "max_nack_response_delay",
            (dds::core::Duration (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::max_nack_response_delay,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(const dds::core::Duration&)) &RtpsReliableWriterProtocol::max_nack_response_delay,
            "The maximum delay to respond to a NACK."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "nack_suppression_duration",
            (dds::core::Duration (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::nack_suppression_duration,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(const dds::core::Duration&)) &RtpsReliableWriterProtocol::nack_suppression_duration,
            "The duration for ignoring consecutive NACKs that may trigger "
            "redundant repairs."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "max_bytes_per_nack_response",
            (int32_t (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::max_bytes_per_nack_response,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(int32_t)) &RtpsReliableWriterProtocol::max_bytes_per_nack_response,
            "The maximum total message size when resending dropped samples."
        )
        .def_property(
            "disable_positive_acks_min_sample_keep_duration",
            (dds::core::Duration (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::disable_positive_acks_min_sample_keep_duration,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(const dds::core::Duration&)) &RtpsReliableWriterProtocol::disable_positive_acks_min_sample_keep_duration,
            "The minimum duration a sample is queued for ACK-disabled readers."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "disable_positive_acks_max_sample_keep_duration",
            (dds::core::Duration (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::disable_positive_acks_max_sample_keep_duration,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(const dds::core::Duration&)) &RtpsReliableWriterProtocol::disable_positive_acks_max_sample_keep_duration,
            "The maximum duration a sample is queued for ACK-disabled readers."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "disable_positive_acks_enable_adaptive_sample_keep_duration",
            (bool (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::disable_positive_acks_enable_adaptive_sample_keep_duration,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(bool)) &RtpsReliableWriterProtocol::disable_positive_acks_enable_adaptive_sample_keep_duration,
            "Enables dynamic adjustment of sample keep duration in response "
            "to congestion."
        )
        .def_property(
            "disable_positive_acks_decrease_sample_keep_duration_factor",
            (int32_t (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::disable_positive_acks_decrease_sample_keep_duration_factor,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(int32_t)) &RtpsReliableWriterProtocol::disable_positive_acks_decrease_sample_keep_duration_factor,
            "Controls rate of contraction of dynamic sample keep duration."
        )
        .def_property(
            "disable_positive_acks_increase_sample_keep_duration_factor",
            (int32_t (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::disable_positive_acks_increase_sample_keep_duration_factor,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(int32_t)) &RtpsReliableWriterProtocol::disable_positive_acks_increase_sample_keep_duration_factor,
            "Controls rate of growth of dynamic sample keep duration."
        )
        .def_property(
            "min_send_window_size",
            (int32_t (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::min_send_window_size,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(int32_t)) &RtpsReliableWriterProtocol::min_send_window_size,
            "Minimum size of send window of unacknowledged samples."
        )
        .def_property(
            "max_send_window_size",
            (int32_t (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::max_send_window_size,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(int32_t)) &RtpsReliableWriterProtocol::max_send_window_size,
            "Maximum size of send window of unacknowledged samples."
        )
        .def_property(
            "send_window_update_period",
            (dds::core::Duration (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::send_window_update_period,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(const dds::core::Duration&)) &RtpsReliableWriterProtocol::send_window_update_period,
            "Period in which send window may be dynamically changed."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "send_window_increase_factor",
            (int32_t (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::send_window_increase_factor,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(int32_t)) &RtpsReliableWriterProtocol::send_window_increase_factor,
            "Increases send window size by this percentage when reacting "
            "dynamically to network conditions."
        )
        .def_property(
            "send_window_decrease_factor",
            (int32_t (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::send_window_decrease_factor,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(int32_t)) &RtpsReliableWriterProtocol::send_window_decrease_factor,
            "Decreases send window size by this percentage when reacting "
            "dynamically to network conditions."
        )
        .def_property(
            "enable_multicast_periodic_heartbeat",
            (bool (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::enable_multicast_periodic_heartbeat,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(bool)) &RtpsReliableWriterProtocol::enable_multicast_periodic_heartbeat,
            "Whether periodic heartbeat messages are sent over multicast."
        )
        .def_property(
            "multicast_resend_threshold",
            (int32_t (RtpsReliableWriterProtocol::*)() const) &RtpsReliableWriterProtocol::multicast_resend_threshold,
            (RtpsReliableWriterProtocol& (RtpsReliableWriterProtocol::*)(int32_t)) &RtpsReliableWriterProtocol::multicast_resend_threshold,
            "The minimum number of requesting readers needed to trigger a "
            "multicast resend."
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

template<>
void pyrti::process_inits<RtpsReliableWriterProtocol>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<RtpsReliableWriterProtocol>(m, "RtpsReliableWriterProtocol");
        }
    );
}
