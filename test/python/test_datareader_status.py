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

from test_utils.fixtures import *
from rti.types.builtin import String, KeyedString
import pathlib


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


def test_subscription_matched_event_triggers():
    participant = create_participant()
    topic = dds.Topic(participant, "TestTopic", String)
    saved_event_reader = None
    saved_status = None

    class SubscriptionMatchedListener(dds.NoOpDataReaderListener):
        def on_subscription_matched(self, reader, status):
            nonlocal saved_event_reader
            nonlocal saved_status
            saved_event_reader = reader
            saved_status = status
    reader = dds.DataReader(participant.implicit_subscriber,
                            topic,
                            dds.DataReaderQos(),
                            SubscriptionMatchedListener())
    writer = dds.DataWriter(participant.implicit_publisher, topic)

    wait.until(lambda: saved_status is not None)
    assert saved_event_reader == reader
    assert reader.subscription_matched_status.current_count == 1
    assert writer.instance_handle == saved_status.last_publication_handle
    participant.close()


def test_subscription_matched_status_change_triggers():
    fixture = PubSubFixture(create_participant(), String)
    wait.until(lambda: fixture.reader.status_changes &
               dds.StatusMask.SUBSCRIPTION_MATCHED)

    assert fixture.reader.subscription_matched_status.total_count_change == 1

    # After looking up the status, the status change should be cleared
    assert not (fixture.reader.status_changes &
                dds.StatusMask.SUBSCRIPTION_MATCHED)


def test_data_available_listener_triggers():
    participant = create_participant()
    topic = dds.Topic(participant, "TestTopic", String)
    saved_event_reader = None

    class DataAvailableListener(dds.NoOpDataReaderListener):
        def on_data_available(self, reader):
            nonlocal saved_event_reader
            saved_event_reader = reader

    reader = dds.DataReader(participant.implicit_subscriber,
                            topic,
                            dds.DataReaderQos(),
                            DataAvailableListener())
    writer = dds.DataWriter(participant.implicit_publisher, topic)
    wait.for_discovery(reader, writer)
    writer.write(String("Hello World"))
    wait.until(lambda: saved_event_reader is not None)
    assert saved_event_reader == reader
    participant.close()


def test_liveliness_changed_listener_triggers():
    participant = create_participant()
    topic = dds.Topic(participant, "TestTopic", String)
    saved_event_reader = None
    saved_status = None

    class LivelinessChangedListener(dds.NoOpDataReaderListener):
        def on_liveliness_changed(self, reader, status):
            nonlocal saved_event_reader
            nonlocal saved_status
            saved_event_reader = reader
            saved_status = status
    reader = dds.DataReader(participant.implicit_subscriber,
                            topic,
                            dds.DataReaderQos(),
                            LivelinessChangedListener())
    writer = dds.DataWriter(participant.implicit_publisher, topic)
    wait.for_discovery(reader, writer)
    sample = KeyedString("Hello World", "Goodbye World")
    handle = writer.register_instance(sample)
    writer.write(sample)

    writer.dispose_instance(handle)
    wait.until(lambda: saved_status is not None)
    assert saved_event_reader == reader
    assert reader.liveliness_changed_status.alive_count == 1
    assert reader.liveliness_changed_status.not_alive_count == 0
    assert reader.liveliness_changed_status.alive_count_change == 0
    assert reader.liveliness_changed_status.not_alive_count_change == 0
    assert writer.instance_handle == saved_status.last_publication_handle
    participant.close()


def test_requested_deadline_missed_listener_triggers():
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::DeadlineMissed")
    foo_type = provider.type("Foo")
    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    any_reader = participant.find_datareader("Reader")
    reader: dds.DynamicData.DataReader = dds.DynamicData.DataReader(any_reader)

    saved_event_reader = None
    saved_status = None

    class RequestedDeadlineMissedListener(dds.DynamicData.NoOpDataReaderListener):
        def on_requested_deadline_missed(self, reader, status):
            nonlocal saved_event_reader
            nonlocal saved_status
            saved_event_reader = reader
            saved_status = status

    reader.listener = RequestedDeadlineMissedListener()
    enable(participant, reader, writer)
    wait.for_discovery(reader, writer)
    sample = writer.create_data()
    handle = writer.register_instance(sample)
    writer.write(sample)

    wait.until(lambda: saved_status is not None)
    assert saved_event_reader == reader
    assert saved_status.total_count == 1
    assert saved_status.total_count_change == 1
    assert reader.requested_deadline_missed_status.total_count == 1
    assert reader.requested_deadline_missed_status.total_count_change == 0
    assert handle == saved_status.last_instance_handle
    participant.close()


def test_requested_incompatible_qos_listener_triggers():
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::IncompatibleQos")
    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    any_reader = participant.find_datareader("Reader")
    reader: dds.DynamicData.DataReader = dds.DynamicData.DataReader(any_reader)

    saved_event_reader = None
    saved_status = None

    class RequestedIncompatibleQosListener(dds.DynamicData.NoOpDataReaderListener):
        def on_requested_incompatible_qos(self, reader, status):
            nonlocal saved_event_reader
            nonlocal saved_status
            saved_event_reader = reader
            saved_status = status

    reader.listener = RequestedIncompatibleQosListener()
    enable(participant, reader, writer)
    wait.until(lambda: saved_status is not None)
    assert saved_event_reader == reader
    assert saved_status.total_count() == 1
    assert saved_status.total_count_change == 1
    assert reader.requested_incompatible_qos_status.total_count() == 1
    assert reader.requested_incompatible_qos_status.total_count_change == 0
    assert saved_status.last_policy == dds.Durability
    assert saved_status.policies[0].policy == dds.Durability
    del saved_status
    participant.close()


def test_sample_lost_listener_triggers():
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::SampleLost")
    foo_type = provider.type("Foo")
    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    any_reader = participant.find_datareader("Reader")
    reader: dds.DynamicData.DataReader = dds.DynamicData.DataReader(any_reader)

    saved_event_reader = None
    saved_status = None

    class SampleLostListener(dds.DynamicData.NoOpDataReaderListener):
        def on_sample_lost(self, reader, status):
            nonlocal saved_event_reader
            nonlocal saved_status
            saved_event_reader = reader
            saved_status = status

    reader.listener = SampleLostListener()
    enable(participant, reader, writer)
    wait.for_discovery(reader, writer)
    sample1 = writer.create_data()
    instance1 = writer.register_instance(sample1)
    writer.write(sample1)

    # Write sample once again, but this is one should be lost
    # because the samples limit is 1 (and we are using best effort)
    writer.write(sample1)

    wait.until(lambda: saved_status is not None)
    assert saved_event_reader == reader
    assert saved_status.last_reason == dds.SampleLostState.LOST_BY_SAMPLES_LIMIT
    assert saved_status.total_count == 1
    assert saved_status.total_count_change == 1
    assert reader.sample_lost_status.total_count == 1
    assert reader.sample_lost_status.total_count_change == 0
    participant.close()


def test_sample_rejected_listener_triggers():
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::SampleRejected")
    foo_type = provider.type("Foo")
    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    any_reader = participant.find_datareader("Reader")
    reader: dds.DynamicData.DataReader = dds.DynamicData.DataReader(any_reader)

    saved_event_reader = None
    saved_status = None

    class SampleRejectedListener(dds.DynamicData.NoOpDataReaderListener):
        def on_sample_rejected(self, reader, status):
            nonlocal saved_event_reader
            nonlocal saved_status
            saved_event_reader = reader
            saved_status = status

    reader.listener = SampleRejectedListener()
    enable(participant, reader, writer)
    wait.for_discovery(reader, writer)

    sample1 = writer.create_data()
    instance1 = writer.register_instance(sample1)
    writer.write(sample1)
    writer.write(sample1)

    wait.until(lambda: saved_status is not None)
    assert saved_event_reader == reader
    assert saved_status.last_reason == dds.SampleRejectedState.REJECTED_BY_SAMPLES_PER_INSTANCE_LIMIT
    assert saved_status.total_count == 1
    assert saved_status.total_count_change == 1
    assert reader.sample_rejected_status.total_count == 1
    assert reader.sample_rejected_status.total_count_change == 0
    participant.close()


def test_datareader_cache_status_change():
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::Default")
    foo_type = provider.type("Foo")
    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    any_reader = participant.find_datareader("Reader")
    reader: dds.DynamicData.DataReader = dds.DynamicData.DataReader(any_reader)

    enable(participant, reader, writer)
    wait.for_discovery(reader, writer)

    for _ in range(5):
        sample = writer.create_data()
        instance = writer.register_instance(sample)

        writer.write(sample)
        writer.unregister_instance(instance)

        wait.for_samples(reader, 2)
        reader.take()
        wait.until_equal(
            0, lambda: writer.datawriter_cache_status.sample_count)

    cache = reader.datareader_cache_status
    assert cache.sample_count == 0
    assert cache.sample_count_peak == 2
    assert cache.alive_instance_count == 0
    assert cache.alive_instance_count_peak == 1
    assert cache.disposed_instance_count == 0
    assert cache.disposed_instance_count_peak == 0
    assert cache.detached_instance_count == 1
    assert cache.detached_instance_count_peak == 1
    participant.close()


def test_datareader_protocol_status_changes():
    provider = get_provider()
    participant = get_participant_from_provider(
        provider, "StatusTests::Default")
    foo_type = provider.type("Foo")
    writer = dds.DynamicData.DataWriter(participant.find_datawriter("Writer"))
    any_reader = participant.find_datareader("Reader")
    reader: dds.DynamicData.DataReader = dds.DynamicData.DataReader(any_reader)
    zero_writer = dds.DynamicData.DataWriter(
        participant.implicit_publisher, writer.topic)

    enable(participant, reader, writer)
    wait.for_discovery(reader, writer)
    wait.for_discovery(reader, zero_writer)

    for _ in range(5):
        writer.write(writer.create_data())

    wait.for_samples(reader, 5)

    status = reader.datareader_protocol_status
    assert status.received_sample_count.change == 5
    assert status.received_sample_count.total == 5
    assert status.received_sample_bytes.change == 5 * 60
    assert status.received_sample_bytes.total == 5 * 60

    status = reader.datareader_protocol_status
    assert status.received_sample_count.change == 0
    assert status.received_sample_count.total == 5
    assert status.received_sample_count.change == 0
    assert status.received_sample_count.total == 5

    matched_status = reader.matched_publication_datareader_protocol_status(
        writer.instance_handle)
    assert status.received_sample_count.total == matched_status.received_sample_count.total
    assert status.received_sample_bytes.total == matched_status.received_sample_bytes.total

    zero_matched_status = reader.matched_publication_datareader_protocol_status(
        zero_writer.instance_handle)
    assert zero_matched_status.received_sample_count.total == 0

    participant.close()
