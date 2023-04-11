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
import rti.idl as idl

from test_utils.fixtures import *
from rti.types.builtin import String
import pathlib
from time import sleep


def get_provider() -> dds.QosProvider:
    XML_FILE_PATH = str(pathlib.Path(
        __file__).parent.absolute()) + "/../xml/StatusTestCases.xml"
    provider = dds.QosProvider(XML_FILE_PATH)
    return provider


def get_participant_from_provider(provider: dds.QosProvider, config: str):
    params = dds.DomainParticipantConfigParams()
    params.domain_id = get_test_domain()
    participant = provider.create_participant_from_config(config, params)
    return participant


def enable(participant, reader, writer):
    participant.enable()
    writer.topic.enable()
    participant.implicit_publisher.enable()
    participant.implicit_subscriber.enable()
    reader.enable()
    writer.enable()


def test_publication_matched_listener_triggers():
    participant = create_participant()
    topic = dds.Topic(participant, "TestTopic", String)
    reader = dds.DataReader(participant.implicit_subscriber, topic)
    saved_event_writer = None
    saved_status = None

    class PublicationMatchedListener(dds.NoOpDataWriterListener):
        def on_publication_matched(self, writer, status):
            nonlocal saved_event_writer
            nonlocal saved_status
            saved_event_writer = writer
            saved_status = status
    writer = dds.DataWriter(participant.implicit_publisher,
                            topic,
                            dds.DataWriterQos(),
                            PublicationMatchedListener())

    wait.until(lambda: saved_status is not None)
    assert saved_event_writer == writer
    assert writer.publication_matched_status.current_count == 1
    assert reader.instance_handle == saved_status.last_subscription_handle
    participant.close()


def test_offered_incompatible_qos_triggers():
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::IncompatibleQos")

    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    reader = dds.DynamicData.DataReader(participant.find_datareader("Reader"))
    assert writer.offered_incompatible_qos_status.total_count == 0
    assert writer.offered_incompatible_qos_status.total_count_change == 0
    assert writer.offered_incompatible_qos_status.last_policy is None
    assert writer.offered_incompatible_qos_status.policies == []

    listener_called = False
    status_asserted = False

    class IncompatibleQosListener(dds.DynamicData.NoOpDataWriterListener):
        def on_offered_incompatible_qos(self, writer, status):
            nonlocal listener_called
            nonlocal status_asserted
            if (status.total_count == 1 and
                status.total_count_change == 1 and
                status.last_policy == dds.Durability and
                    status.policies[0].policy == dds.Durability):
                status_asserted = True
            listener_called = True

    writer.listener = IncompatibleQosListener()
    enable(participant, reader, writer)
    wait.until(lambda: listener_called)
    assert status_asserted
    assert writer.offered_incompatible_qos_status.total_count == 1
    assert writer.offered_incompatible_qos_status.total_count_change == 0
    assert writer.offered_incompatible_qos_status.last_policy == dds.Durability
    assert writer.offered_incompatible_qos_status.policies[0].policy == dds.Durability
    participant.close()


def test_offered_incompatible_qos_can_be_disabled():
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::IncompatibleQos")
    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    reader = dds.DynamicData.DataReader(participant.find_datareader("Reader"))
    listener_called = False

    class IncompatibleQosListener(dds.DynamicData.NoOpDataWriterListener):
        def on_offered_incompatible_qos(self, writer, status):
            nonlocal listener_called
            listener_called = True
    writer.set_listener(IncompatibleQosListener(), dds.StatusMask.NONE)
    enable(participant, reader, writer)
    sleep(.05)
    assert not listener_called
    participant.close()


def test_liveliness_lost_status_change():
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::LivelinessLost")
    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    reader = dds.DynamicData.DataReader(participant.find_datareader("Reader"))
    assert writer.liveliness_lost_status.total_count == 0
    assert writer.liveliness_lost_status.total_count_change == 0
    saved_writer = None
    saved_status = None

    class LivelinessLostListener(dds.DynamicData.NoOpDataWriterListener):
        def on_liveliness_lost(self, writer, status):
            nonlocal saved_writer
            nonlocal saved_status
            saved_writer = writer
            saved_status = status

    writer.listener = LivelinessLostListener()
    enable(participant, reader, writer)

    wait.until(lambda: saved_status is not None)
    assert saved_status.total_count == 1
    assert saved_status.total_count_change == 1

    assert writer.liveliness_lost_status.total_count == 1
    assert writer.liveliness_lost_status.total_count_change == 0
    participant.close()


def test_liveliness_lost_can_be_disabled():
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::LivelinessLost")
    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    reader = dds.DynamicData.DataReader(participant.find_datareader("Reader"))
    listener_called = False

    class LivelinessLostListener(dds.DynamicData.NoOpDataWriterListener):
        def on_liveliness_lost(self, writer, status):
            nonlocal listener_called
            listener_called = True
    writer.set_listener(LivelinessLostListener(), dds.StatusMask.NONE)
    enable(participant, reader, writer)
    sleep(.05)
    assert not listener_called
    participant.close()


def test_offered_deadline_missed_status_change():
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::DeadlineMissed")

    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    reader = dds.DynamicData.DataReader(participant.find_datareader("Reader"))
    assert writer.offered_deadline_missed_status.total_count == 0
    assert writer.offered_deadline_missed_status.total_count_change == 0

    saved_writer = None
    saved_status = None

    class DeadlineMissedListener(dds.DynamicData.NoOpDataWriterListener):
        def on_offered_deadline_missed(self, writer, status):
            nonlocal saved_writer
            nonlocal saved_status
            saved_writer = writer
            saved_status = status
    writer.listener = DeadlineMissedListener()
    enable(participant, reader, writer)
    sample = writer.create_data()
    instance = writer.register_instance(sample)
    writer.write(sample)
    wait.until(lambda: saved_writer is not None)
    assert saved_status.total_count == 1
    assert saved_status.total_count_change == 1
    assert saved_status.last_instance_handle == instance

    assert writer.offered_deadline_missed_status.total_count == 1
    assert writer.offered_deadline_missed_status.total_count_change == 0
    assert writer.offered_deadline_missed_status.last_instance_handle == instance
    participant.close()


def test_datawriter_cache_status_changes():
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::Default")

    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    reader = dds.DynamicData.DataReader(participant.find_datareader("Reader"))

    enable(participant, reader, writer)
    wait.for_discovery(reader, writer)

    samples_to_write = 5
    for _ in range(samples_to_write):
        sample = writer.create_data()
        instance = writer.register_instance(sample)
        writer.write(sample)
        writer.unregister_instance(instance)
        wait.for_samples(reader, 2)
        received_samples = reader.take()
        assert len(received_samples) == 2
        wait.until_equal(
            0, lambda: writer.datawriter_cache_status.sample_count)

    cache_status = writer.datawriter_cache_status
    assert cache_status.sample_count == 0
    assert cache_status.sample_count_peak == 2
    assert cache_status.alive_instance_count == 0
    assert cache_status.alive_instance_count_peak == 1
    assert cache_status.disposed_instance_count == 0
    assert cache_status.disposed_instance_count_peak == 0
    assert cache_status.unregistered_instance_count == 0
    assert cache_status.unregistered_instance_count_peak == 1


def test_datawriter_protocol_status_changes(shared_participant):
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::Default")

    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    reader = dds.DynamicData.DataReader(participant.find_datareader("Reader"))

    enable(participant, reader, writer)
    wait.for_discovery(reader, writer)

    status_by_sub = writer.matched_subscription_datawriter_protocol_status(
        reader.instance_handle)
    assert status_by_sub is not None
    assert status_by_sub.pushed_sample_count.total == 0

    samples_to_write = 5
    for _ in range(samples_to_write):
        writer.write(writer.create_data())

    data_writer_protocol_status = writer.datawriter_protocol_status
    assert data_writer_protocol_status.pushed_sample_count.total == samples_to_write
    assert data_writer_protocol_status.pushed_sample_count.change == samples_to_write
    assert data_writer_protocol_status.pushed_sample_bytes.total == 60 * samples_to_write
    assert data_writer_protocol_status.pushed_sample_bytes.change == 60 * samples_to_write

    wait.for_samples(reader, samples_to_write)
    status_by_sub = writer.matched_subscription_datawriter_protocol_status(
        reader.instance_handle)
    assert status_by_sub.pushed_sample_count.total >= samples_to_write
    # Test other overload of matched_subscription_datawriter_protocol_status
    data = shared_participant.discovered_participant_data(
        participant.instance_handle)
    # It isn't certain which unicast locator it will be so we'll check that at
    # least one has the proper values
    one_locator_has_expected_values = False
    for locator in data.default_unicast_locators:
        status_from_locator = writer.matched_subscription_datawriter_protocol_status(
            locator)
        if status_from_locator.pushed_sample_count.total >= samples_to_write \
                and status_from_locator.pushed_sample_count.total == status_by_sub.pushed_sample_count.total:
            one_locator_has_expected_values = True
            break
    assert one_locator_has_expected_values


def test_reliable_reader_activity_changed_listener_triggers():
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::ReliableReaderActivityChanged")

    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    reader = dds.DynamicData.DataReader(participant.find_datareader("Reader"))

    saved_event_writer = None
    saved_status = None

    class ReliableReaderActivityChangedListener(dds.DynamicData.NoOpDataWriterListener):
        def on_reliable_reader_activity_changed(self, writer, status):
            nonlocal saved_event_writer
            nonlocal saved_status
            saved_event_writer = writer
            saved_status = status
    writer.listener = ReliableReaderActivityChangedListener()
    assert writer.reliable_reader_activity_changed_status.active_count.total == 0
    assert writer.reliable_reader_activity_changed_status.active_count.change == 0
    assert writer.reliable_reader_activity_changed_status.inactive_count.total == 0
    assert writer.reliable_reader_activity_changed_status.inactive_count.change == 0

    enable(participant, reader, writer)
    wait.for_discovery(reader, writer)
    wait.until(lambda: saved_event_writer is not None)
    assert saved_status.active_count.total == 1
    assert saved_status.active_count.change == 1
    assert saved_status.inactive_count.total == 0
    assert saved_status.inactive_count.change == 0
    participant.close()


def test_reliable_writer_cache_listener_triggers():
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::ReliableWriterCacheChanged")

    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    reader = dds.DynamicData.DataReader(participant.find_datareader("Reader"))

    saved_event_writer = None
    saved_status = None

    class ReliableWriterCacheChangedListener(dds.DynamicData.NoOpDataWriterListener):
        def on_reliable_writer_cache_changed(self, writer, status):
            nonlocal saved_event_writer
            nonlocal saved_status
            saved_event_writer = writer
            saved_status = status

    writer.listener = ReliableWriterCacheChangedListener()

    enable(participant, reader, writer)
    wait.for_discovery(reader, writer)
    pre_status = writer.reliable_writer_cache_changed_status
    assert pre_status.unacknowledged_sample_count_peak == 0
    assert pre_status.empty_reliable_writer_cache.total == 0
    assert pre_status.empty_reliable_writer_cache.change == 0

    writer.write(writer.create_data())
    wait.until(lambda: saved_event_writer is not None)
    assert 1 == saved_status.unacknowledged_sample_count_peak
    assert 1 == saved_status.empty_reliable_writer_cache.total
    assert 1 == saved_status.empty_reliable_writer_cache.change
    assert 1 == writer.reliable_writer_cache_changed_status.unacknowledged_sample_count_peak
    assert 1 == writer.reliable_writer_cache_changed_status.empty_reliable_writer_cache.total
    assert 0 == writer.reliable_writer_cache_changed_status.empty_reliable_writer_cache.change
    participant.close()


def test_instance_replaced_triggers():
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::InstanceReplaced")

    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    reader = dds.DynamicData.DataReader(participant.find_datareader("Reader"))

    saved_event_writer = None
    saved_instance_handle = dds.InstanceHandle.nil()

    class InstanceReplacedListener(dds.DynamicData.NoOpDataWriterListener):
        def on_instance_replaced(self, writer, instance_handle):
            nonlocal saved_event_writer
            nonlocal saved_instance_handle
            saved_event_writer = writer
            saved_instance_handle = instance_handle

    writer.listener = InstanceReplacedListener()

    enable(participant, reader, writer)
    wait.for_discovery(reader, writer)

    first_sample = writer.create_data()
    first_sample["foo"] = 1
    instance = writer.register_instance(first_sample)
    writer.write(first_sample)
    wait.for_data(reader)
    writer.dispose_instance(instance)

    wait.until_equal(
        1, lambda: reader.datareader_cache_status.disposed_instance_count)
    second_sample = writer.create_data()
    second_sample["foo"] = 2
    writer.write(second_sample)
    wait.until(lambda: saved_event_writer is not None)
    participant.close()


def test_service_request_accepted_triggers(shared_participant):
    saved_event_writer = None
    saved_status = None

    @idl.struct(member_annotations={"x": [idl.key]})
    class Point:
        x: int = 0
        y: int = 0

    class ServiceRequestAcceptedListener(dds.NoOpDataWriterListener):
        def on_service_request_accepted(self, writer, status):
            nonlocal saved_event_writer
            nonlocal saved_status
            saved_event_writer = writer
            saved_status = status

    dw_qos = dds.DataWriterQos()
    dw_qos.reliability.kind = dds.ReliabilityKind.RELIABLE
    dw_qos.durability.kind = dds.DurabilityKind.TRANSIENT_LOCAL
    dw_qos.history.kind = dds.HistoryKind.KEEP_ALL
    dw_qos.topic_query_dispatch.enable = True

    dr_qos = dds.DataReaderQos()
    dr_qos.reliability.kind = dds.ReliabilityKind.RELIABLE
    dr_qos.durability.kind = dds.DurabilityKind.TRANSIENT_LOCAL
    dr_qos.history.kind = dds.HistoryKind.KEEP_ALL

    topic = dds.Topic(shared_participant, "TestTopic", Point)
    writer = dds.DataWriter(
        shared_participant,
        topic,
        dw_qos,
        ServiceRequestAcceptedListener(),
        dds.StatusMask.SERVICE_REQUEST_ACCEPTED)
    reader = dds.DataReader(
        shared_participant,
        topic,
        dr_qos,
        None,
        dds.StatusMask.NONE)

    for i in range(0, 5):
        sample = Point(x=i, y=i)
        instance_handle = writer.register_instance(sample)
        writer.write(sample, instance_handle)

    wait.for_data(reader, 5)
    topic_query = dds.TopicQuery(
        reader, dds.TopicQuerySelection(dds.Filter("x = 1 or x = 3")))

    wait.until(lambda: saved_status is not None)
    status = writer.service_request_accepted_status
    assert status.total_count.total == 1
    assert status.current_count.total == 1
    assert saved_status.total_count.change == 1
    assert saved_status.current_count.change == 1
    assert status.last_request_handle != dds.InstanceHandle.nil()
    assert status.service_id == dds.ServiceRequestId.TOPIC_QUERY
