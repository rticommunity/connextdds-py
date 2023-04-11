#
# (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

import rti.connextdds as dds
import pytest
import enum


class Entity(enum.Enum):
    DomainParticipantFactory = 1
    DomainParticipant = 2
    Publisher = 3
    Subscriber = 4
    Topic = 5
    DataReader = 6
    DataWriter = 7


def assert_structures_equal(a, b, field_name=""):
    # This helper function is to help find what field exactly is different
    # between two QoS policies
    assert type(a) is type(b)
    for field in dir(a):
        if field.startswith("_"):
            continue
        if type(getattr(a, field)) is dict:
            assert_structures_equal(getattr(a, field), getattr(
                b, field), field_name)
        else:
            if getattr(a, field) != getattr(b, field):
                if "rti.connextdds." in str(type(getattr(a, field))):
                    assert_structures_equal(getattr(a, field), getattr(
                        b, field), field_name + " " + field)
                else:
                    assert False, "Field \'{}\' is different".format(
                        field_name + " " + field)


# This is a list of all of the fields in a policy that are static
# Unfortunately, this is a limitation of the pybind11 objects since they cannot
# be introspected using the same libraries as native python objects
# HOWEVER, we will assume all fields that are in all caps are constant and
# ignore them so they do not need to be added to the list
ignored_fields = [
    (dds.AsynchronousPublisher, "disabled"),
    (dds.AsynchronousPublisher, "enabled"),
    (dds.Batch, "disabled"),
    (dds.Batch, "enabled"),
    (dds.Batch, "enabled_with_max_data_bytes"),
    (dds.Batch, "enabled_with_max_samples"),
    (dds.DataReaderProtocol, "virtual_guid"),
    (dds.DataWriterProtocol, "virtual_guid"),
    (dds.Durability, "persistent"),
    (dds.Durability, "transient"),
    (dds.Durability, "transient_local"),
    (dds.Durability, "volatile"),
    (dds.EntityFactory, "auto_enable"),
    (dds.EntityFactory, "manually_enable"),
    (dds.History, "keep_all"),
    (dds.History, "keep_last"),
    (dds.Liveliness, "automatic"),
    (dds.Liveliness, "manual_by_participant"),
    (dds.Liveliness, "manual_by_topic"),
    (dds.Ownership, "exclusive"),
    (dds.Ownership, "shared"),
    (dds.Presentation, "group_access_scope"),
    (dds.Presentation, "instance_access_scope"),
    (dds.Presentation, "topic_access_scope"),
    (dds.PublishMode, "asynchronous"),
    (dds.PublishMode, "synchronous"),
    (dds.ReaderDataLifecycle, "auto_purge_disposed_samples"),
    (dds.ReaderDataLifecycle, "auto_purge_no_writer_samples"),
    (dds.ReaderDataLifecycle, "no_auto_purge"),
    (dds.Reliability, "best_effort"),
    (dds.Reliability, "reliable"),
    (dds.TransportBuiltin, "all"),
    (dds.TransportBuiltin, "none"),
    (dds.TransportBuiltin, "shmem"),
    (dds.TransportBuiltin, "udpv4"),
    (dds.TransportBuiltin, "udpv6"),
    (dds.TypeConsistencyEnforcement, "allow_type_coercion"),
    (dds.TypeConsistencyEnforcement, "auto_type_coercion"),
    (dds.TypeConsistencyEnforcement, "disallow_type_coercion")]


def assert_no_field_equal_in_structures(a, b):
    assert type(a) == type(b)
    for field in dir(a):
        if field.startswith("_") or (type(a), field) in ignored_fields or field.isupper():
            continue
        if getattr(a, field) == getattr(b, field):
            assert False, "Field \'{}\' is equal".format(field)
    return True


class QosPolicy:
    def __init__(self, policy, default_values, non_default_values, field_name):
        self.policy = policy
        self.default_values = default_values
        self.non_default_values = non_default_values
        self.field_name = field_name

    def __str__(self) -> str:
        return self.field_name

    def policy_defined_for_entity(self, entity):
        return entity in self.default_values


def get_default_qos_policies_from_policy(policy):

    if policy is dds.AsynchronousPublisher:
        default_async_pub = dds.AsynchronousPublisher()
        default_async_pub.disable_asynchronous_write = False
        default_async_pub.thread = dds.ThreadSettings()
        default_async_pub.thread.mask = dds.ThreadSettingsKindMask()
        default_async_pub.disable_asynchronous_batch = False
        default_async_pub.asynchronous_batch_thread = dds.ThreadSettings()
        default_async_pub.asynchronous_batch_thread.mask = dds.ThreadSettingsKindMask()
        default_async_pub.disable_topic_query_publication = False
        default_async_pub.topic_query_publication_thread = dds.ThreadSettings()
        default_async_pub.topic_query_publication_thread.mask = dds.ThreadSettingsKindMask()

        non_default_async_pub = dds.AsynchronousPublisher()
        non_default_async_pub.disable_asynchronous_write = True
        non_default_async_pub.thread = dds.ThreadSettings()
        non_default_async_pub.thread.mask = dds.ThreadSettingsKindMask.STDIO
        non_default_async_pub.disable_asynchronous_batch = True
        non_default_async_pub.asynchronous_batch_thread = dds.ThreadSettings()
        non_default_async_pub.asynchronous_batch_thread.mask = dds.ThreadSettingsKindMask.STDIO
        non_default_async_pub.disable_topic_query_publication = True
        non_default_async_pub.topic_query_publication_thread = dds.ThreadSettings()
        non_default_async_pub.topic_query_publication_thread.mask = dds.ThreadSettingsKindMask.STDIO
        return QosPolicy(
            dds.AsynchronousPublisher,
            {Entity.Publisher: default_async_pub},
            {Entity.Publisher: non_default_async_pub},
            "asynchronous_publisher")

    elif policy is dds.Availability:
        default_availability = dds.Availability()
        default_availability.enable_required_subscriptions = False
        default_availability.max_data_availability_waiting_time = dds.Duration.automatic
        default_availability.max_endpoint_availability_waiting_time = dds.Duration.automatic
        default_availability.required_matched_endpoint_groups = []

        v = dds.EndpointGroupVector(1)
        v[0] = dds.EndpointGroup("a", 1)
        non_default_availability = dds.Availability(
            True,
            dds.Duration(1, 0),
            dds.Duration(1, 0),
            v)

        return QosPolicy(
            dds.Availability,
            {Entity.DataWriter: default_availability,
             Entity.DataReader: default_availability},
            {Entity.DataWriter: non_default_availability,
             Entity.DataReader: non_default_availability},
            "availability")

    elif policy is dds.Batch:
        default_batch = dds.Batch()
        default_batch.enable = False
        default_batch.max_data_bytes = 1024
        default_batch.max_samples = dds.LENGTH_UNLIMITED
        default_batch.max_flush_delay = dds.Duration.infinite
        default_batch.source_timestamp_resolution = dds.Duration.infinite
        default_batch.thread_safe_write = True

        non_default_batch = dds.Batch()
        non_default_batch.enable = True
        non_default_batch.max_data_bytes = 2048
        non_default_batch.max_samples = 10
        non_default_batch.max_flush_delay = dds.Duration(1, 0)
        non_default_batch.source_timestamp_resolution = dds.Duration(1, 0)
        non_default_batch.thread_safe_write = False

        return QosPolicy(
            dds.Batch,
            {Entity.DataWriter: default_batch},
            {Entity.DataWriter: non_default_batch},
            "batch")

    elif policy is dds.Database:
        default_database = dds.Database()
        default_database.thread = dds.DomainParticipant.default_participant_qos.database.thread
        default_database.shutdown_timeout = dds.Duration(15, 0)
        default_database.cleanup_period = dds.Duration(61, 0)
        default_database.shutdown_cleanup_period = dds.Duration.from_milliseconds(
            10)
        default_database.initial_records = 1024
        default_database.max_skiplist_level = 7
        default_database.max_weak_references = dds.LENGTH_UNLIMITED
        default_database.initial_weak_references = 2049

        non_default_database = dds.Database()
        non_default_database.thread.mask = dds.ThreadSettings().mask.REALTIME_PRIORITY
        non_default_database.shutdown_timeout = dds.Duration(1, 0)
        non_default_database.cleanup_period = dds.Duration(1, 0)
        non_default_database.shutdown_cleanup_period = dds.Duration(10, 0)
        non_default_database.initial_records = 2048
        non_default_database.max_skiplist_level = 10
        non_default_database.max_weak_references = 10
        non_default_database.initial_weak_references = 10

        return QosPolicy(
            dds.Database,
            {Entity.DomainParticipant: default_database},
            {Entity.DomainParticipant: non_default_database},
            "database")

    elif policy is dds.DataReaderProtocol:
        default_dr_protocol = dds.DataReaderProtocol()
        default_dr_protocol.virtual_guid = dds.Guid.automatic
        default_dr_protocol.rtps_object_id = dds.WireProtocol.RTPS_AUTO_ID
        default_dr_protocol.expects_inline_qos = False
        default_dr_protocol.disable_positive_acks = False
        default_dr_protocol.propagate_dispose_of_unregistered_instances = False
        default_dr_protocol.propagate_unregister_of_disposed_instances = False
        default_rtps_reliable_reader = dds.RtpsReliableReaderProtocol()
        default_rtps_reliable_reader.min_heartbeat_response_delay = dds.Duration.zero
        default_rtps_reliable_reader.max_heartbeat_response_delay = dds.Duration.from_milliseconds(
            500)
        default_rtps_reliable_reader.heartbeat_suppression_duration = dds.Duration.from_microseconds(
            62500)
        default_rtps_reliable_reader.nack_period = dds.Duration(5)
        default_rtps_reliable_reader.receive_window_size = 256
        default_rtps_reliable_reader.round_trip_time = dds.Duration.zero
        default_rtps_reliable_reader.app_ack_period = dds.Duration(5)
        default_rtps_reliable_reader.samples_per_app_ack = 1
        default_dr_protocol.rtps_reliable_reader = default_rtps_reliable_reader

        non_default_dr_protocol = dds.DataReaderProtocol()
        non_default_dr_protocol.virtual_guid = dds.Guid()
        non_default_dr_protocol.rtps_object_id = 1
        non_default_dr_protocol.expects_inline_qos = True
        non_default_dr_protocol.disable_positive_acks = True
        non_default_dr_protocol.propagate_dispose_of_unregistered_instances = True
        non_default_dr_protocol.propagate_unregister_of_disposed_instances = True
        non_default_rtps_reliable_reader = dds.RtpsReliableReaderProtocol()
        non_default_rtps_reliable_reader.min_heartbeat_response_delay = dds.Duration.from_milliseconds(
            1)
        non_default_rtps_reliable_reader.max_heartbeat_response_delay = dds.Duration.from_milliseconds(
            1000)
        non_default_rtps_reliable_reader.heartbeat_suppression_duration = dds.Duration.from_microseconds(
            125000)
        non_default_rtps_reliable_reader.nack_period = dds.Duration(10)
        non_default_rtps_reliable_reader.receive_window_size = 512
        non_default_rtps_reliable_reader.round_trip_time = dds.Duration.from_milliseconds(
            1)
        non_default_rtps_reliable_reader.app_ack_period = dds.Duration(10)
        non_default_rtps_reliable_reader.samples_per_app_ack = 2
        non_default_dr_protocol.rtps_reliable_reader = non_default_rtps_reliable_reader

        return QosPolicy(
            dds.DataReaderProtocol,
            {Entity.DataReader: default_dr_protocol},
            {Entity.DataReader: non_default_dr_protocol},
            "data_reader_protocol")

    elif policy is dds.DataWriterProtocol:
        default_dw_protocol = dds.DataWriterProtocol()
        default_dw_protocol.virtual_guid = dds.Guid.automatic
        default_dw_protocol.rtps_object_id = dds.WireProtocol.RTPS_AUTO_ID
        default_dw_protocol.push_on_write = True
        default_dw_protocol.disable_positive_acks = False
        default_dw_protocol.disable_inline_keyhash = False
        default_dw_protocol.serialize_key_with_dispose = False
        default_dw_protocol.propagate_app_ack_with_no_response = True
        default_rtps_reliable_writer = dds.RtpsReliableWriterProtocol()
        default_rtps_reliable_writer.low_watermark = 0
        default_rtps_reliable_writer.high_watermark = 1
        default_rtps_reliable_writer.heartbeat_period = dds.Duration(3)
        default_rtps_reliable_writer.fast_heartbeat_period = dds.Duration(3)
        default_rtps_reliable_writer.late_joiner_heartbeat_period = dds.Duration(
            3)
        default_rtps_reliable_writer.virtual_heartbeat_period = dds.Duration.automatic
        default_rtps_reliable_writer.samples_per_virtual_heartbeat = dds.LENGTH_UNLIMITED
        default_rtps_reliable_writer.max_heartbeat_retries = 10
        default_rtps_reliable_writer.inactivate_nonprogressing_readers = False
        default_rtps_reliable_writer.heartbeats_per_max_samples = 8
        default_rtps_reliable_writer.min_nack_response_delay = dds.Duration.zero
        default_rtps_reliable_writer.max_nack_response_delay = dds.Duration.from_milliseconds(
            200)
        default_rtps_reliable_writer.nack_suppression_duration = dds.Duration.zero
        default_rtps_reliable_writer.max_bytes_per_nack_response = 131072
        default_rtps_reliable_writer.disable_positive_acks_min_sample_keep_duration = dds.Duration.from_milliseconds(
            1)
        default_rtps_reliable_writer.disable_positive_acks_max_sample_keep_duration = dds.Duration(
            1)
        default_rtps_reliable_writer.disable_positive_acks_enable_adaptive_sample_keep_duration = True
        default_rtps_reliable_writer.disable_positive_acks_decrease_sample_keep_duration_factor = 95
        default_rtps_reliable_writer.disable_positive_acks_increase_sample_keep_duration_factor = 150
        default_rtps_reliable_writer.min_send_window_size = dds.LENGTH_UNLIMITED
        default_rtps_reliable_writer.max_send_window_size = dds.LENGTH_UNLIMITED
        default_rtps_reliable_writer.send_window_update_period = dds.Duration(
            3)
        default_rtps_reliable_writer.send_window_increase_factor = 105
        default_rtps_reliable_writer.send_window_decrease_factor = 70
        default_rtps_reliable_writer.enable_multicast_periodic_heartbeat = False
        default_rtps_reliable_writer.multicast_resend_threshold = 2
        default_rtps_reliable_writer.disable_repair_piggyback_heartbeat = False
        default_dw_protocol.rtps_reliable_writer = default_rtps_reliable_writer
        default_dw_protocol.initial_virtual_sequence_number = dds.SequenceNumber.automatic

        non_default_dw_protocol = dds.DataWriterProtocol()
        non_default_dw_protocol.virtual_guid = dds.Guid().automatic
        non_default_dw_protocol.rtps_object_id = 1
        non_default_dw_protocol.push_on_write = False
        non_default_dw_protocol.disable_positive_acks = True
        non_default_dw_protocol.disable_inline_keyhash = True
        non_default_dw_protocol.serialize_key_with_dispose = True
        non_default_dw_protocol.propagate_app_ack_with_no_response = False
        non_default_rtps_reliable_writer = dds.RtpsReliableWriterProtocol()
        non_default_rtps_reliable_writer.low_watermark = 1
        non_default_rtps_reliable_writer.high_watermark = 2
        non_default_rtps_reliable_writer.heartbeat_period = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.fast_heartbeat_period = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.late_joiner_heartbeat_period = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.virtual_heartbeat_period = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.samples_per_virtual_heartbeat = 1
        non_default_rtps_reliable_writer.max_heartbeat_retries = 1
        non_default_rtps_reliable_writer.inactivate_nonprogressing_readers = True
        non_default_rtps_reliable_writer.heartbeats_per_max_samples = 1
        non_default_rtps_reliable_writer.min_nack_response_delay = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.max_nack_response_delay = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.nack_suppression_duration = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.max_bytes_per_nack_response = 100000
        non_default_rtps_reliable_writer.disable_positive_acks_min_sample_keep_duration = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.disable_positive_acks_max_sample_keep_duration = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.disable_positive_acks_enable_adaptive_sample_keep_duration = False
        non_default_rtps_reliable_writer.disable_positive_acks_decrease_sample_keep_duration_factor = 100
        non_default_rtps_reliable_writer.disable_positive_acks_increase_sample_keep_duration_factor = 100
        non_default_rtps_reliable_writer.min_send_window_size = 1
        non_default_rtps_reliable_writer.max_send_window_size = 1
        non_default_rtps_reliable_writer.send_window_update_period = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.send_window_increase_factor = 101
        non_default_rtps_reliable_writer.send_window_decrease_factor = 90
        non_default_rtps_reliable_writer.enable_multicast_periodic_heartbeat = True
        non_default_rtps_reliable_writer.multicast_resend_threshold = 1
        non_default_rtps_reliable_writer.disable_repair_piggyback_heartbeat = True
        non_default_dw_protocol.rtps_reliable_writer = non_default_rtps_reliable_writer
        non_default_dw_protocol.initial_virtual_sequence_number = dds.SequenceNumber(
            1)

        return QosPolicy(
            dds.DataWriterProtocol,
            {Entity.DataWriter: default_dw_protocol},
            {Entity.DataWriter: non_default_dw_protocol},
            "data_writer_protocol")

    elif policy is dds.DataReaderResourceLimits:
        default_dr_resource_limits = dds.DataReaderResourceLimits()
        default_dr_resource_limits.max_remote_writers = dds.LENGTH_UNLIMITED
        default_dr_resource_limits.max_remote_writers_per_instance = dds.LENGTH_UNLIMITED
        default_dr_resource_limits.max_samples_per_remote_writer = dds.LENGTH_UNLIMITED
        default_dr_resource_limits.max_infos = dds.LENGTH_UNLIMITED
        default_dr_resource_limits.initial_remote_writers = 2
        default_dr_resource_limits.initial_remote_writers_per_instance = 2
        default_dr_resource_limits.initial_infos = 32
        default_dr_resource_limits.initial_outstanding_reads = 2
        default_dr_resource_limits.max_outstanding_reads = dds.LENGTH_UNLIMITED
        default_dr_resource_limits.max_samples_per_read = 1024
        default_dr_resource_limits.disable_fragmentation_support = False
        default_dr_resource_limits.max_fragmented_samples = 1024
        default_dr_resource_limits.initial_fragmented_samples = 4
        default_dr_resource_limits.max_fragmented_samples_per_remote_writer = 256
        default_dr_resource_limits.max_fragments_per_sample = dds.LENGTH_UNLIMITED
        default_dr_resource_limits.dynamically_allocate_fragmented_samples = True
        default_dr_resource_limits.max_total_instances = dds.DataReaderResourceLimits.AUTO_MAX_TOTAL_INSTANCES
        default_dr_resource_limits.max_remote_virtual_writers = dds.LENGTH_UNLIMITED
        default_dr_resource_limits.initial_remote_virtual_writers = 2
        default_dr_resource_limits.max_remote_virtual_writers_per_instance = dds.LENGTH_UNLIMITED
        default_dr_resource_limits.initial_remote_virtual_writers_per_instance = 2
        default_dr_resource_limits.max_remote_writers_per_sample = 3
        default_dr_resource_limits.max_query_condition_filters = 4
        default_dr_resource_limits.max_app_ack_response_length = 1
        default_dr_resource_limits.keep_minimum_state_for_instances = True
        default_dr_resource_limits.initial_topic_queries = 1
        default_dr_resource_limits.max_topic_queries = dds.LENGTH_UNLIMITED
        auto_count = dds.AllocationSettings.AUTO_COUNT
        default_dr_resource_limits.shmem_ref_transfer_mode_attached_segment_allocation = dds.AllocationSettings(
            auto_count, auto_count, auto_count)
        default_dr_resource_limits.instance_replacement = dds.DataReaderResourceLimitsInstanceReplacementSettings(
            dds.DataReaderInstanceRemovalKind.NO_INSTANCE,
            dds.DataReaderInstanceRemovalKind.EMPTY_INSTANCES,
            dds.DataReaderInstanceRemovalKind.EMPTY_INSTANCES)

        non_default_dr_resource_limits = dds.DataReaderResourceLimits()
        non_default_dr_resource_limits.max_remote_writers = 1
        non_default_dr_resource_limits.max_remote_writers_per_instance = 1
        non_default_dr_resource_limits.max_samples_per_remote_writer = 1
        non_default_dr_resource_limits.max_infos = 1
        non_default_dr_resource_limits.initial_remote_writers = 1
        non_default_dr_resource_limits.initial_remote_writers_per_instance = 1
        non_default_dr_resource_limits.initial_infos = 1
        non_default_dr_resource_limits.initial_outstanding_reads = 1
        non_default_dr_resource_limits.max_outstanding_reads = 1
        non_default_dr_resource_limits.max_samples_per_read = 1
        non_default_dr_resource_limits.disable_fragmentation_support = True
        non_default_dr_resource_limits.max_fragmented_samples = 1
        non_default_dr_resource_limits.initial_fragmented_samples = 1
        non_default_dr_resource_limits.max_fragmented_samples_per_remote_writer = 1
        non_default_dr_resource_limits.max_fragments_per_sample = 1
        non_default_dr_resource_limits.dynamically_allocate_fragmented_samples = False
        non_default_dr_resource_limits.max_total_instances = 1
        non_default_dr_resource_limits.max_remote_virtual_writers = 1
        non_default_dr_resource_limits.initial_remote_virtual_writers = 1
        non_default_dr_resource_limits.max_remote_virtual_writers_per_instance = 1
        non_default_dr_resource_limits.initial_remote_virtual_writers_per_instance = 1
        non_default_dr_resource_limits.max_remote_writers_per_sample = 1
        non_default_dr_resource_limits.max_query_condition_filters = 1
        non_default_dr_resource_limits.max_app_ack_response_length = 2
        non_default_dr_resource_limits.keep_minimum_state_for_instances = False
        non_default_dr_resource_limits.initial_topic_queries = 2
        non_default_dr_resource_limits.max_topic_queries = 2
        non_default_dr_resource_limits.shmem_ref_transfer_mode_attached_segment_allocation = dds.AllocationSettings(
            1, 1, 1)
        non_default_dr_resource_limits.instance_replacement = dds.DataReaderResourceLimitsInstanceReplacementSettings(
            dds.DataReaderInstanceRemovalKind.EMPTY_INSTANCES,
            dds.DataReaderInstanceRemovalKind.NO_INSTANCE,
            dds.DataReaderInstanceRemovalKind.NO_INSTANCE)

        return QosPolicy(
            dds.DataReaderResourceLimits,
            {Entity.DataReader: default_dr_resource_limits},
            {Entity.DataReader: non_default_dr_resource_limits},
            "data_reader_resource_limits")

    elif policy is dds.DataWriterResourceLimits:
        default_dw_resource_limits = dds.DataWriterResourceLimits()
        default_dw_resource_limits.initial_concurrent_blocking_threads = 1
        default_dw_resource_limits.max_concurrent_blocking_threads = dds.LENGTH_UNLIMITED
        default_dw_resource_limits.max_remote_readers = dds.LENGTH_UNLIMITED
        default_dw_resource_limits.max_remote_reader_filters = dds.LENGTH_UNLIMITED
        default_dw_resource_limits.initial_batches = 8
        default_dw_resource_limits.max_batches = dds.LENGTH_UNLIMITED
        default_dw_resource_limits.cookie_max_length = dds.LENGTH_UNLIMITED
        default_dw_resource_limits.instance_replacement = dds.DataWriterResourceLimitsInstanceReplacementKind.UNREGISTERED
        default_dw_resource_limits.replace_empty_instances = False
        default_dw_resource_limits.autoregister_instances = False
        default_dw_resource_limits.initial_virtual_writers = 1
        default_dw_resource_limits.max_virtual_writers = dds.LENGTH_UNLIMITED
        default_dw_resource_limits.max_remote_readers = dds.LENGTH_UNLIMITED
        default_dw_resource_limits.max_app_ack_remote_readers = dds.LENGTH_UNLIMITED
        default_dw_resource_limits.initial_active_topic_queries = 1
        default_dw_resource_limits.max_active_topic_queries = dds.LENGTH_UNLIMITED
        auto_count = dds.AllocationSettings.AUTO_COUNT
        default_dw_resource_limits.writer_loaned_sample_allocation = dds.AllocationSettings(
            auto_count, auto_count, auto_count)
        default_dw_resource_limits.initialize_writer_loaned_sample = False

        non_default_dw_resource_limits = dds.DataWriterResourceLimits()
        non_default_dw_resource_limits.initial_concurrent_blocking_threads = 2
        non_default_dw_resource_limits.max_concurrent_blocking_threads = 2
        non_default_dw_resource_limits.max_remote_readers = 2
        non_default_dw_resource_limits.max_remote_reader_filters = 2
        non_default_dw_resource_limits.initial_batches = 2
        non_default_dw_resource_limits.max_batches = 2
        non_default_dw_resource_limits.cookie_max_length = 2
        non_default_dw_resource_limits.instance_replacement = dds.DataWriterResourceLimitsInstanceReplacementKind.ALIVE
        non_default_dw_resource_limits.replace_empty_instances = True
        non_default_dw_resource_limits.autoregister_instances = True
        non_default_dw_resource_limits.initial_virtual_writers = 2
        non_default_dw_resource_limits.max_virtual_writers = 2
        non_default_dw_resource_limits.max_remote_readers = 2
        non_default_dw_resource_limits.max_app_ack_remote_readers = 2
        non_default_dw_resource_limits.initial_active_topic_queries = 2
        non_default_dw_resource_limits.max_active_topic_queries = 2
        non_default_dw_resource_limits.writer_loaned_sample_allocation = dds.AllocationSettings(
            2, 2, 2)
        non_default_dw_resource_limits.initialize_writer_loaned_sample = True

        return QosPolicy(
            dds.DataWriterResourceLimits,
            {Entity.DataWriter: default_dw_resource_limits},
            {Entity.DataWriter: non_default_dw_resource_limits},
            "data_writer_resource_limits")

    elif policy is dds.DataRepresentation:
        default_data_representation_writer_topic = dds.DataRepresentation()
        default_data_representation_writer_topic.value = [
            dds.DataRepresentation.AUTO_ID]
        default_data_representation_writer_topic.compression_settings = dds.CompressionSettings(
            dds.CompressionIdMask.NONE, dds.CompressionSettings.COMPRESSION_LEVEL_DEFAULT, 8192)

        default_data_representation_reader = dds.DataRepresentation()
        default_data_representation_reader.value = [
            dds.DataRepresentation.AUTO_ID]
        default_data_representation_reader.compression_settings = dds.CompressionSettings(
            dds.CompressionIdMask.ALL, dds.CompressionSettings.COMPRESSION_LEVEL_DEFAULT, 8192)

        non_default_data_representation_writer_topic = dds.DataRepresentation()
        non_default_data_representation_writer_topic.value = [
            dds.DataRepresentation.XCDR2]
        non_default_data_representation_writer_topic.compression_settings = dds.CompressionSettings(
            dds.CompressionIdMask.ALL, dds.CompressionSettings.COMPRESSION_LEVEL_BEST_SPEED, 4096)

        non_default_data_representation_reader = dds.DataRepresentation()
        non_default_data_representation_reader.value = [
            dds.DataRepresentation.XCDR2]
        non_default_data_representation_reader.compression_settings = dds.CompressionSettings(
            dds.CompressionIdMask.NONE, dds.CompressionSettings.COMPRESSION_LEVEL_BEST_COMPRESSION, 4096)

        return QosPolicy(
            dds.DataRepresentation,
            {Entity.DataWriter: default_data_representation_writer_topic,
             Entity.Topic: default_data_representation_writer_topic,
             Entity.DataReader: default_data_representation_reader},
            {Entity.DataWriter: non_default_data_representation_writer_topic,
             Entity.Topic: non_default_data_representation_writer_topic,
             Entity.DataReader: non_default_data_representation_reader},
            "data_representation")

    elif policy is dds.DataTag:
        raise NotImplementedError(
            "DataTag QoS Policy not tested in generic test")

    elif policy is dds.Deadline:
        default_deadline = dds.Deadline()
        default_deadline.period = dds.Duration.infinite

        non_default_deadline = dds.Deadline(1, 0)
        non_default_deadline = dds.Deadline(dds.Duration(1, 1))

        return QosPolicy(
            dds.Deadline,
            {Entity.DataWriter: default_deadline,
             Entity.DataReader: default_deadline,
             Entity.Topic: default_deadline},
            {Entity.DataWriter: non_default_deadline,
             Entity.DataReader: non_default_deadline,
             Entity.Topic: non_default_deadline},
            "deadline")

    elif policy is dds.DestinationOrder:
        default_destination_order_reader = dds.DestinationOrder()
        default_destination_order_reader.kind = dds.DestinationOrderKind.BY_RECEPTION_TIMESTAMP
        default_destination_order_reader.scope = dds.DestinationOrderScopeKind.INSTANCE

        default_destination_order_reader.source_timestamp_tolerance = dds.Duration(
            30)

        default_destination_order_writer = dds.DestinationOrder()
        default_destination_order_writer.kind = dds.DestinationOrderKind.BY_RECEPTION_TIMESTAMP
        default_destination_order_writer.scope = dds.DestinationOrderScopeKind.INSTANCE
        default_destination_order_writer.source_timestamp_tolerance = dds.Duration.from_milliseconds(
            100)

        non_default_destination_order_reader = dds.DestinationOrder(
            dds.DestinationOrderKind.BY_SOURCE_TIMESTAMP)
        non_default_destination_order_reader.scope = dds.DestinationOrderScopeKind.TOPIC
        non_default_destination_order_reader.source_timestamp_tolerance = dds.Duration(
            2)

        non_default_destination_order_writer = dds.DestinationOrder()
        non_default_destination_order_writer.kind = dds.DestinationOrderKind.BY_SOURCE_TIMESTAMP
        non_default_destination_order_writer.scope = dds.DestinationOrderScopeKind.TOPIC
        non_default_destination_order_writer.source_timestamp_tolerance = dds.Duration(
            30)

        return QosPolicy(
            dds.DestinationOrder,
            {Entity.DataWriter: default_destination_order_writer,
             Entity.DataReader: default_destination_order_reader},
            {Entity.DataWriter: non_default_destination_order_writer,
             Entity.DataReader: non_default_destination_order_reader},
            "destination_order")

    elif policy is dds.Discovery:
        default_discovery = dds.Discovery()
        default_discovery.enabled_transports = []
        default_discovery.initial_peers = [
            "builtin.udpv4://239.255.0.1",
            "builtin.udpv4://127.0.0.1",
            "builtin.shmem://"]
        default_discovery.multicast_receive_addresses = [
            "builtin.udpv4://239.255.0.1"]
        default_discovery.metatraffic_transport_priority = 0
        default_discovery.accept_unknown_peers = True
        default_discovery.enable_endpoint_discovery = True

        non_default_discovery = dds.Discovery()
        non_default_discovery.enabled_transports = ["builtin.udpv4"]
        non_default_discovery.initial_peers = ["builtin.shmem://"]
        non_default_discovery.multicast_receive_addresses = []
        non_default_discovery.metatraffic_transport_priority = 1
        non_default_discovery.accept_unknown_peers = False
        non_default_discovery.enable_endpoint_discovery = False

        return QosPolicy(
            dds.Discovery,
            {Entity.DomainParticipant: default_discovery},
            {Entity.DomainParticipant: non_default_discovery},
            "discovery")

    elif policy is dds.DiscoveryConfig:

        default_discovery_config = dds.DiscoveryConfig()
        default_discovery_config.participant_liveliness_lease_duration = dds.Duration(
            100)
        default_discovery_config.participant_liveliness_assert_period = dds.Duration(
            30)
        default_discovery_config.participant_announcement_period = dds.Duration.automatic
        default_discovery_config.remote_participant_purge_kind = dds.RemoteParticipantPurgeKind.LIVELINESS_BASED
        default_discovery_config.max_liveliness_loss_detection_period = dds.Duration(
            60)
        default_discovery_config.initial_participant_announcements = 5
        default_discovery_config.new_remote_participant_announcements = 2
        default_discovery_config.min_initial_participant_announcement_period = dds.Duration.from_milliseconds(
            10)
        default_discovery_config.max_initial_participant_announcement_period = dds.Duration(
            1)
        default_discovery_config_resource_limits = dds.BuiltinTopicReaderResourceLimits()
        default_discovery_config_resource_limits.initial_samples = 64
        default_discovery_config_resource_limits.max_samples = dds.LENGTH_UNLIMITED
        default_discovery_config_resource_limits.initial_infos = 64
        default_discovery_config_resource_limits.max_infos = dds.LENGTH_UNLIMITED
        default_discovery_config_resource_limits.initial_outstanding_reads = 2
        default_discovery_config_resource_limits.max_outstanding_reads = dds.LENGTH_UNLIMITED
        default_discovery_config_resource_limits.max_samples_per_read = 1024
        default_discovery_config_resource_limits.disable_fragmentation_support = False
        default_discovery_config_resource_limits.max_fragmented_samples = 1024
        default_discovery_config_resource_limits.initial_fragmented_samples = 4
        default_discovery_config_resource_limits.max_fragmented_samples_per_remote_writer = 256
        default_discovery_config_resource_limits.max_fragments_per_sample = dds.LENGTH_UNLIMITED
        default_discovery_config_resource_limits.dynamically_allocate_fragmented_samples = True
        default_discovery_config.participant_reader_resource_limits = default_discovery_config_resource_limits
        default_discovery_config_pub_reader = dds.RtpsReliableReaderProtocol()
        default_discovery_config_pub_reader.min_heartbeat_response_delay = dds.Duration.zero
        default_discovery_config_pub_reader.max_heartbeat_response_delay = dds.Duration.zero
        default_discovery_config_pub_reader.heartbeat_suppression_duration = dds.Duration.from_microseconds(
            62500)
        default_discovery_config_pub_reader.nack_period = dds.Duration(5)
        default_discovery_config_pub_reader.receive_window_size = 256
        default_discovery_config_pub_reader.round_trip_time = dds.Duration.zero
        default_discovery_config_pub_reader.app_ack_period = dds.Duration(5)
        default_discovery_config_pub_reader.samples_per_app_ack = 1
        default_discovery_config.publication_reader = default_discovery_config_pub_reader
        default_discovery_config.publication_reader_resource_limits = default_discovery_config_resource_limits
        default_discovery_config.subscription_reader = default_discovery_config_pub_reader
        default_discovery_config.subscription_reader_resource_limits = default_discovery_config_resource_limits
        default_rtps_reliable_writer = dds.RtpsReliableWriterProtocol()
        default_rtps_reliable_writer.low_watermark = 0
        default_rtps_reliable_writer.high_watermark = 1
        default_rtps_reliable_writer.heartbeat_period = dds.Duration(3)
        default_rtps_reliable_writer.fast_heartbeat_period = dds.Duration(3)
        default_rtps_reliable_writer.late_joiner_heartbeat_period = dds.Duration(
            3)
        default_rtps_reliable_writer.virtual_heartbeat_period = dds.Duration.automatic
        default_rtps_reliable_writer.samples_per_virtual_heartbeat = dds.LENGTH_UNLIMITED
        default_rtps_reliable_writer.max_heartbeat_retries = 10
        default_rtps_reliable_writer.inactivate_nonprogressing_readers = False
        default_rtps_reliable_writer.heartbeats_per_max_samples = 8
        default_rtps_reliable_writer.min_nack_response_delay = dds.Duration.zero
        default_rtps_reliable_writer.nack_suppression_duration = dds.Duration.zero
        default_rtps_reliable_writer.max_bytes_per_nack_response = 131072
        default_rtps_reliable_writer.disable_positive_acks_min_sample_keep_duration = dds.Duration.from_milliseconds(
            1)
        default_rtps_reliable_writer.disable_positive_acks_max_sample_keep_duration = dds.Duration(
            1)
        default_rtps_reliable_writer.disable_positive_acks_enable_adaptive_sample_keep_duration = True
        default_rtps_reliable_writer.disable_positive_acks_decrease_sample_keep_duration_factor = 95
        default_rtps_reliable_writer.disable_positive_acks_increase_sample_keep_duration_factor = 150
        default_rtps_reliable_writer.min_send_window_size = dds.LENGTH_UNLIMITED
        default_rtps_reliable_writer.max_send_window_size = dds.LENGTH_UNLIMITED
        default_rtps_reliable_writer.send_window_update_period = dds.Duration(
            3)
        default_rtps_reliable_writer.send_window_increase_factor = 105
        default_rtps_reliable_writer.enable_multicast_periodic_heartbeat = False
        default_rtps_reliable_writer.multicast_resend_threshold = 2
        default_rtps_reliable_writer.disable_repair_piggyback_heartbeat = False
        default_rtps_reliable_writer.send_window_decrease_factor = 50
        default_rtps_reliable_writer.max_nack_response_delay = dds.Duration.zero
        default_discovery_config.publication_writer = default_rtps_reliable_writer
        default_discovery_config_pub_writer_data_lifecycle = dds.WriterDataLifecycle()
        default_discovery_config_pub_writer_data_lifecycle.autodispose_unregistered_instances = False
        default_discovery_config_pub_writer_data_lifecycle.autopurge_unregistered_instances_delay = dds.Duration.zero
        default_discovery_config_pub_writer_data_lifecycle.autopurge_disposed_instances_delay = dds.Duration.infinite
        default_discovery_config.publication_writer_data_lifecycle = default_discovery_config_pub_writer_data_lifecycle
        default_discovery_config.subscription_writer = default_rtps_reliable_writer
        default_discovery_config.subscription_reader_resource_limits = default_discovery_config_resource_limits
        default_discovery_config.subscription_writer_data_lifecycle = default_discovery_config_pub_writer_data_lifecycle
        default_discovery_config.builtin_discovery_plugins = dds.DiscoveryConfigBuiltinPluginKindMask.SDP
        default_discovery_config.enabled_builtin_channels = dds.DiscoveryConfigBuiltinChannelKindMask.SERVICE_REQUEST
        default_discovery_config.participant_message_reader_reliability_kind = dds.ReliabilityKind.BEST_EFFORT
        default_rtps_reliable_reader = dds.RtpsReliableReaderProtocol()
        default_rtps_reliable_reader.min_heartbeat_response_delay = dds.Duration.zero
        default_rtps_reliable_reader.heartbeat_suppression_duration = dds.Duration.from_microseconds(
            62500)
        default_rtps_reliable_reader.nack_period = dds.Duration(5)
        default_rtps_reliable_reader.receive_window_size = 256
        default_rtps_reliable_reader.round_trip_time = dds.Duration.zero
        default_rtps_reliable_reader.app_ack_period = dds.Duration(5)
        default_rtps_reliable_reader.samples_per_app_ack = 1
        default_rtps_reliable_reader.max_heartbeat_response_delay = dds.Duration.zero
        default_discovery_config.participant_message_reader = default_rtps_reliable_reader
        default_rtps_reliable_writer.fast_heartbeat_period = dds.Duration(1)
        default_rtps_reliable_writer.heartbeat_period = dds.Duration(1)
        default_rtps_reliable_writer.heartbeats_per_max_samples = 1
        default_rtps_reliable_writer.late_joiner_heartbeat_period = dds.Duration(
            1)
        default_rtps_reliable_writer.max_bytes_per_nack_response = 9216
        default_rtps_reliable_writer.send_window_update_period = dds.Duration(
            1)
        default_discovery_config.participant_message_writer = default_rtps_reliable_writer
        default_discovery_config_publish_mode = dds.PublishMode()
        default_discovery_config_publish_mode.kind = dds.PublishModeKind.SYNCHRONOUS
        default_discovery_config_publish_mode.flow_controller_name = dds.FlowController.DEFAULT_FLOW_CONTROLLER_NAME
        default_discovery_config_publish_mode.priority = dds.PublishMode.PUBLICATION_PRIORITY_UNDEFINED
        default_discovery_config.publication_writer_publish_mode = default_discovery_config_publish_mode
        default_discovery_config.subscription_writer_publish_mode = default_discovery_config_publish_mode
        default_async_pub = dds.AsynchronousPublisher()
        default_async_pub.disable_asynchronous_write = False
        default_async_pub.thread = dds.ThreadSettings()
        default_async_pub.thread.mask = dds.ThreadSettingsKindMask()
        default_async_pub.disable_asynchronous_batch = False
        default_async_pub.asynchronous_batch_thread = dds.ThreadSettings()
        default_async_pub.asynchronous_batch_thread.mask = dds.ThreadSettingsKindMask()
        default_async_pub.disable_topic_query_publication = False
        default_async_pub.topic_query_publication_thread = dds.ThreadSettings()
        default_async_pub.topic_query_publication_thread.mask = dds.ThreadSettingsKindMask()
        default_discovery_config.asynchronous_publisher = default_async_pub
        default_discovery_config.default_domain_announcement_period = dds.Duration(
            30)
        default_discovery_config.ignore_default_domain_announcements = True

        default_discovery_config.service_request_writer_data_lifecycle = default_discovery_config_pub_writer_data_lifecycle
        default_discovery_config.service_request_writer_publish_mode = default_discovery_config_publish_mode
        default_discovery_config.service_request_reader = default_rtps_reliable_reader
        default_discovery_config.locator_reachability_assert_period = dds.Duration(
            20)
        default_discovery_config.locator_reachability_lease_duration = dds.Duration.infinite
        default_discovery_config.locator_reachability_change_detection_period = dds.Duration(
            60)
        default_rtps_reliable_writer.fast_heartbeat_period = dds.Duration.from_milliseconds(
            250)
        default_rtps_reliable_writer.max_heartbeat_retries = dds.LENGTH_UNLIMITED
        default_discovery_config.secure_volatile_writer = default_rtps_reliable_writer
        default_discovery_config.secure_volatile_writer_publish_mode = default_discovery_config_publish_mode

        default_discovery_config.secure_volatile_reader = default_rtps_reliable_reader
        default_discovery_config.endpoint_type_object_lb_serialization_threshold = 0
        default_discovery_config.dns_tracker_polling_period = dds.Duration.infinite
        default_rtps_reliable_writer.heartbeat_period = dds.Duration(3)
        default_rtps_reliable_writer.fast_heartbeat_period = dds.Duration(3)
        default_rtps_reliable_writer.heartbeats_per_max_samples = 8
        default_rtps_reliable_writer.late_joiner_heartbeat_period = dds.Duration(
            3)
        default_rtps_reliable_writer.max_bytes_per_nack_response = 131072
        default_rtps_reliable_writer.max_heartbeat_retries = 10
        default_rtps_reliable_writer.send_window_update_period = dds.Duration(
            3)
        default_discovery_config.service_request_writer = default_rtps_reliable_writer

        non_default_discovery_config = dds.DiscoveryConfig()
        non_default_discovery_config.participant_liveliness_lease_duration = dds.Duration(
            30)
        non_default_discovery_config.participant_liveliness_assert_period = dds.Duration(
            10)
        non_default_discovery_config.participant_announcement_period = dds.Duration(
            30)
        non_default_discovery_config.remote_participant_purge_kind = dds.RemoteParticipantPurgeKind.NO_PURGE
        non_default_discovery_config.max_liveliness_loss_detection_period = dds.Duration(
            30)
        non_default_discovery_config.initial_participant_announcements = 3
        non_default_discovery_config.new_remote_participant_announcements = 3
        non_default_discovery_config.min_initial_participant_announcement_period = dds.Duration(
            1)
        non_default_discovery_config.max_initial_participant_announcement_period = dds.Duration(
            2)
        non_default_discovery_config_resource_limits = dds.BuiltinTopicReaderResourceLimits()
        non_default_discovery_config_resource_limits.initial_samples = 100
        non_default_discovery_config_resource_limits.max_samples = 200
        non_default_discovery_config_resource_limits.initial_infos = 100
        non_default_discovery_config_resource_limits.max_infos = 200
        non_default_discovery_config_resource_limits.initial_outstanding_reads = 1
        non_default_discovery_config_resource_limits.max_outstanding_reads = 100
        non_default_discovery_config_resource_limits.max_samples_per_read = 100
        non_default_discovery_config_resource_limits.disable_fragmentation_support = True
        non_default_discovery_config_resource_limits.max_fragmented_samples = 1000
        non_default_discovery_config_resource_limits.initial_fragmented_samples = 1
        non_default_discovery_config_resource_limits.max_fragmented_samples_per_remote_writer = 100
        non_default_discovery_config_resource_limits.max_fragments_per_sample = 10000
        non_default_discovery_config_resource_limits.dynamically_allocate_fragmented_samples = False
        non_default_discovery_config.participant_reader_resource_limits = non_default_discovery_config_resource_limits
        non_default_discovery_config_pub_reader = dds.RtpsReliableReaderProtocol()
        non_default_discovery_config_pub_reader.min_heartbeat_response_delay = dds.Duration(
            1)
        non_default_discovery_config_pub_reader.max_heartbeat_response_delay = dds.Duration(
            10)
        non_default_discovery_config_pub_reader.heartbeat_suppression_duration = dds.Duration(
            1)
        non_default_discovery_config_pub_reader.nack_period = dds.Duration(3)
        non_default_discovery_config_pub_reader.receive_window_size = 512
        non_default_discovery_config_pub_reader.round_trip_time = dds.Duration(
            1)
        non_default_discovery_config_pub_reader.app_ack_period = dds.Duration(
            10)
        non_default_discovery_config_pub_reader.samples_per_app_ack = 3
        non_default_discovery_config.publication_reader = non_default_discovery_config_pub_reader
        non_default_discovery_config.publication_reader_resource_limits = non_default_discovery_config_resource_limits
        non_default_discovery_config.subscription_reader = non_default_discovery_config_pub_reader
        non_default_discovery_config.subscription_reader_resource_limits = non_default_discovery_config_resource_limits
        non_default_rtps_reliable_writer = dds.RtpsReliableWriterProtocol()
        non_default_rtps_reliable_writer.low_watermark = 1
        non_default_rtps_reliable_writer.high_watermark = 2
        non_default_rtps_reliable_writer.heartbeat_period = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.fast_heartbeat_period = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.late_joiner_heartbeat_period = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.virtual_heartbeat_period = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.samples_per_virtual_heartbeat = 1
        non_default_rtps_reliable_writer.max_heartbeat_retries = 1
        non_default_rtps_reliable_writer.inactivate_nonprogressing_readers = True
        non_default_rtps_reliable_writer.heartbeats_per_max_samples = 1
        non_default_rtps_reliable_writer.min_nack_response_delay = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.max_nack_response_delay = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.nack_suppression_duration = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.max_bytes_per_nack_response = 100000
        non_default_rtps_reliable_writer.disable_positive_acks_min_sample_keep_duration = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.disable_positive_acks_max_sample_keep_duration = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.disable_positive_acks_enable_adaptive_sample_keep_duration = False
        non_default_rtps_reliable_writer.disable_positive_acks_decrease_sample_keep_duration_factor = 100
        non_default_rtps_reliable_writer.disable_positive_acks_increase_sample_keep_duration_factor = 100
        non_default_rtps_reliable_writer.min_send_window_size = 1
        non_default_rtps_reliable_writer.max_send_window_size = 1
        non_default_rtps_reliable_writer.send_window_update_period = dds.Duration.from_milliseconds(
            100)
        non_default_rtps_reliable_writer.send_window_increase_factor = 101
        non_default_rtps_reliable_writer.send_window_decrease_factor = 90
        non_default_rtps_reliable_writer.enable_multicast_periodic_heartbeat = True
        non_default_rtps_reliable_writer.multicast_resend_threshold = 1
        non_default_rtps_reliable_writer.disable_repair_piggyback_heartbeat = True
        non_default_discovery_config.publication_writer = non_default_rtps_reliable_writer
        non_default_discovery_config_pub_writer_data_lifecycle = dds.WriterDataLifecycle()
        non_default_discovery_config_pub_writer_data_lifecycle.autodispose_unregistered_instances = True
        non_default_discovery_config_pub_writer_data_lifecycle.autopurge_unregistered_instances_delay = dds.Duration(
            10)
        non_default_discovery_config_pub_writer_data_lifecycle.autopurge_disposed_instances_delay = dds.Duration(
            30)
        non_default_discovery_config.publication_writer_data_lifecycle = non_default_discovery_config_pub_writer_data_lifecycle
        non_default_discovery_config.subscription_writer = non_default_rtps_reliable_writer
        non_default_discovery_config.subscription_reader_resource_limits = non_default_discovery_config_resource_limits
        non_default_discovery_config.subscription_writer_data_lifecycle = non_default_discovery_config_pub_writer_data_lifecycle
        non_default_discovery_config.builtin_discovery_plugins = dds.DiscoveryConfigBuiltinPluginKindMask.SPDP
        non_default_discovery_config.enabled_builtin_channels = dds.DiscoveryConfigBuiltinChannelKindMask.NONE
        non_default_discovery_config.participant_message_reader_reliability_kind = dds.ReliabilityKind.RELIABLE
        non_default_rtps_reliable_reader = dds.RtpsReliableReaderProtocol()
        non_default_rtps_reliable_reader.min_heartbeat_response_delay = dds.Duration.from_milliseconds(
            1)
        non_default_rtps_reliable_reader.max_heartbeat_response_delay = dds.Duration.from_milliseconds(
            1000)
        non_default_rtps_reliable_reader.heartbeat_suppression_duration = dds.Duration.from_microseconds(
            125000)
        non_default_rtps_reliable_reader.nack_period = dds.Duration(10)
        non_default_rtps_reliable_reader.receive_window_size = 512
        non_default_rtps_reliable_reader.round_trip_time = dds.Duration.from_milliseconds(
            1)
        non_default_rtps_reliable_reader.app_ack_period = dds.Duration(10)
        non_default_rtps_reliable_reader.samples_per_app_ack = 2
        non_default_discovery_config.participant_message_reader = non_default_rtps_reliable_reader
        non_default_discovery_config.participant_message_writer = non_default_rtps_reliable_writer
        non_default_discovery_config_publish_mode = dds.PublishMode()
        non_default_discovery_config_publish_mode.kind = dds.PublishModeKind.ASYNCHRONOUS
        non_default_discovery_config_publish_mode.flow_controller_name = "foo"
        non_default_discovery_config_publish_mode.priority = 3
        non_default_discovery_config.publication_writer_publish_mode = non_default_discovery_config_publish_mode
        non_default_discovery_config.subscription_writer_publish_mode = non_default_discovery_config_publish_mode
        non_default_async_pub = dds.AsynchronousPublisher()
        non_default_async_pub.disable_asynchronous_write = True
        non_default_async_pub.thread = dds.ThreadSettings()
        non_default_async_pub.thread.mask = dds.ThreadSettingsKindMask.CANCEL_ASYNCHRONOUS
        non_default_async_pub.disable_asynchronous_batch = True
        non_default_async_pub.asynchronous_batch_thread = dds.ThreadSettings()
        non_default_async_pub.asynchronous_batch_thread.mask = dds.ThreadSettingsKindMask.CANCEL_ASYNCHRONOUS
        non_default_async_pub.disable_topic_query_publication = True
        non_default_async_pub.topic_query_publication_thread = dds.ThreadSettings()
        non_default_async_pub.topic_query_publication_thread.mask = dds.ThreadSettingsKindMask.CANCEL_ASYNCHRONOUS
        non_default_discovery_config.asynchronous_publisher = non_default_async_pub
        non_default_discovery_config.default_domain_announcement_period = dds.Duration(
            20)
        non_default_discovery_config.ignore_default_domain_announcements = False
        non_default_discovery_config.service_request_writer = non_default_rtps_reliable_writer
        non_default_discovery_config.service_request_writer_data_lifecycle = non_default_discovery_config_pub_writer_data_lifecycle
        non_default_discovery_config.service_request_writer_publish_mode = non_default_discovery_config_publish_mode
        non_default_discovery_config.service_request_reader = non_default_rtps_reliable_reader
        non_default_discovery_config.locator_reachability_assert_period = dds.Duration(
            30)
        non_default_discovery_config.locator_reachability_lease_duration = dds.Duration(
            24)
        non_default_discovery_config.locator_reachability_change_detection_period = dds.Duration(
            30)
        non_default_discovery_config.secure_volatile_writer = non_default_rtps_reliable_writer
        non_default_discovery_config.secure_volatile_writer_publish_mode = non_default_discovery_config_publish_mode
        non_default_discovery_config.secure_volatile_reader = non_default_rtps_reliable_reader
        non_default_discovery_config.endpoint_type_object_lb_serialization_threshold = 10
        non_default_discovery_config.dns_tracker_polling_period = dds.Duration(
            20)

        return QosPolicy(
            dds.DiscoveryConfig,
            {Entity.DomainParticipant: default_discovery_config},
            {Entity.DomainParticipant: non_default_discovery_config},
            "discovery_config")

    elif policy is dds.DomainParticipantResourceLimits:

        default_domain_participant_resource_limits = dds.DomainParticipantResourceLimits()
        default_domain_participant_resource_limits.local_writer_allocation = dds.AllocationSettings(
            16, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.local_reader_allocation = dds.AllocationSettings(
            16, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.local_publisher_allocation = dds.AllocationSettings(
            4, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.local_subscriber_allocation = dds.AllocationSettings(
            4, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.local_topic_allocation = dds.AllocationSettings(
            16, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.remote_writer_allocation = dds.AllocationSettings(
            64, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.remote_reader_allocation = dds.AllocationSettings(
            64, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.remote_participant_allocation = dds.AllocationSettings(
            16, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.matching_writer_reader_pair_allocation = dds.AllocationSettings(
            32, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.matching_reader_writer_pair_allocation = dds.AllocationSettings(
            32, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.ignored_entity_allocation = dds.AllocationSettings(
            8, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.content_filtered_topic_allocation = dds.AllocationSettings(
            4, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.content_filter_allocation = dds.AllocationSettings(
            4, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.read_condition_allocation = dds.AllocationSettings(
            4, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.query_condition_allocation = dds.AllocationSettings(
            4, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.outstanding_asynchronous_sample_allocation = dds.AllocationSettings(
            64, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.flow_controller_allocation = dds.AllocationSettings(
            4, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.local_writer_hash_buckets = 4
        default_domain_participant_resource_limits.local_reader_hash_buckets = 4
        default_domain_participant_resource_limits.local_publisher_hash_buckets = 1
        default_domain_participant_resource_limits.local_subscriber_hash_buckets = 1
        default_domain_participant_resource_limits.local_topic_hash_buckets = 4
        default_domain_participant_resource_limits.remote_writer_hash_buckets = 16
        default_domain_participant_resource_limits.remote_reader_hash_buckets = 16
        default_domain_participant_resource_limits.remote_participant_hash_buckets = 4
        default_domain_participant_resource_limits.matching_writer_reader_pair_hash_buckets = 32
        default_domain_participant_resource_limits.matching_reader_writer_pair_hash_buckets = 32
        default_domain_participant_resource_limits.ignored_entity_hash_buckets = 1
        default_domain_participant_resource_limits.content_filtered_topic_hash_buckets = 1
        default_domain_participant_resource_limits.content_filter_hash_buckets = 1
        default_domain_participant_resource_limits.flow_controller_hash_buckets = 1
        default_domain_participant_resource_limits.max_gather_destinations = 16
        default_domain_participant_resource_limits.participant_user_data_max_length = 256
        default_domain_participant_resource_limits.topic_data_max_length = 256
        default_domain_participant_resource_limits.publisher_group_data_max_length = 256
        default_domain_participant_resource_limits.subscriber_group_data_max_length = 256
        default_domain_participant_resource_limits.writer_user_data_max_length = 256
        default_domain_participant_resource_limits.reader_user_data_max_length = 256
        default_domain_participant_resource_limits.max_partitions = 64
        default_domain_participant_resource_limits.max_partition_cumulative_characters = 256
        default_domain_participant_resource_limits.type_code_max_serialized_length = 0
        default_domain_participant_resource_limits.type_object_max_serialized_length = 8192
        default_domain_participant_resource_limits.serialized_type_object_dynamic_allocation_threshold = 8192
        default_domain_participant_resource_limits.type_object_max_deserialized_length = dds.LENGTH_UNLIMITED
        default_domain_participant_resource_limits.deserialized_type_object_dynamic_allocation_threshold = 4096
        default_domain_participant_resource_limits.contentfilter_property_max_length = 256
        default_domain_participant_resource_limits.channel_seq_max_length = 32
        default_domain_participant_resource_limits.channel_filter_expression_max_length = 256
        default_domain_participant_resource_limits.participant_property_list_max_length = 32
        default_domain_participant_resource_limits.participant_property_string_max_length = 4096
        default_domain_participant_resource_limits.writer_property_list_max_length = 32
        default_domain_participant_resource_limits.writer_property_string_max_length = 1024
        default_domain_participant_resource_limits.reader_property_list_max_length = 32
        default_domain_participant_resource_limits.reader_property_string_max_length = 1024
        default_domain_participant_resource_limits.max_endpoint_groups = 32
        default_domain_participant_resource_limits.max_endpoint_group_cumulative_characters = 1024
        default_domain_participant_resource_limits.transport_info_list_max_length = 12
        default_domain_participant_resource_limits.ignored_entity_replacement_kind = dds.IgnoredEntityReplacementKind.NO_REPLACEMENT
        default_domain_participant_resource_limits.remote_topic_query_allocation = dds.AllocationSettings(
            1, dds.LENGTH_UNLIMITED, -1)
        default_domain_participant_resource_limits.remote_topic_query_hash_buckets = 1
        default_domain_participant_resource_limits.writer_data_tag_list_max_length = 0
        default_domain_participant_resource_limits.writer_data_tag_string_max_length = 0
        default_domain_participant_resource_limits.reader_data_tag_list_max_length = 0
        default_domain_participant_resource_limits.reader_data_tag_string_max_length = 0
        default_domain_participant_resource_limits.shmem_ref_transfer_mode_max_segments = 500

        non_default_domain_participant_resource_limits = dds.DomainParticipantResourceLimits()
        non_default_domain_participant_resource_limits.local_writer_allocation = dds.AllocationSettings(
            160, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.local_reader_allocation = dds.AllocationSettings(
            160, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.local_publisher_allocation = dds.AllocationSettings(
            40, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.local_subscriber_allocation = dds.AllocationSettings(
            40, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.local_topic_allocation = dds.AllocationSettings(
            160, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.remote_writer_allocation = dds.AllocationSettings(
            640, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.remote_reader_allocation = dds.AllocationSettings(
            640, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.remote_participant_allocation = dds.AllocationSettings(
            160, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.matching_writer_reader_pair_allocation = dds.AllocationSettings(
            320, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.matching_reader_writer_pair_allocation = dds.AllocationSettings(
            320, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.ignored_entity_allocation = dds.AllocationSettings(
            80, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.content_filtered_topic_allocation = dds.AllocationSettings(
            40, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.content_filter_allocation = dds.AllocationSettings(
            40, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.read_condition_allocation = dds.AllocationSettings(
            40, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.query_condition_allocation = dds.AllocationSettings(
            40, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.outstanding_asynchronous_sample_allocation = dds.AllocationSettings(
            640, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.flow_controller_allocation = dds.AllocationSettings(
            40, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.local_writer_hash_buckets = 40
        non_default_domain_participant_resource_limits.local_reader_hash_buckets = 40
        non_default_domain_participant_resource_limits.local_publisher_hash_buckets = 10
        non_default_domain_participant_resource_limits.local_subscriber_hash_buckets = 10
        non_default_domain_participant_resource_limits.local_topic_hash_buckets = 40
        non_default_domain_participant_resource_limits.remote_writer_hash_buckets = 160
        non_default_domain_participant_resource_limits.remote_reader_hash_buckets = 160
        non_default_domain_participant_resource_limits.remote_participant_hash_buckets = 40
        non_default_domain_participant_resource_limits.matching_writer_reader_pair_hash_buckets = 320
        non_default_domain_participant_resource_limits.matching_reader_writer_pair_hash_buckets = 320
        non_default_domain_participant_resource_limits.ignored_entity_hash_buckets = 10
        non_default_domain_participant_resource_limits.content_filtered_topic_hash_buckets = 10
        non_default_domain_participant_resource_limits.content_filter_hash_buckets = 10
        non_default_domain_participant_resource_limits.flow_controller_hash_buckets = 10
        non_default_domain_participant_resource_limits.max_gather_destinations = 160
        non_default_domain_participant_resource_limits.participant_user_data_max_length = 2560
        non_default_domain_participant_resource_limits.topic_data_max_length = 2560
        non_default_domain_participant_resource_limits.publisher_group_data_max_length = 2560
        non_default_domain_participant_resource_limits.subscriber_group_data_max_length = 2560
        non_default_domain_participant_resource_limits.writer_user_data_max_length = 2560
        non_default_domain_participant_resource_limits.reader_user_data_max_length = 2560
        non_default_domain_participant_resource_limits.max_partitions = 640
        non_default_domain_participant_resource_limits.max_partition_cumulative_characters = 2560
        non_default_domain_participant_resource_limits.type_code_max_serialized_length = 10000
        non_default_domain_participant_resource_limits.type_object_max_serialized_length = 81920
        non_default_domain_participant_resource_limits.serialized_type_object_dynamic_allocation_threshold = 81920
        non_default_domain_participant_resource_limits.type_object_max_deserialized_length = 10000
        non_default_domain_participant_resource_limits.deserialized_type_object_dynamic_allocation_threshold = 40960
        non_default_domain_participant_resource_limits.contentfilter_property_max_length = 2560
        non_default_domain_participant_resource_limits.channel_seq_max_length = 320
        non_default_domain_participant_resource_limits.channel_filter_expression_max_length = 2560
        non_default_domain_participant_resource_limits.participant_property_list_max_length = 40960
        non_default_domain_participant_resource_limits.participant_property_string_max_length = 10
        non_default_domain_participant_resource_limits.writer_property_list_max_length = 320
        non_default_domain_participant_resource_limits.writer_property_string_max_length = 10240
        non_default_domain_participant_resource_limits.reader_property_list_max_length = 320
        non_default_domain_participant_resource_limits.reader_property_string_max_length = 10240
        non_default_domain_participant_resource_limits.max_endpoint_groups = 320
        non_default_domain_participant_resource_limits.max_endpoint_group_cumulative_characters = 10240
        non_default_domain_participant_resource_limits.transport_info_list_max_length = 120
        non_default_domain_participant_resource_limits.ignored_entity_replacement_kind = dds.IgnoredEntityReplacementKind.NOT_ALIVE_FIRST
        non_default_domain_participant_resource_limits.remote_topic_query_allocation = dds.AllocationSettings(
            10, dds.LENGTH_UNLIMITED, -1)
        non_default_domain_participant_resource_limits.remote_topic_query_hash_buckets = 10
        non_default_domain_participant_resource_limits.writer_data_tag_list_max_length = 5
        non_default_domain_participant_resource_limits.writer_data_tag_string_max_length = 5
        non_default_domain_participant_resource_limits.reader_data_tag_list_max_length = 5
        non_default_domain_participant_resource_limits.reader_data_tag_string_max_length = 5
        non_default_domain_participant_resource_limits.shmem_ref_transfer_mode_max_segments = 5000

        return QosPolicy(
            dds.DomainParticipantResourceLimits,
            {Entity.DomainParticipant: default_domain_participant_resource_limits},
            {Entity.DomainParticipant: non_default_domain_participant_resource_limits},
            "resource_limits")

    elif policy is dds.Durability:

        default_durability = dds.Durability()
        default_durability.kind = dds.DurabilityKind.VOLATILE
        default_durability.direct_communication = True
        default_durability.writer_depth = dds.Durability.AUTO_WRITER_DEPTH

        non_default_durability = dds.Durability(
            dds.DurabilityKind.TRANSIENT_LOCAL)
        non_default_durability.direct_communication = False
        non_default_durability.writer_depth = 10

        return QosPolicy(
            dds.Durability,
            {Entity.Topic: default_durability, Entity.DataWriter: default_durability,
             Entity.DataReader: default_durability},
            {Entity.Topic: non_default_durability, Entity.DataWriter: non_default_durability,
             Entity.DataReader: non_default_durability},
            "durability")

    elif policy is dds.DurabilityService:
        default_durability_service = dds.DurabilityService()
        default_durability_service.service_cleanup_delay = dds.Duration(0)
        default_durability_service.history_kind = dds.HistoryKind.KEEP_LAST
        default_durability_service.history_depth = 1
        default_durability_service.max_samples = dds.LENGTH_UNLIMITED
        default_durability_service.max_instances = dds.LENGTH_UNLIMITED
        default_durability_service.max_samples_per_instance = dds.LENGTH_UNLIMITED

        non_default_durability_service = dds.DurabilityService(
            dds.Duration(10),
            dds.HistoryKind.KEEP_ALL,
            10,
            10,
            10,
            10)

        return QosPolicy(
            dds.DurabilityService,
            {Entity.Topic: default_durability_service,
             Entity.DataWriter: default_durability_service},
            {Entity.Topic: non_default_durability_service,
             Entity.DataWriter: non_default_durability_service},
            "durability_service")

    elif policy is dds.EntityFactory:

        default_entity_factory = dds.EntityFactory()
        default_entity_factory.autoenable_created_entities = True

        non_default_entity_factory = dds.EntityFactory(False)

        return QosPolicy(
            dds.EntityFactory,
            {Entity.DomainParticipantFactory: default_entity_factory,
             Entity.DomainParticipant: default_entity_factory,
             Entity.Publisher: default_entity_factory,
             Entity.Subscriber: default_entity_factory},
            {Entity.DomainParticipantFactory: non_default_entity_factory,
             Entity.DomainParticipant: non_default_entity_factory,
             Entity.Publisher: non_default_entity_factory,
             Entity.Subscriber: non_default_entity_factory},
            "entity_factory")

    elif policy is dds.EntityName:
        default_entity_name = dds.EntityName()
        default_entity_name.name = None
        default_entity_name.role_name = None

        non_default_entity_name = dds.EntityName("non_default_entity_name")
        non_default_entity_name.role_name = "non_default_entity_name"

        return QosPolicy(
            dds.EntityName,
            {Entity.Publisher: default_entity_name,
             Entity.Subscriber: default_entity_name,
             Entity.DataWriter: default_entity_name,
             Entity.DataReader: default_entity_name},
            {Entity.Publisher: non_default_entity_name,
             Entity.Subscriber: non_default_entity_name,
             Entity.DataWriter: non_default_entity_name,
             Entity.DataReader: non_default_entity_name},
            "entity_name")

    elif policy == "ParticipantName":
        default_entity_name = dds.EntityName()
        default_entity_name.name = None
        default_entity_name.role_name = None

        non_default_entity_name = dds.EntityName()
        non_default_entity_name.name = "non_default_entity_name"
        non_default_entity_name.role_name = "non_default_entity_name"
        return QosPolicy(
            dds.EntityName,
            {Entity.DomainParticipant: default_entity_name},
            {Entity.DomainParticipant: non_default_entity_name},
            "participant_name")

    elif policy is dds.Event:
        default_event = dds.Event()
        default_event.thread = dds.DomainParticipant.default_participant_qos.event.thread
        default_event.initial_count = 256
        default_event.max_count = dds.LENGTH_UNLIMITED

        non_default_thread = dds.ThreadSettings()
        non_default_thread.mask = dds.ThreadSettingsKindMask.REALTIME_PRIORITY
        non_default_event = dds.Event(non_default_thread, 10, 10)

        return QosPolicy(
            dds.Event,
            {Entity.DomainParticipant: default_event},
            {Entity.DomainParticipant: non_default_event},
            "event")

    elif policy is dds.GroupData:

        default_group_data = dds.GroupData()
        default_group_data.value = []

        non_default_group_data = dds.GroupData([1, 2, 3, 4, 5])

        return QosPolicy(
            dds.GroupData,
            {Entity.Publisher: default_group_data,
             Entity.Subscriber: default_group_data},
            {Entity.Publisher: non_default_group_data,
             Entity.Subscriber: non_default_group_data},
            "group_data")

    elif policy is dds.History:

        default_history = dds.History()
        default_history.kind = dds.HistoryKind.KEEP_LAST
        default_history.depth = 1

        non_default_history = dds.History(dds.HistoryKind.KEEP_ALL, 10)

        return QosPolicy(
            dds.History,
            {Entity.Topic: default_history,
             Entity.DataWriter: default_history,
             Entity.DataReader: default_history},
            {Entity.Topic: non_default_history,
             Entity.DataWriter: non_default_history,
             Entity.DataReader: non_default_history},
            "history")

    elif policy is dds.LatencyBudget:

        default_latency_budget = dds.LatencyBudget()
        default_latency_budget.duration = dds.Duration(0)

        non_default_latency_budget = dds.LatencyBudget(dds.Duration(10))

        return QosPolicy(
            dds.LatencyBudget,
            {Entity.Topic: default_latency_budget,
             Entity.DataWriter: default_latency_budget,
             Entity.DataReader: default_latency_budget},
            {Entity.Topic: non_default_latency_budget,
             Entity.DataWriter: non_default_latency_budget,
             Entity.DataReader: non_default_latency_budget},
            "latency_budget")

    elif policy is dds.Lifespan:

        default_lifespan = dds.Lifespan()
        default_lifespan.duration = dds.Duration.infinite

        non_default_lifespan = dds.Lifespan(dds.Duration(10))

        return QosPolicy(
            dds.Lifespan,
            {Entity.DataWriter: default_lifespan,
             Entity.Topic: default_lifespan},
            {Entity.DataWriter: non_default_lifespan,
             Entity.Topic: non_default_lifespan},
            "lifespan")

    elif policy is dds.Liveliness:

        default_liveliness = dds.Liveliness()
        default_liveliness.kind = dds.LivelinessKind.AUTOMATIC
        default_liveliness.lease_duration = dds.Duration.infinite
        default_liveliness.assertions_per_lease_duration = 3

        non_default_liveliness = dds.Liveliness(
            dds.LivelinessKind.MANUAL_BY_PARTICIPANT, dds.Duration(10))
        non_default_liveliness.assertions_per_lease_duration = 10

        return QosPolicy(
            dds.Liveliness,
            {Entity.Topic: default_liveliness,
             Entity.DataWriter: default_liveliness,
             Entity.DataReader: default_liveliness},
            {Entity.Topic: non_default_liveliness,
             Entity.DataWriter: non_default_liveliness,
             Entity.DataReader: non_default_liveliness},
            "liveliness")

    elif policy is dds.LocatorFilter:
        raise Exception("LocatorFilter is not tested in generic tests")

    elif policy == "Logging":
        # Logging Qos is not exposed in Python. Use dds.Logger to set logging
        raise Exception("Logging is not tested in generic tests")

    elif policy is dds.MultiChannel:

        default_multi_channel = dds.MultiChannel()
        default_multi_channel.channels = []
        default_multi_channel.filter_name = dds.MultiChannel.STRINGMATCH_FILTER_NAME

        non_default_multi_channel = dds.MultiChannel(
            [dds.ChannelSettings(dds.TransportMulticastSettingsSeq(), "", 0)],
            dds.MultiChannel.SQL_FILTER_NAME)

        return QosPolicy(
            dds.MultiChannel,
            {Entity.DataWriter: default_multi_channel},
            {Entity.DataWriter: non_default_multi_channel},
            "multi_channel")

    elif policy is dds.Ownership:

        default_ownership = dds.Ownership()
        default_ownership.kind = dds.OwnershipKind.SHARED

        non_default_ownership = dds.Ownership(dds.OwnershipKind.EXCLUSIVE)

        return QosPolicy(
            dds.Ownership,
            {Entity.Topic: default_ownership,
             Entity.DataWriter: default_ownership,
             Entity.DataReader: default_ownership},
            {Entity.Topic: non_default_ownership,
             Entity.DataWriter: non_default_ownership,
             Entity.DataReader: non_default_ownership},
            "ownership")

    elif policy is dds.OwnershipStrength:

        default_ownership_strength = dds.OwnershipStrength()
        default_ownership_strength.value = 0

        non_default_ownership_strength = dds.OwnershipStrength(10)

        return QosPolicy(
            dds.OwnershipStrength,
            {Entity.DataWriter: default_ownership_strength},
            {Entity.DataWriter: non_default_ownership_strength},
            "ownership_strength")

    elif policy is dds.Partition:

        default_partition = dds.Partition()
        default_partition.name = []

        non_default_partition = dds.Partition("partition")
        non_default_partition = dds.Partition(["partition"])

        return QosPolicy(
            dds.Partition,
            {Entity.DomainParticipant: default_partition,
             Entity.Publisher: default_partition,
             Entity.Subscriber: default_partition},
            {Entity.DomainParticipant: non_default_partition,
             Entity.Publisher: non_default_partition,
             Entity.Subscriber: non_default_partition},
            "partition")

    elif policy is dds.Presentation:

        default_presentation = dds.Presentation()
        default_presentation.access_scope = dds.PresentationAccessScopeKind.INSTANCE
        default_presentation.coherent_access = False
        default_presentation.drop_incomplete_coherent_set = True
        default_presentation.ordered_access = False

        non_default_presentation = dds.Presentation(
            dds.PresentationAccessScopeKind.HIGHEST_OFFERED,
            True,
            True)
        non_default_presentation.drop_incomplete_coherent_set = False

        return QosPolicy(
            dds.Presentation,
            {Entity.Publisher: default_presentation,
             Entity.Subscriber: default_presentation},
            {Entity.Publisher: non_default_presentation,
             Entity.Subscriber: non_default_presentation},
            "presentation")

    elif policy == "Profile":
        # Does not exist in python API, exposed via dds.QosProvider
        raise Exception("Profile does not exist in python")
    elif policy is dds.Property:
        raise Exception("Property is not tested in generic tests")

    elif policy is dds.PublishMode:
        default_publish_mode = dds.PublishMode()
        default_publish_mode.kind = dds.PublishModeKind.SYNCHRONOUS
        default_publish_mode.flow_controller_name = dds.FlowController.DEFAULT_FLOW_CONTROLLER_NAME
        default_publish_mode.priority = dds.PublishMode.PUBLICATION_PRIORITY_UNDEFINED

        non_default_publish_mode = dds.PublishMode()
        non_default_publish_mode.kind = dds.PublishModeKind.ASYNCHRONOUS
        non_default_publish_mode.flow_controller_name = "flow_controller"
        non_default_publish_mode.priority = 10

        return QosPolicy(
            dds.PublishMode,
            {Entity.DataWriter: default_publish_mode},
            {Entity.DataWriter: non_default_publish_mode},
            "publish_mode")

    elif policy is dds.ReaderDataLifecycle:

        default_reader_data_lifecycle = dds.ReaderDataLifecycle()
        default_reader_data_lifecycle.autopurge_nowriter_samples_delay = dds.Duration.infinite
        default_reader_data_lifecycle.autopurge_disposed_samples_delay = dds.Duration.infinite
        default_reader_data_lifecycle.autopurge_disposed_instances_delay = dds.Duration.infinite
        default_reader_data_lifecycle.autopurge_nowriter_instances_delay = dds.Duration.zero

        ten_sec = dds.Duration(10)
        non_default_reader_data_lifecycle = dds.ReaderDataLifecycle(
            ten_sec, ten_sec)
        non_default_reader_data_lifecycle.autopurge_disposed_instances_delay = dds.Duration(
            10, 0)
        non_default_reader_data_lifecycle.autopurge_nowriter_instances_delay = dds.Duration(
            10, 0)

        return QosPolicy(
            dds.ReaderDataLifecycle,
            {Entity.DataReader: default_reader_data_lifecycle},
            {Entity.DataReader: non_default_reader_data_lifecycle},
            "reader_data_lifecycle")

    elif policy is dds.ReceiverPool:

        default_receiver_pool = dds.ReceiverPool()
        default_receiver_pool.thread = dds.DomainParticipant.default_participant_qos.receiver_pool.thread
        default_receiver_pool.buffer_size = dds.ReceiverPool.LENGTH_AUTO
        default_receiver_pool.buffer_alignment = 16

        non_default_thread = dds.ThreadSettings()
        non_default_thread.mask = dds.ThreadSettingsKindMask.FLOATING_POINT
        non_default_receiver_pool = dds.ReceiverPool(
            non_default_thread, 1024, 8)

        return QosPolicy(
            dds.ReceiverPool,
            {Entity.DomainParticipant: default_receiver_pool},
            {Entity.DomainParticipant: non_default_receiver_pool},
            "receiver_pool")

    elif policy is dds.Reliability:

        default_reliability_reader_topic = dds.Reliability()
        default_reliability_reader_topic.kind = dds.ReliabilityKind.BEST_EFFORT
        default_reliability_reader_topic.acknowledgment_kind = dds.AcknowledgmentKind.PROTOCOL
        default_reliability_reader_topic.max_blocking_time = dds.Duration.from_milliseconds(
            100)
        default_reliability_reader_topic.instance_state_recovery_kind = dds.InstanceStateRecoveryKind.NONE

        default_reliability_writer = dds.Reliability()
        default_reliability_writer.kind = dds.ReliabilityKind.RELIABLE
        default_reliability_writer.acknowledgment_kind = dds.AcknowledgmentKind.PROTOCOL
        default_reliability_writer.max_blocking_time = dds.Duration.from_milliseconds(
            100)
        default_reliability_writer.instance_state_recovery_kind = dds.InstanceStateRecoveryKind.NONE

        non_default_reliability_reader_topic = dds.Reliability()
        non_default_reliability_reader_topic.kind = dds.ReliabilityKind.RELIABLE
        non_default_reliability_reader_topic.acknowledgment_kind = dds.AcknowledgmentKind.APPLICATION_EXPLICIT
        non_default_reliability_reader_topic.max_blocking_time = dds.Duration.from_milliseconds(
            200)
        non_default_reliability_reader_topic.instance_state_recovery_kind = dds.InstanceStateRecoveryKind.RECOVER_STATE

        non_default_reliability_writer = dds.Reliability()
        non_default_reliability_writer.kind = dds.ReliabilityKind.BEST_EFFORT
        non_default_reliability_writer.acknowledgment_kind = dds.AcknowledgmentKind.APPLICATION_ORDERED
        non_default_reliability_writer.max_blocking_time = dds.Duration.from_milliseconds(
            200)
        non_default_reliability_writer.instance_state_recovery_kind = dds.InstanceStateRecoveryKind.RECOVER_STATE

        return QosPolicy(
            dds.Reliability,
            {Entity.DataReader: default_reliability_reader_topic,
                Entity.Topic: default_reliability_reader_topic,
                Entity.DataWriter: default_reliability_writer},
            {Entity.DataReader: non_default_reliability_reader_topic,
                Entity.Topic: non_default_reliability_reader_topic,
                Entity.DataWriter: non_default_reliability_writer},
            "reliability")

    elif policy is dds.ResourceLimits:

        default_resource_limits = dds.ResourceLimits()
        default_resource_limits.max_samples = dds.LENGTH_UNLIMITED
        default_resource_limits.max_instances = dds.LENGTH_UNLIMITED
        default_resource_limits.max_samples_per_instance = dds.LENGTH_UNLIMITED
        default_resource_limits.initial_samples = 32
        default_resource_limits.initial_instances = 32
        default_resource_limits.instance_hash_buckets = 1

        non_default_resource_limits = dds.ResourceLimits(100, 100, 100)
        non_default_resource_limits.initial_samples = 10
        non_default_resource_limits.initial_instances = 10
        non_default_resource_limits.instance_hash_buckets = 10

        return QosPolicy(
            dds.ResourceLimits,
            {Entity.Topic: default_resource_limits,
             Entity.DataReader: default_resource_limits,
             Entity.DataWriter: default_resource_limits},
            {Entity.Topic: non_default_resource_limits,
             Entity.DataReader: non_default_resource_limits,
             Entity.DataWriter: non_default_resource_limits},
            "resource_limits")

    elif policy is dds.Service:

        default_service = dds.Service()
        default_service.kind = dds.ServiceKind.NO_SERVICE

        non_default_service = dds.Service(dds.ServiceKind.DATABASE_INTEGRATION)

        return QosPolicy(
            dds.Service,
            {Entity.DomainParticipant: default_service,
             Entity.DataReader: default_service,
             Entity.DataWriter: default_service},
            {Entity.DomainParticipant: non_default_service,
             Entity.DataReader: non_default_service,
             Entity.DataWriter: non_default_service},
            "service")

    elif policy is dds.SystemResourceLimits:

        default_system_resource_limits = dds.SystemResourceLimits()
        default_system_resource_limits.max_objects_per_thread = 261120
        default_system_resource_limits.initial_objects_per_thread = 1024

        non_default_system_resource_limits = dds.SystemResourceLimits(1024)
        non_default_system_resource_limits.initial_objects_per_thread = 100

        return QosPolicy(
            dds.SystemResourceLimits,
            {Entity.DomainParticipantFactory: default_system_resource_limits},
            {Entity.DomainParticipantFactory: non_default_system_resource_limits},
            "system_resource_limits")

    elif policy is dds.TimeBasedFilter:

        default_time_based_filter = dds.TimeBasedFilter()
        default_time_based_filter.minimum_separation = dds.Duration.zero

        non_default_time_based_filter = dds.TimeBasedFilter(
            dds.Duration(10, 0))

        return QosPolicy(
            dds.TimeBasedFilter,
            {Entity.DataReader: default_time_based_filter},
            {Entity.DataReader: non_default_time_based_filter},
            "time_based_filter")

    elif policy is dds.TopicData:

        default_topic_data = dds.TopicData()
        default_topic_data.value = []

        non_default_topic_data = dds.TopicData()
        non_default_topic_data.value = [1, 2, 3, 4, 5]

        return QosPolicy(
            dds.TopicData,
            {Entity.Topic: default_topic_data},
            {Entity.Topic: non_default_topic_data},
            "topic_data")

    elif policy is dds.TopicQueryDispatch:

        default_topic_query_dispatch = dds.TopicQueryDispatch()
        default_topic_query_dispatch.enable = False
        default_topic_query_dispatch.publication_period = dds.Duration(1)
        default_topic_query_dispatch.samples_per_period = dds.LENGTH_UNLIMITED

        non_default_topic_query_dispatch = dds.TopicQueryDispatch(
            True, dds.Duration(2), 100)

        return QosPolicy(
            dds.TopicQueryDispatch,
            {Entity.DataWriter: default_topic_query_dispatch},
            {Entity.DataWriter: non_default_topic_query_dispatch},
            "topic_query_dispatch")

    elif policy is dds.TransportBuiltin:

        default_transport_builtin = dds.TransportBuiltin()
        default_transport_builtin.mask = (
            dds.TransportBuiltinMask.UDPv4 | dds.TransportBuiltinMask.SHMEM)

        non_default_transport_builtin = dds.TransportBuiltin(
            dds.TransportBuiltinMask.SHMEM)

        return QosPolicy(
            dds.TransportBuiltin,
            {Entity.DomainParticipant: default_transport_builtin},
            {Entity.DomainParticipant: non_default_transport_builtin},
            "transport_builtin")

    elif policy is dds.TransportMulticast:

        default_transport_multicast = dds.TransportMulticast()
        default_transport_multicast.value = []
        default_transport_multicast.kind = dds.TransportMulticastKind.AUTOMATIC

        non_default_transport_multicast = dds.TransportMulticast([
            dds.TransportMulticastSettings([], "", 0)],
            dds.TransportMulticastKind.UNICAST)

        return QosPolicy(
            dds.TransportMulticast,
            {Entity.DataReader: default_transport_multicast},
            {Entity.DataReader: non_default_transport_multicast},
            "transport_multicast")

    elif policy is dds.TransportMulticastMapping:

        default_transport_multicast_mapping = dds.TransportMulticastMapping()
        default_transport_multicast_mapping.value = []

        non_default_transport_multicast_mapping = dds.TransportMulticastMapping([
            dds.MulticastMapping()])

        return QosPolicy(
            dds.TransportMulticastMapping,
            {Entity.DomainParticipant: default_transport_multicast_mapping},
            {Entity.DomainParticipant: non_default_transport_multicast_mapping},
            "transport_multicast_mapping")

    elif policy is dds.TransportPriority:

        default_transport_priority = dds.TransportPriority()
        default_transport_priority.value = 0

        non_default_transport_priority = dds.TransportPriority(10)

        return QosPolicy(
            dds.TransportPriority,
            {Entity.Topic: default_transport_priority,
             Entity.DataReader: default_transport_priority,
             Entity.DataWriter: default_transport_priority},
            {Entity.Topic: non_default_transport_priority,
             Entity.DataReader: non_default_transport_priority,
             Entity.DataWriter: non_default_transport_priority},
            "transport_priority")

    elif policy is dds.TransportSelection:

        default_transport_selection = dds.TransportSelection()
        default_transport_selection.value = []

        non_default_transport_selection = dds.TransportSelection(["a"])

        return QosPolicy(
            dds.TransportSelection,
            {Entity.DataReader: default_transport_selection,
             Entity.DataWriter: default_transport_selection},
            {Entity.DataReader: non_default_transport_selection,
             Entity.DataWriter: non_default_transport_selection},
            "transport_selection")

    elif policy is dds.TransportUnicast:

        default_transport_unicast = dds.TransportUnicast()
        default_transport_unicast.value = []

        non_default_transport_unicast = dds.TransportUnicast(
            [dds.TransportUnicastSettings()])

        return QosPolicy(
            dds.TransportUnicast,
            {Entity.DataReader: default_transport_unicast,
             Entity.DataWriter: default_transport_unicast},
            {Entity.DataReader: non_default_transport_unicast,
             Entity.DataWriter: non_default_transport_unicast},
            "transport_unicast")

    elif policy == "DefaultUnicast":
        default_transport_unicast = dds.TransportUnicast()
        default_transport_unicast.value = []

        non_default_transport_unicast = dds.TransportUnicast(
            [dds.TransportUnicastSettings()])

        return QosPolicy(
            dds.TransportUnicast,
            {Entity.DomainParticipant: default_transport_unicast},
            {Entity.DomainParticipant: non_default_transport_unicast},
            "default_unicast")

    elif policy is dds.TypeConsistencyEnforcement:

        default_type_consistency_enforcement = dds.TypeConsistencyEnforcement()
        default_type_consistency_enforcement.kind = dds.TypeConsistencyEnforcementKind.AUTO_TYPE_COERCION
        default_type_consistency_enforcement.ignore_sequence_bounds = True
        default_type_consistency_enforcement.ignore_string_bounds = True
        default_type_consistency_enforcement.ignore_member_names = False
        default_type_consistency_enforcement.prevent_type_widening = False
        default_type_consistency_enforcement.force_type_validation = False
        default_type_consistency_enforcement.ignore_enum_literal_names = False

        non_default_type_consistency_enforcement = dds.TypeConsistencyEnforcement(
            dds.TypeConsistencyEnforcementKind.ALLOW_TYPE_COERCION)
        non_default_type_consistency_enforcement.ignore_sequence_bounds = False
        non_default_type_consistency_enforcement.ignore_string_bounds = False
        non_default_type_consistency_enforcement.ignore_member_names = True
        non_default_type_consistency_enforcement.prevent_type_widening = True
        non_default_type_consistency_enforcement.force_type_validation = True
        non_default_type_consistency_enforcement.ignore_enum_literal_names = True

        return QosPolicy(
            dds.TypeConsistencyEnforcement,
            {Entity.DataReader: default_type_consistency_enforcement},
            {Entity.DataReader: non_default_type_consistency_enforcement},
            "type_consistency_enforcement")

    elif policy is dds.UserData:

        default_user_data = dds.UserData()
        default_user_data.value = []

        non_default_user_data = dds.UserData([1, 2, 3])

        return QosPolicy(
            dds.UserData,
            {Entity.DomainParticipant: default_user_data,
             Entity.DataReader: default_user_data,
             Entity.DataWriter: default_user_data},
            {Entity.DomainParticipant: non_default_user_data,
             Entity.DataReader: non_default_user_data,
             Entity.DataWriter: non_default_user_data},
            "user_data")

    elif policy is dds.WireProtocol:

        default_wire_protocol = dds.WireProtocol()
        default_wire_protocol.participant_id = -1
        default_wire_protocol.rtps_host_id = dds.WireProtocol.RTPS_AUTO_ID
        default_wire_protocol.rtps_app_id = dds.WireProtocol.RTPS_AUTO_ID
        default_wire_protocol.rtps_instance_id = dds.WireProtocol.RTPS_AUTO_ID
        default_wire_protocol.rtps_well_known_ports = dds.RtpsWellKnownPorts.interoperable
        default_wire_protocol.rtps_reserved_port_mask = dds.RtpsReservedPortKindMask.DEFAULT_MASK
        default_wire_protocol.rtps_auto_id_kind = dds.WireProtocolAutoKind.RTPS_AUTO_ID_FROM_UUID
        default_wire_protocol.compute_crc = False
        default_wire_protocol.check_crc = False

        non_default_wire_protocol = dds.WireProtocol()
        non_default_wire_protocol.participant_id = 1
        non_default_wire_protocol.rtps_host_id = 2
        non_default_wire_protocol.rtps_app_id = 3
        non_default_wire_protocol.rtps_instance_id = 4
        non_default_wire_protocol.rtps_well_known_ports = dds.RtpsWellKnownPorts.backwards_compatible
        non_default_wire_protocol.rtps_reserved_port_mask = dds.RtpsReservedPortKindMask.NONE
        non_default_wire_protocol.rtps_auto_id_kind = dds.WireProtocolAutoKind.RTPS_AUTO_ID_FROM_MAC
        non_default_wire_protocol.compute_crc = True
        non_default_wire_protocol.check_crc = True

        return QosPolicy(
            dds.WireProtocol,
            {Entity.DomainParticipant: default_wire_protocol},
            {Entity.DomainParticipant: non_default_wire_protocol},
            "wire_protocol")

    elif policy is dds.WriterDataLifecycle:

        default_writer_data_lifecycle = dds.WriterDataLifecycle()
        default_writer_data_lifecycle.autodispose_unregistered_instances = False
        default_writer_data_lifecycle.autopurge_unregistered_instances_delay = dds.Duration.infinite
        default_writer_data_lifecycle.autopurge_disposed_instances_delay = dds.Duration.infinite

        non_default_writer_data_lifecycle = dds.WriterDataLifecycle(True)
        non_default_writer_data_lifecycle.autopurge_unregistered_instances_delay = dds.Duration(
            1, 0)
        non_default_writer_data_lifecycle.autopurge_disposed_instances_delay = dds.Duration(
            2, 0)

        return QosPolicy(
            dds.WriterDataLifecycle,
            {Entity.DataWriter: default_writer_data_lifecycle},
            {Entity.DataWriter: non_default_writer_data_lifecycle},
            "writer_data_lifecycle")


# Note: If you want to test a policy that doesn't have a class associated with
# it's name, use a string. For example, ParticipantName is really just
# an entity name so we pass it as a string
policies = [dds.AsynchronousPublisher,
            dds.Availability,
            dds.Batch,
            dds.Database,
            dds.DataReaderProtocol,
            dds.DataWriterProtocol,
            dds.DataReaderResourceLimits,
            dds.DataWriterResourceLimits,
            dds.DataRepresentation,
            dds.Deadline,
            dds.DestinationOrder,
            dds.Discovery,
            dds.DiscoveryConfig,
            dds.DomainParticipantResourceLimits,
            dds.Durability,
            dds.DurabilityService,
            dds.EntityFactory,
            dds.EntityName,
            "ParticipantName",
            dds.Event,
            dds.GroupData,
            dds.History,
            dds.LatencyBudget,
            dds.Lifespan,
            dds.Liveliness,
            dds.MultiChannel,
            dds.Ownership,
            dds.OwnershipStrength,
            dds.Partition,
            dds.Presentation,
            dds.PublishMode,
            dds.ReaderDataLifecycle,
            dds.ReceiverPool,
            dds.Reliability,
            dds.ResourceLimits,
            dds.Service,
            dds.SystemResourceLimits,
            dds.TimeBasedFilter,
            dds.TopicData,
            dds.TopicQueryDispatch,
            dds.TransportBuiltin,
            dds.TransportMulticast,
            dds.TransportMulticastMapping,
            dds.TransportPriority,
            dds.TransportSelection,
            dds.TransportUnicast,
            "DefaultUnicast",
            dds.TypeConsistencyEnforcement,
            dds.UserData,
            dds.WireProtocol,
            dds.WriterDataLifecycle]


@pytest.mark.parametrize("policy", policies)
def test_qos_policy_default_value(policy):

    qos_policy = get_default_qos_policies_from_policy(policy)
    default_values = qos_policy.default_values
    field_name = qos_policy.field_name

    if qos_policy.policy_defined_for_entity(Entity.DomainParticipantFactory):
        domain_participant_factory_qos = dds.DomainParticipantFactoryQos()
        assert_structures_equal(getattr(domain_participant_factory_qos,
                                field_name), default_values[Entity.DomainParticipantFactory])
        assert getattr(domain_participant_factory_qos,
                       field_name) == default_values[Entity.DomainParticipantFactory]
        assert not getattr(domain_participant_factory_qos,
                           field_name) != default_values[Entity.DomainParticipantFactory]

    if qos_policy.policy_defined_for_entity(Entity.DomainParticipant):
        domain_participant_qos = dds.DomainParticipantQos()

        assert_structures_equal(
            getattr(
                domain_participant_qos,
                field_name),
            default_values[Entity.DomainParticipant])
        if field_name != 'discovery_config':
            # Skip discovery_config because of the inconsistency in the init
            # functions of DiscoveryConfig and
            # DomainParticipantQos.DiscoveryConfig
            assert getattr(domain_participant_qos,
                           field_name) == default_values[Entity.DomainParticipant]
            assert not getattr(domain_participant_qos,
                               field_name) != default_values[Entity.DomainParticipant]

    if qos_policy.policy_defined_for_entity(Entity.Publisher):
        publisher_qos = dds.PublisherQos()
        assert_structures_equal(getattr(publisher_qos, field_name),
                                default_values[Entity.Publisher])
        assert getattr(
            publisher_qos, field_name) == default_values[Entity.Publisher]
        assert not getattr(
            publisher_qos, field_name) != default_values[Entity.Publisher]

    if qos_policy.policy_defined_for_entity(Entity.Subscriber):
        subscriber_qos = dds.SubscriberQos()
        assert_structures_equal(getattr(subscriber_qos, field_name),
                                default_values[Entity.Subscriber])
        assert getattr(
            subscriber_qos, field_name) == default_values[Entity.Subscriber]
        assert not getattr(
            subscriber_qos, field_name) != default_values[Entity.Subscriber]

    if qos_policy.policy_defined_for_entity(Entity.Topic):
        topic_qos = dds.TopicQos()
        assert_structures_equal(getattr(topic_qos, field_name),
                                default_values[Entity.Topic])
        assert getattr(topic_qos, field_name) == default_values[Entity.Topic]
        assert not getattr(
            topic_qos, field_name) != default_values[Entity.Topic]

    if qos_policy.policy_defined_for_entity(Entity.DataReader):
        data_reader_qos = dds.DataReaderQos()
        assert_structures_equal(getattr(data_reader_qos, field_name),
                                default_values[Entity.DataReader])
        assert getattr(data_reader_qos,
                       field_name) == default_values[Entity.DataReader]
        assert not getattr(
            data_reader_qos, field_name) != default_values[Entity.DataReader]

    if qos_policy.policy_defined_for_entity(Entity.DataWriter):
        data_writer_qos = dds.DataWriterQos()
        assert_structures_equal(getattr(data_writer_qos, field_name),
                                default_values[Entity.DataWriter])
        assert getattr(data_writer_qos,
                       field_name) == default_values[Entity.DataWriter]
        assert not getattr(
            data_writer_qos, field_name) != default_values[Entity.DataWriter]


def check_no_fields_equal_for_entity(entity, policy, default_values, non_default_values):
    if policy.policy_defined_for_entity(entity):
        assert_no_field_equal_in_structures(
            default_values[entity], non_default_values[entity])


@pytest.mark.parametrize("policy", policies)
def test_qos_policy_can_have_other_value_set(policy):
    # This test can fail for a few reasons
    # 1. The field in the policy isn't added in the default/non_default list above
    # 2. The default/non_default values are accidentally equal
    # 3. A static property was added to the QoS and not added to the blacklist above
    qos_policy = get_default_qos_policies_from_policy(policy)
    default_values = qos_policy.default_values
    non_default_values = qos_policy.non_default_values

    check_no_fields_equal_for_entity(
        Entity.DomainParticipantFactory, qos_policy, default_values, non_default_values)
    check_no_fields_equal_for_entity(
        Entity.DomainParticipant, qos_policy, default_values, non_default_values)
    check_no_fields_equal_for_entity(
        Entity.Publisher, qos_policy, default_values, non_default_values)
    check_no_fields_equal_for_entity(
        Entity.Subscriber, qos_policy, default_values, non_default_values)
    check_no_fields_equal_for_entity(
        Entity.Topic, qos_policy, default_values, non_default_values)
    check_no_fields_equal_for_entity(
        Entity.DataReader, qos_policy, default_values, non_default_values)
    check_no_fields_equal_for_entity(
        Entity.DataWriter, qos_policy, default_values, non_default_values)


def test_data_tag():
    dt1 = dds.DataTag()
    dt2 = dds.DataTag([("key1", "value1")])
    dt3 = dds.DataTag({"key1": "value1"})
    assert dt1 != dt2
    assert dt2 == dt3
    dt2.set([("key2", "value2")])
    dt3.set([("key1", "value2")])
    assert "key1" in dt3
    assert "key2" in dt2
    assert dt2["key2"] == "value2"
    assert dt3["key1"] == "value2"
    dt1["key1"] = "value1"
    assert dt1.get("key1") == "value1"
    assert len(dt2) == 2
    assert len(dt2) == dt2.size()
    dt2.remove("key2")
    assert len(dt2) == 1
    assert dt2.get("key1") == "value1"
    with pytest.raises(dds.PreconditionNotMetError):
        dt1.get("fake_key")
    assert dt1.try_get("fake_key") == None
    assert dt1.exists("key1")
    dt1.set("key3", "value3")
    dt1.set({"key2": "value2", "key4": "value4"})
    dt1.set(("key5", "value5"))
    assert len(dt1) == 5
    assert len(dt1.get_all()) == 5
    assert dt1.get_all()["key1"] == "value1"
    assert dt1.get_all()["key2"] == "value2"
    assert dt1.get_all()["key3"] == "value3"
    assert dt1.get_all()["key4"] == "value4"
    assert dt1.get_all()["key5"] == "value5"


def test_locator_filter():
    l = dds.Locator(dds.LocatorKind.UDPv4, 10, dds.Uint8Seq([1, 1, 1, 1]))
    lfe = dds.LocatorFilterElement("a", [l])
    lf1 = dds.LocatorFilter()
    lf2 = dds.LocatorFilter([lfe])
    assert not lf1 == lf2
    assert lf1 != lf2
    assert lf1.locator_filters == []
    assert lf2.locator_filters == [lfe]
    lf1.locator_filters = [lfe]
    assert lf1 == lf2
    assert lf1.filter_name == dds.Filter.STRINGMATCH_FILTER_NAME
    lf1.filter_name = dds.Filter.SQL_FILTER_NAME
    assert lf1.filter_name == dds.Filter.SQL_FILTER_NAME


def test_property():
    p1 = dds.Property()
    p2 = dds.Property([("key1", "value1")])
    p3 = dds.Property({"key1": "value1"})
    p4 = dds.Property([("key1", "value1"), ("key2", "value2")], True)
    assert p1 != p2
    assert p2 == p3
    p2.set([("key2", "value2")])
    p3.set([("key1", "value2")])
    assert "key1" in p3
    assert "key2" in p2
    assert p2["key2"] == "value2"
    assert p3["key1"] == "value2"
    p1["key1"] = "value1"
    assert p1.get("key1") == "value1"
    assert len(p2) == 2
    assert len(p2) == p2.size()
    p2.remove("key2")
    assert len(p2) == 1
    assert p2.get("key1") == "value1"
    with pytest.raises(dds.PreconditionNotMetError):
        p1.get("fake_key")
    assert p1.try_get("fake_key") == None
    assert p1.exists("key1")
    p1.set("key3", "value3")
    p1.set({"key2": "value2", "key4": "value4"})
    p1.set(("key5", "value5"))
    assert len(p1) == 5
    assert len(p1.get_all()) == 5
    assert p1.get_all()["key1"] == "value1"
    assert p1.get_all()["key2"] == "value2"
    assert p1.get_all()["key3"] == "value3"
    assert p1.get_all()["key4"] == "value4"
    assert p1.get_all()["key5"] == "value5"
    assert p4.propagate("key1")
    p4.set("key2", "value2", True)
    p4["key3"] = ("value3", False)
    assert p4.propagate("key2")
    assert not p4.propagate("key3")


def test_static_properties():
    # This test is to make sure that the static properties work as expected

    # ---- Asynchronous Publisher ----
    assert dds.AsynchronousPublisher.enabled().disable_asynchronous_batch
    assert not dds.AsynchronousPublisher.enabled(
        True).disable_asynchronous_batch
    assert not dds.AsynchronousPublisher.disabled.disable_asynchronous_batch

    # ---- Batch ----
    assert dds.Batch.enabled.enable
    assert not dds.Batch.disabled.enable
    assert dds.Batch.enabled_with_max_data_bytes(123).max_data_bytes == 123
    assert dds.Batch.enabled_with_max_samples(456).max_samples == 456

    # ---- DataRepresentation ----
    assert dds.DataRepresentation.XCDR == 0
    assert dds.DataRepresentation.XCDR2 == 2
    assert dds.DataRepresentation.XML == 1
    assert dds.DataRepresentation.AUTO_ID == -1

    # ---- Durability ----
    assert dds.Durability.volatile.kind == dds.DurabilityKind.VOLATILE
    assert dds.Durability.transient_local.kind == dds.DurabilityKind.TRANSIENT_LOCAL
    assert dds.Durability.transient.kind == dds.DurabilityKind.TRANSIENT
    assert dds.Durability.persistent.kind == dds.DurabilityKind.PERSISTENT

    # ---- EntityFactory ----
    assert dds.EntityFactory.auto_enable.autoenable_created_entities == True
    assert dds.EntityFactory.manually_enable.autoenable_created_entities == False

    # ---- History ----
    assert dds.History.keep_all.kind == dds.HistoryKind.KEEP_ALL
    assert dds.History.keep_last(19).kind == dds.HistoryKind.KEEP_LAST
    assert dds.History.keep_last(19).depth == 19

    # ---- Liveliness ----
    assert dds.Liveliness.automatic.kind == dds.LivelinessKind.AUTOMATIC
    assert dds.Liveliness.manual_by_participant(
    ).kind == dds.LivelinessKind.MANUAL_BY_PARTICIPANT
    assert dds.Liveliness.manual_by_topic().kind == dds.LivelinessKind.MANUAL_BY_TOPIC

    # ---- Ownership ----
    assert dds.Ownership.shared.kind == dds.OwnershipKind.SHARED
    assert dds.Ownership.exclusive.kind == dds.OwnershipKind.EXCLUSIVE

    # ---- Presentation ----
    assert dds.Presentation.group_access_scope(
        True, True).access_scope == dds.PresentationAccessScopeKind.GROUP
    assert dds.Presentation.instance_access_scope(
        True, True).access_scope == dds.PresentationAccessScopeKind.INSTANCE
    assert dds.Presentation.topic_access_scope(
        True, True).access_scope == dds.PresentationAccessScopeKind.TOPIC

    # ---- PublishMode ----
    assert dds.PublishMode.synchronous.kind == dds.PublishModeKind.SYNCHRONOUS
    assert dds.PublishMode.asynchronous().kind == dds.PublishModeKind.ASYNCHRONOUS
    assert dds.PublishMode.asynchronous("foo").flow_controller_name == "foo"
    assert dds.PublishMode.asynchronous(
        "foo",
        dds.PublishMode.PUBLICATION_PRIORITY_UNDEFINED).priority == dds.PublishMode.PUBLICATION_PRIORITY_UNDEFINED

    # ---- ReaderDataLifecycle ----
    assert dds.ReaderDataLifecycle.no_auto_purge.autopurge_disposed_instances_delay == dds.Duration.infinite
    assert dds.ReaderDataLifecycle.auto_purge_disposed_samples(
        dds.Duration(1, 2)).autopurge_disposed_samples_delay == dds.Duration(1, 2)
    assert dds.ReaderDataLifecycle.auto_purge_no_writer_samples(
        dds.Duration(3, 4)).autopurge_nowriter_samples_delay == dds.Duration(3, 4)

    # ---- Reliability ----
    assert dds.Reliability.best_effort.kind == dds.ReliabilityKind.BEST_EFFORT
    assert dds.Reliability.reliable().kind == dds.ReliabilityKind.RELIABLE
    assert dds.Reliability.reliable(dds.Duration(
        1, 2)).max_blocking_time == dds.Duration(1, 2)

    # ---- TransportBuiltin ----
    assert dds.TransportBuiltin.all.mask == dds.TransportBuiltinMask.ALL
    assert dds.TransportBuiltin.none.mask == dds.TransportBuiltinMask.NONE
    assert dds.TransportBuiltin.shmem.mask == dds.TransportBuiltinMask.SHMEM
    assert dds.TransportBuiltin.udpv4.mask == dds.TransportBuiltinMask.UDPv4
    assert dds.TransportBuiltin.udpv6.mask == dds.TransportBuiltinMask.UDPv6

    # ---- TypeConsistencyEnforcement ----
    assert dds.TypeConsistencyEnforcement.auto_type_coercion.kind == dds.TypeConsistencyEnforcementKind.AUTO_TYPE_COERCION
    assert dds.TypeConsistencyEnforcement.allow_type_coercion.kind == dds.TypeConsistencyEnforcementKind.ALLOW_TYPE_COERCION
    assert dds.TypeConsistencyEnforcement.disallow_type_coercion.kind == dds.TypeConsistencyEnforcementKind.DISALLOW_TYPE_COERCION


def test_data_iterator():
    group_data = dds.GroupData(dds.Uint8Seq([1, 2, 3]))
    assert len(group_data.value) == 3
    for index, value in enumerate(group_data):
        assert value == index + 1

    topic_data = dds.TopicData(dds.Uint8Seq([1, 2, 3]))
    assert len(topic_data.value) == 3
    for index, value in enumerate(topic_data):
        assert value == index + 1

    user_data = dds.UserData(dds.Uint8Seq([1, 2, 3]))
    assert len(user_data.value) == 3
    for index, value in enumerate(user_data):
        assert value == index + 1
