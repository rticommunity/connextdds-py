#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>
#include "PySafeEnum.hpp"
#include "PyMaskType.hpp"

using namespace rti::core;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<DiscoveryConfigBuiltinPluginKindMask>& cls) {
    cls
        .def_static(
            "all",
            &DiscoveryConfigBuiltinPluginKindMask::all,
            "Create a mask with all bits set."
        )
        .def_static(
            "none",
            &DiscoveryConfigBuiltinPluginKindMask::none,
            "Create a mask with no bits set."
        )
        .def_static(
            "sdp",
            &DiscoveryConfigBuiltinPluginKindMask::SDP,
            "Create a mask that selects the Simple Discovery Protocol (SDP)."
        )
        .def_static(
            "spdp",
            &DiscoveryConfigBuiltinPluginKindMask::SPDP
        )
        .def_static(
            "sedp",
            &DiscoveryConfigBuiltinPluginKindMask::SEDP
        );
}

template<>
void init_class_defs(py::class_<DiscoveryConfig>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates the default policy."
        )
        .def_property(
            "participant_liveliness_lease_duration",
            (dds::core::Duration (DiscoveryConfig::*)() const) &DiscoveryConfig::participant_liveliness_lease_duration,
            (DiscoveryConfig& (DiscoveryConfig::*)(const dds::core::Duration&)) &DiscoveryConfig::participant_liveliness_lease_duration,
            "The liveliness lease duration for the participant."
        )
        .def_property(
            "participant_liveliness_assert_period",
            (dds::core::Duration (DiscoveryConfig::*)() const) &DiscoveryConfig::participant_liveliness_assert_period,
            (DiscoveryConfig& (DiscoveryConfig::*)(const dds::core::Duration&)) &DiscoveryConfig::participant_liveliness_assert_period,
            "The period to assert liveliness for the participant."
            "\n\n"
            "This property's getter returns a deep copy."
        )
        .def_property(
            "remote_participant_purge_kind",
            (RemoteParticipantPurgeKind (DiscoveryConfig::*)() const) &DiscoveryConfig::remote_participant_purge_kind,
            (DiscoveryConfig& (DiscoveryConfig::*)(RemoteParticipantPurgeKind)) &DiscoveryConfig::remote_participant_purge_kind,
            "The participant's behavior for maintaining knowledge of remote "
            "participants (and their contained entities) with which discovery "
            "communication has been lost."
        )
        .def_property(
            "max_liveliness_loss_detection_period",
            (dds::core::Duration (DiscoveryConfig::*)() const) &DiscoveryConfig::max_liveliness_loss_detection_period,
            (DiscoveryConfig& (DiscoveryConfig::*)(const dds::core::Duration&)) &DiscoveryConfig::max_liveliness_loss_detection_period,
            "The period to assert liveliness for the participant."
            "\n\n"
            "This property's getter returns a deep copy."
        )
        .def_property(
            "initial_participant_announcements",
            (int32_t (DiscoveryConfig::*)() const) &DiscoveryConfig::initial_participant_announcements,
            (DiscoveryConfig& (DiscoveryConfig::*)(int32_t)) &DiscoveryConfig::initial_participant_announcements,
            "The number of initial announcements sent when a participant is "
            "first enabled or when a remote participant is newly discovered."
        )
        .def_property(
            "min_initial_participant_announcement_period",
            (dds::core::Duration (DiscoveryConfig::*)() const) &DiscoveryConfig::min_initial_participant_announcement_period,
            (DiscoveryConfig& (DiscoveryConfig::*)(const dds::core::Duration&)) &DiscoveryConfig::min_initial_participant_announcement_period,
            "The minimum period between initial announcements when a "
            "participant is first enabled or when a remote participant is "
            "newly discovered."
            "\n\n"
            "This property's getter returns a deep copy."
        )
        .def_property(
            "max_initial_participant_announcement_period",
            (dds::core::Duration (DiscoveryConfig::*)() const) &DiscoveryConfig::max_initial_participant_announcement_period,
            (DiscoveryConfig& (DiscoveryConfig::*)(const dds::core::Duration&)) &DiscoveryConfig::max_initial_participant_announcement_period,
            "The maximum period between initial announcements when a "
            "participant is first enabled or when a remote participant is "
            "newly discovered."
        )
        .def_property(
            "participant_reader_resource_limits",
            (BuiltinTopicReaderResourceLimits& (DiscoveryConfig::*)()) &DiscoveryConfig::participant_reader_resource_limits,
            (DiscoveryConfig& (DiscoveryConfig::*)(const BuiltinTopicReaderResourceLimits&)) &DiscoveryConfig::participant_reader_resource_limits,
            "Resource limits."
        )
        .def_property(
            "publication_reader",
            (RtpsReliableReaderProtocol& (DiscoveryConfig::*)()) &DiscoveryConfig::publication_reader,
            (DiscoveryConfig& (DiscoveryConfig::*)(const RtpsReliableReaderProtocol&)) &DiscoveryConfig::publication_reader,
            "RTPS protocol-related configuration settings for the RTPS "
            "reliable reader associated to a built-in publication reader."
        )
        .def_property(
            "publication_reader_resource_limits",
            (BuiltinTopicReaderResourceLimits& (DiscoveryConfig::*)()) &DiscoveryConfig::publication_reader_resource_limits,
            (DiscoveryConfig& (DiscoveryConfig::*)(const BuiltinTopicReaderResourceLimits&)) &DiscoveryConfig::publication_reader_resource_limits,
            "Publication reader resource limits."
        )
        .def_property(
            "subscription_reader",
            (RtpsReliableReaderProtocol& (DiscoveryConfig::*)()) &DiscoveryConfig::subscription_reader,
            (DiscoveryConfig& (DiscoveryConfig::*)(const RtpsReliableReaderProtocol&)) &DiscoveryConfig::subscription_reader,
            "RTPS protocol-related configuration settings for the RTPS "
            "reliable reader associated to a built-in subscription reader."
        )
        .def_property(
            "subscription_reader_resource_limits",
            (BuiltinTopicReaderResourceLimits& (DiscoveryConfig::*)()) &DiscoveryConfig::subscription_reader_resource_limits,
            (DiscoveryConfig& (DiscoveryConfig::*)(const BuiltinTopicReaderResourceLimits&)) &DiscoveryConfig::subscription_reader_resource_limits,
            "Subscription reader resource limits."
        )
        .def_property(
            "publication_writer",
            (RtpsReliableWriterProtocol& (DiscoveryConfig::*)()) &DiscoveryConfig::publication_writer,
            (DiscoveryConfig& (DiscoveryConfig::*)(const RtpsReliableWriterProtocol&)) &DiscoveryConfig::publication_writer,
            "RTPS protocol-related configuration settings for the RTPS "
            "reliable writer associated to a built-in publication reader."
        )
        .def_property(
            "publication_writer_data_lifecycle",
            (dds::core::policy::WriterDataLifecycle& (DiscoveryConfig::*)()) &DiscoveryConfig::publication_writer_data_lifecycle,
            (DiscoveryConfig& (DiscoveryConfig::*)(const dds::core::policy::WriterDataLifecycle&)) &DiscoveryConfig::publication_writer_data_lifecycle,
            "riter data lifecycle settings for a built-in publication writer."
        )
        .def_property(
            "subscription_writer",
            (RtpsReliableWriterProtocol& (DiscoveryConfig::*)()) &DiscoveryConfig::subscription_writer,
            (DiscoveryConfig& (DiscoveryConfig::*)(const RtpsReliableWriterProtocol&)) &DiscoveryConfig::subscription_writer,
            "RTPS protocol-related configuration settings for the RTPS "
            "reliable writer associated to a built-in subscription reader."
        )
        .def_property(
            "subscription_writer_data_lifecycle",
            (dds::core::policy::WriterDataLifecycle& (DiscoveryConfig::*)()) &DiscoveryConfig::subscription_writer_data_lifecycle,
            (DiscoveryConfig& (DiscoveryConfig::*)(const dds::core::policy::WriterDataLifecycle&)) &DiscoveryConfig::subscription_writer_data_lifecycle,
            "riter data lifecycle settings for a built-in subscription writer."
        )
        .def_property(
            "builtin_discovery_plugins",
            (DiscoveryConfigBuiltinPluginKindMask (DiscoveryConfig::*)() const) &DiscoveryConfig::builtin_discovery_plugins,
            (DiscoveryConfig& (DiscoveryConfig::*)(const DiscoveryConfigBuiltinPluginKindMask&)) &DiscoveryConfig::builtin_discovery_plugins,
            "The kind mask for built-in discovery plugins."
        )
        .def_property(
            "participant_message_reader_reliability_kind",
            (dds::core::policy::ReliabilityKind (DiscoveryConfig::*)() const) &DiscoveryConfig::participant_message_reader_reliability_kind,
            (DiscoveryConfig& (DiscoveryConfig::*)(dds::core::policy::ReliabilityKind)) &DiscoveryConfig::participant_message_reader_reliability_kind,
            "Reliability policy for a built-in participant message reader."
        )
        .def_property(
            "participant_message_reader",
            (RtpsReliableReaderProtocol& (DiscoveryConfig::*)()) &DiscoveryConfig::participant_message_reader,
            (DiscoveryConfig& (DiscoveryConfig::*)(const RtpsReliableReaderProtocol&)) &DiscoveryConfig::participant_message_reader,
            "Reliability policy for a built-in participant message reader."
        )
        .def_property(
            "participant_message_writer",
            (RtpsReliableWriterProtocol& (DiscoveryConfig::*)()) &DiscoveryConfig::participant_message_writer,
            (DiscoveryConfig& (DiscoveryConfig::*)(const RtpsReliableWriterProtocol&)) &DiscoveryConfig::participant_message_writer,
            "Reliability policy for a built-in participant message writer."
        )
        .def_property(
            "publication_writer_publish_mode",
            (PublishMode& (DiscoveryConfig::*)()) &DiscoveryConfig::publication_writer_publish_mode,
            (DiscoveryConfig& (DiscoveryConfig::*)(const PublishMode&)) &DiscoveryConfig::publication_writer_publish_mode,
            "Publish mode policy for the built-in publication writer."
        )
        .def_property(
            "subscription_writer_publish_mode",
            (PublishMode& (DiscoveryConfig::*)()) &DiscoveryConfig::subscription_writer_publish_mode,
            (DiscoveryConfig& (DiscoveryConfig::*)(const PublishMode&)) &DiscoveryConfig::subscription_writer_publish_mode,
            "Publish mode policy for the built-in subscription writer."
        )
        .def_property(
            "asynchronous_publisher",
            (AsynchronousPublisher& (DiscoveryConfig::*)()) &DiscoveryConfig::asynchronous_publisher,
            (DiscoveryConfig& (DiscoveryConfig::*)(const AsynchronousPublisher&)) &DiscoveryConfig::asynchronous_publisher,
            "Asynchronous publishing settings for the discovery Publisher and "
            "all entities that are created by it."
        )
        .def_property(
            "default_domain_announcement_period",
            (dds::core::Duration (DiscoveryConfig::*)() const) &DiscoveryConfig::default_domain_announcement_period,
            (DiscoveryConfig& (DiscoveryConfig::*)(const dds::core::Duration&)) &DiscoveryConfig::default_domain_announcement_period,
            "The period to announce a participant to the default domain 0."
            "\n\n"
            "This property's getter returns a deep copy."
        )
        .def_property(
            "ignore_default_domain_announcements",
            (bool (DiscoveryConfig::*)() const) &DiscoveryConfig::ignore_default_domain_announcements,
            (DiscoveryConfig& (DiscoveryConfig::*)(bool)) &DiscoveryConfig::ignore_default_domain_announcements,
            "Used to ignore the announcements received by a participant on "
            "the default domain 0 corresponding to participants running on "
            "domains IDs other than 0."
        )
        .def_property(
            "service_request_writer",
            (RtpsReliableWriterProtocol& (DiscoveryConfig::*)()) &DiscoveryConfig::service_request_writer,
            (DiscoveryConfig& (DiscoveryConfig::*)(const RtpsReliableWriterProtocol&)) &DiscoveryConfig::service_request_writer,
            "RTPS protocol-related configuration settings for the RTPS "
            "reliable writer associated to the built-in ServiceRequest "
            "writer."
        )
        .def_property(
            "service_request_writer_data_lifecycle",
            (dds::core::policy::WriterDataLifecycle& (DiscoveryConfig::*)()) &DiscoveryConfig::service_request_writer_data_lifecycle,
            (DiscoveryConfig& (DiscoveryConfig::*)(const dds::core::policy::WriterDataLifecycle&)) &DiscoveryConfig::service_request_writer_data_lifecycle,
            "Writer data lifecycle settings for a built-in ServiceRequest "
            "writer."
        )
        .def_property(
            "service_request_writer_publish_mode",
            (PublishMode& (DiscoveryConfig::*)()) &DiscoveryConfig::service_request_writer_publish_mode,
            (DiscoveryConfig& (DiscoveryConfig::*)(const PublishMode&)) &DiscoveryConfig::service_request_writer_publish_mode,
            "Publish mode policy for the built-in service request writer."
        )
        .def_property(
            "service_request_reader",
            (RtpsReliableReaderProtocol& (DiscoveryConfig::*)()) &DiscoveryConfig::service_request_reader,
            (DiscoveryConfig& (DiscoveryConfig::*)(const RtpsReliableReaderProtocol&)) &DiscoveryConfig::service_request_reader,
            "RTPS protocol-related configuration settings for the RTPS "
            "reliable reader associated to a built-in publication reader."
        )
        .def_property(
            "locator_reachability_assert_period",
            (dds::core::Duration (DiscoveryConfig::*)() const) &DiscoveryConfig::locator_reachability_assert_period,
            (DiscoveryConfig& (DiscoveryConfig::*)(const dds::core::Duration&)) &DiscoveryConfig::locator_reachability_assert_period,
            "Period at which this DomainParticipant will assert the locators "
            "discovered from other DomainParticipants."
            "\n\n"
            "This property's getter returns a deep copy."
        )
        .def_property(
            "locator_reachability_lease_duration",
            (dds::core::Duration (DiscoveryConfig::*)() const) &DiscoveryConfig::locator_reachability_lease_duration,
            (DiscoveryConfig& (DiscoveryConfig::*)(const dds::core::Duration&)) &DiscoveryConfig::locator_reachability_lease_duration,
            "The time period after which other DomainParticipants can "
            "consider one of their locators as \"unreachable\" if they do not "
            "receive a REACHABILITY PING from this DomainParticipant."
            "\n\n"
            "This property's getter returns a deep copy."
        )
        .def_property(
            "locator_reachability_change_detection_period",
            (dds::core::Duration (DiscoveryConfig::*)() const) &DiscoveryConfig::locator_reachability_change_detection_period,
            (DiscoveryConfig& (DiscoveryConfig::*)(const dds::core::Duration&)) &DiscoveryConfig::locator_reachability_change_detection_period,
            "Period at which this DomainParticipant will check if its "
            "locators are reachable from other DomainParticipants."
            "\n\n"
            "This property's getter returns a deep copy."
        )
#if rti_connext_version_gte(6, 0, 0)
        .def_property(
            "secure_volatile_writer_publish_mode",
            (PublishMode& (DiscoveryConfig::*)()) &DiscoveryConfig::secure_volatile_writer_publish_mode,
            (DiscoveryConfig& (DiscoveryConfig::*)(const PublishMode&)) &DiscoveryConfig::secure_volatile_writer_publish_mode,
            "Publish mode policy for the built-in secure volatile writer."
        )
        .def_property(
            "endpoint_type_object_lb_serialization_threshold",
            (int32_t (DiscoveryConfig::*)() const) &DiscoveryConfig::endpoint_type_object_lb_serialization_threshold,
            (DiscoveryConfig& (DiscoveryConfig::*)(int32_t)) &DiscoveryConfig::endpoint_type_object_lb_serialization_threshold,
            "Option to reduce the size required to propagate a TypeObject "
            "in Simple Endpoint Discovery."
        )
#endif
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
void process_inits<DiscoveryConfig>(py::module& m, ClassInitList& l) {
    auto rppk = init_dds_safe_enum<RemoteParticipantPurgeKind_def>(m, "RemoteParticipantPurgeKind");

    py::enum_<RemoteParticipantPurgeKind::type>(rppk, "RemoteParticipantPurgeKind")
        .value(
            "LIVELINESS_BASED",
            RemoteParticipantPurgeKind::type::LIVELINESS_BASED,
            "[default] Maintain knowledge of the remote participant for as "
            "long as it maintains its liveliness contract."
            "\n\n"
            "A participant will continue attempting communication with its "
            "peers, even if discovery communication with them is lost, as "
            "long as the remote participants maintain their liveliness. If "
            "both discovery communication and participant liveliness are "
            "lost, however, the local participant will remove all records of "
            "the remote participant and its contained endpoints, and no "
            "further data communication with them will occur until and unless "
            "they are rediscovered."
            "\n\n"
            "The liveliness contract a participant promises to its peers "
            "(its \"liveliness lease duration\") is specified in its "
            "DiscoveryConfig.participant_liveliness_lease_duration QoS field. "
            "It maintains that contract by writing data to those other "
            "participants with a writer that has a LivelinessKind of "
            "LivelinessKind.AUTOMATIC or LivelinessKind.MANUAL_BY_PARTICIPANT "
            "and by asserting itself (at the "
            "DiscoveryConfig.participant_liveliness_assert_period) over the "
            "Simple Discovery Protocol."
        )
        .value(
            "NO_PURGE",
            RemoteParticipantPurgeKind::type::NO_PURGE,
            "Never \"forget\" a remote participant with which discovery "
            "communication has been lost."
            "\n\n"
            "If a participant with this behavior loses discovery communication "
            "with a remote participant, it will nevertheless remember that "
            "remote participant and its endpoints and continue attempting to "
            "communicate with them indefinitely."
            "\n\n"
            "This value has consequences for a participant's resource usage. "
            "If discovery communication with a remote participant is lost, "
            "but the same participant is later rediscovered, any relevant "
            "records that remain in the database will be reused. However, if "
            "it is not rediscovered, the records will continue to take up "
            "space in the database for as long as the local participant "
            "remains in existence."
        )
        .export_values();
        
    l.push_back(
        [m]() mutable {
            auto cls = init_mask_type<DiscoveryConfigBuiltinPluginKindMask, uint64_t>(m, "DiscoveryConfigBuiltinPluginKindMask", "Create a DiscoveryConfigBuiltinPluginKindMask equivalent to DiscoveryConfigBuiltinPluginKindMask.none()");
            return [cls]() mutable {
                init_class_defs<DiscoveryConfigBuiltinPluginKindMask>(cls);
            };
        }
    );

    l.push_back(
        [m]() mutable {
            return init_class<DiscoveryConfig>(m, "DiscoveryConfig");
        }
    );
}

}
