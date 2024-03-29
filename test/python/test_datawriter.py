#
# (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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

import pytest
from test_utils.fixtures import *
from rti.types.builtin import String, KeyedString, Bytes


# --- Helpers and fixtures ----------------------------------------------------

def same_elements(a, b):
    return len(a) == len(b) and all(x in b for x in a)


def check_expected_data(reader: dds.DataReader, expected_samples: list):
    wait.for_data(reader, count=len(expected_samples))
    received = reader.take()
    assert same_elements([data for (data, _) in received], expected_samples)


def check_data_and_get_info(reader: dds.DataReader, expected_samples: list):
    wait.for_data(reader, count=len(expected_samples))
    samples = reader.take()
    assert same_elements([data for (data, _) in samples], expected_samples)
    return [info for (_, info) in samples]


@idl.struct(member_annotations={'x': [idl.key]})
class PointIDL:
    x: int = 0
    y: int = 0


@idl.struct(member_annotations={'x': [idl.key]})
class PointIDLForDD:
    x: int = 0
    y: int = 0


# The reason these types are wrapped in a lambda is so that they are lazily
# created and they will be garbage collected before we do heap monitoring.
# Since these types would normally be created at test collection time, they
# would outlive the tests and be detected as memory leaks.
def get_test_types_generator():
    return [
        lambda: PointIDL,
        lambda: idl.get_type_support(PointIDLForDD).dynamic_type,
        lambda: String,
        lambda: KeyedString,
    ]


def get_keyed_types_generator():
    return [
        lambda: PointIDL,
        lambda: idl.get_type_support(PointIDLForDD).dynamic_type,
        lambda: KeyedString,
    ]


@pytest.fixture(scope="function", params=get_test_types_generator())
def pubsub(request, shared_participant):
    """This fixture provides a test with a writer and a reader that have already
       been discovered. The participant is shared within the module to speed up
       execution, but the contained entities are deleted after each test
       function."""

    data_type = request.param()
    fixture = PubSubFixture(shared_participant, data_type)
    yield fixture
    fixture.participant.close_contained_entities()


@pytest.fixture(scope="function", params=get_keyed_types_generator())
def pubsub_keyed_types(request, shared_participant):
    data_type = request.param()
    fixture = PubSubFixture(shared_participant, data_type)
    yield fixture
    fixture.participant.close_contained_entities()


@pytest.fixture(scope="function", params=get_keyed_types_generator())
def pub(request, shared_participant):
    data_type = request.param()
    fixture = PubSubFixture(
        shared_participant, data_type, create_reader=False, writer_policies=[dds.EntityName("writer")])
    yield fixture
    fixture.participant.close_contained_entities()


def get_sample_value(type: type):
    if type is PointIDL:
        return PointIDL(x=1, y=2)
    elif type == idl.get_type_support(PointIDLForDD).dynamic_type:
        point_dynamic_data = dds.DynamicData(
            idl.get_type_support(PointIDLForDD).dynamic_type)
        point_dynamic_data["x"] = 1
        point_dynamic_data["y"] = 2
        return point_dynamic_data
    elif type is String:
        return String("Hello World")
    elif type is KeyedString:
        return KeyedString("Hello World", "Hello World")
    else:
        raise TypeError(f"Unsupported type: {type}")


def get_sample_keys(type: type):
    if type is PointIDL:
        return PointIDL(x=1, y=0)
    elif type == idl.get_type_support(PointIDLForDD).dynamic_type:
        point_dynamic_data = dds.DynamicData(
            idl.get_type_support(PointIDLForDD).dynamic_type)
        point_dynamic_data["x"] = 1
        point_dynamic_data["y"] = 0
        return point_dynamic_data
    elif type is KeyedString:
        return KeyedString("Hello World", "")
    else:
        raise TypeError(f"Unsupported type: {type}")


def keys_equal(a, b) -> bool:
    if type(a) is not type(b):
        raise TypeError(
            f"Types do not match: {type(a)} != {type(b)}")
    if type(a) is PointIDL:
        return a.x == b.x
    elif type(a) == dds.DynamicData:
        return a["x"] == b["x"]
    elif type(a) is KeyedString:
        return a.key == b.key
    else:
        raise TypeError(f"Unsupported type: {type}")


class PointListener(dds.NoOpDataWriterListener):
    def on_data_available(self, writer):
        pass


class OtherPointListener(dds.NoOpDataWriterListener):
    def on_data_available(self, writer):
        pass


def get_writer_listeners(type: type):
    if type is PointIDL:
        return (PointListener(), OtherPointListener())
    elif type == idl.get_type_support(PointIDLForDD).dynamic_type:
        return (dds.DynamicData.NoOpDataWriterListener(),
                dds.DynamicData.NoOpDataWriterListener())
    elif type is String:
        return (dds.NoOpDataWriterListener(),
                dds.NoOpDataWriterListener())
    elif type is KeyedString:
        return (dds.NoOpDataWriterListener(),
                dds.NoOpDataWriterListener())
    else:
        raise TypeError(f"Unsupported type: {type}")


def get_writer_w_listeners(participant, topic, type, listener):
    ns = test_utils._get_topic_namespace(topic, type)
    return ns.DataWriter(participant, topic, dds.DataWriterQos(), listener)


# --- Getter and setter tests -------------------------------------------------


def test_writer_get_set_qos(pub):
    qos = dds.DataWriterQos()
    pub.writer >> qos
    assert qos == pub.writer.qos
    qos.availability.max_endpoint_availability_waiting_time = dds.Duration(
        2, 2)
    pub.writer.qos = qos
    assert pub.writer.qos.availability.max_endpoint_availability_waiting_time == dds.Duration(
        2, 2)
    qos.availability.max_endpoint_availability_waiting_time = dds.Duration(
        1, 1)
    pub.writer << qos
    assert pub.writer.qos.availability.max_endpoint_availability_waiting_time == dds.Duration(
        1, 1)


def test_writer_readonly_properties(pub):
    assert pub.writer.topic == pub.topic
    assert pub.writer.publisher == pub.publisher
    assert pub.writer.topic_name == pub.topic.name
    assert pub.writer.type_name == pub.topic.type_name


def test_writer_find(pub):
    assert pub.writer.find_all_by_topic(
        pub.publisher, pub.topic.name) == [pub.writer]
    assert pub.writer.find_by_name(
        pub.publisher, pub.writer.qos.entity_name.name) == pub.writer
    assert pub.writer.find_by_topic(
        pub.publisher, pub.topic.name) == pub.writer


def test_writer_assert_liveliness(pub):
    pub.writer.assert_liveliness()

# --- Write tests -------------------------------------------------------------


def test_write(pubsub):
    sample = get_sample_value(pubsub.data_type)
    pubsub.writer.write(sample)
    check_expected_data(pubsub.reader, [sample])


def test_write_with_shift_operator(pubsub):
    sample = get_sample_value(pubsub.data_type)
    pubsub.writer << sample
    check_expected_data(pubsub.reader, [sample])


def test_write_with_timestamp(pubsub):
    sample = get_sample_value(pubsub.data_type)
    pubsub.writer.write(sample, dds.Time(123))
    info = check_data_and_get_info(pubsub.reader, [sample])

    assert info[0].source_timestamp == dds.Time(123)


def test_write_with_instance_handle(pubsub_keyed_types):
    sample = get_sample_value(pubsub_keyed_types.data_type)
    instance_handle = pubsub_keyed_types.writer.register_instance(sample)
    assert not instance_handle.is_nil

    pubsub_keyed_types.writer.write(sample, handle=instance_handle)
    info = check_data_and_get_info(pubsub_keyed_types.reader, [sample])

    assert info[0].instance_handle == instance_handle


def test_write_with_instance_handle_and_timestamp(pubsub_keyed_types):
    sample = get_sample_value(pubsub_keyed_types.data_type)
    instance_handle = pubsub_keyed_types.writer.register_instance(sample)

    pubsub_keyed_types.writer.write(sample, handle=instance_handle,
                                    timestamp=dds.Time(123))
    info = check_data_and_get_info(pubsub_keyed_types.reader, [sample])

    assert info[0].instance_handle == instance_handle
    assert info[0].source_timestamp == dds.Time(123)


def test_write_w_params(pubsub):
    sample = get_sample_value(pubsub.data_type)
    params = dds.WriteParams()
    params.source_timestamp = dds.Time(123)
    pubsub.writer.write(sample, params)

    info = check_data_and_get_info(pubsub.reader, [sample])
    assert info[0].source_timestamp == dds.Time(123)


def test_write_with_timestamp_with_shift_operator(pubsub):
    sample = get_sample_value(pubsub.data_type)
    pubsub.writer << (sample, dds.Time(123))

    info = check_data_and_get_info(pubsub.reader, [sample])
    assert info[0].source_timestamp == dds.Time(123)


def test_write_list(pubsub):
    samples = [get_sample_value(pubsub.data_type),
               get_sample_value(pubsub.data_type)]
    pubsub.writer.write(samples)
    check_expected_data(pubsub.reader, samples)


def test_write_list_with_shift_operator(pubsub):
    samples = [get_sample_value(pubsub.data_type),
               get_sample_value(pubsub.data_type)]
    pubsub.writer << samples
    check_expected_data(pubsub.reader, samples)


def test_write_list_with_timestamp(pubsub):
    samples = [get_sample_value(pubsub.data_type),
               get_sample_value(pubsub.data_type)]
    pubsub.writer.write(samples, dds.Time(123))

    infos = check_data_and_get_info(pubsub.reader, samples)
    assert all(info.source_timestamp == dds.Time(123) for info in infos)


def test_write_list_of_pairs_with_shift_operator(pubsub):
    samples = [get_sample_value(pubsub.data_type),
               get_sample_value(pubsub.data_type)]
    pubsub.writer << [(sample, dds.Time(1)) for sample in samples]

    infos = check_data_and_get_info(pubsub.reader, samples)
    assert all(info.source_timestamp == dds.Time(1)
               for info, sample in zip(infos, samples))


# --- Listeners tests ----------------------------------------------------------


def test_datawriter_listener_can_be_set(pubsub):
    listener, other_listener = get_writer_listeners(pubsub.data_type)

    assert pubsub.writer.listener is None

    pubsub.writer.listener = listener
    assert pubsub.writer.listener == listener

    # Test set_listener fn
    pubsub.writer.set_listener(other_listener, dds.StatusMask.ALL)
    assert pubsub.writer.listener == other_listener

    # Test it can be set to none
    pubsub.writer.listener = None
    assert pubsub.writer.listener is None

    pubsub.writer.listener = get_writer_listeners(pubsub.data_type)[0]
    assert isinstance(pubsub.writer.listener,
                      (dds.DataWriterListener,
                       dds.DynamicData.DataWriterListener))

    # Test constructor
    new_writer = get_writer_w_listeners(
        pubsub.participant, pubsub.topic, pubsub.data_type, listener)
    assert new_writer.listener == listener
    new_writer.close()


def test_datawriter_listener_is_invoked(shared_participant):

    topic = dds.Topic(shared_participant, "listener_test_topic", PointIDL)
    writer_qos = dds.DataWriterQos()
    writer_qos << dds.Reliability(dds.ReliabilityKind.BEST_EFFORT)
    reader_qos = dds.DataReaderQos()
    reader_qos << dds.Reliability(dds.ReliabilityKind.RELIABLE)
    pub = dds.Publisher(shared_participant)
    sub = dds.Subscriber(shared_participant)

    class TestListener(dds.NoOpDataWriterListener):
        on_offered_incompatible_qos_called = False

        def on_offered_incompatible_qos(self, writer, incompatible_qos_status):
            assert incompatible_qos_status.last_policy is dds.Reliability
            assert len(incompatible_qos_status.policies) == 1
            assert incompatible_qos_status.policies[0].policy is dds.Reliability
            assert incompatible_qos_status.policies[0].count == 1
            self.on_offered_incompatible_qos_called = True

    writer = dds.DataWriter(pub, topic, writer_qos, TestListener())
    # Create a reader with incompatible qos
    reader = dds.DataReader(sub, topic, reader_qos)

    wait.until(lambda: writer.listener.on_offered_incompatible_qos_called)


# --- Instance tests ----------------------------------------------------------


def test_register_dispose_unregister_instance(pub):
    sample = get_sample_value(pub.data_type)
    instance_handle = pub.writer.register_instance(sample)
    assert instance_handle is not None
    assert instance_handle != dds.InstanceHandle.nil()

    pub.writer.dispose_instance(instance_handle)
    pub.writer.unregister_instance(instance_handle)


def test_register_dispose_unregister_instance_with_timestamp(pub):
    sample = get_sample_value(pub.data_type)
    instance_handle = pub.writer.register_instance(sample, dds.Time(123))
    assert instance_handle is not None
    assert instance_handle != dds.InstanceHandle.nil()

    pub.writer.dispose_instance(instance_handle, dds.Time(124))
    pub.writer.unregister_instance(instance_handle, dds.Time(125))


def test_register_dispose_unregister_instance_with_params(pub):
    sample = get_sample_value(pub.data_type)
    params = dds.WriteParams()
    params.source_timestamp = dds.Time(123)
    instance_handle = pub.writer.register_instance(sample, params)
    assert instance_handle is not None
    assert instance_handle != dds.InstanceHandle.nil()

    params.source_timestamp = dds.Time(124)
    params.handle = instance_handle
    pub.writer.dispose_instance(params)

    params.source_timestamp = dds.Time(125)
    pub.writer.unregister_instance(params)


def test_pubsub_with_timestamp(pubsub_keyed_types):
    pubsub = pubsub_keyed_types
    sample = get_sample_value(pubsub.data_type)
    instance_handle = pubsub.writer.register_instance(sample, dds.Time(123))
    assert instance_handle is not None
    assert instance_handle != dds.InstanceHandle.nil()

    pubsub.writer.write(sample, dds.Time(124))
    wait.for_data(pubsub.reader)
    samples = pubsub.reader.take()
    assert len(samples) == 1
    _, info = samples[0]
    assert info.valid
    assert info.source_timestamp == dds.Time(124)

    pubsub.writer.dispose_instance(instance_handle, dds.Time(124))
    wait.for_samples(pubsub.reader)
    samples = pubsub.reader.take()
    assert len(samples) == 1
    data, info = samples[0]
    assert data is None
    assert not info.valid
    assert info.source_timestamp == dds.Time(124)


# --- Matched tests ----------------------------------------------------------

def test_writer_matched_subscription_active(pubsub):
    assert pubsub.writer.is_matched_subscription_active(
        pubsub.reader.instance_handle)
    for locator in pubsub.writer.matched_subscriptions_locators:
        assert locator.port >= 7400


def test_writer_matched_subscription_data(pubsub):
    data = pubsub.writer.matched_subscription_data(
        pubsub.reader.instance_handle)
    assert data is not None
    assert data.topic_name == pubsub.reader.topic_name
    assert data.type_name == pubsub.reader.type_name


@pytest.mark.parametrize("test_type", get_test_types_generator())
def test_writer_matched_subscription_participant_data(shared_participant, test_type):
    p1 = shared_participant
    p2 = create_participant()
    publisher = dds.Publisher(p1)
    test_type = test_type()
    ns = dds.DynamicData if type(test_type) is dds.StructType else dds
    pub_topic = ns.Topic(p1, "TestTopic", test_type)
    sub_topic = ns.Topic(p2, "TestTopic", test_type)
    writer = ns.DataWriter(publisher, pub_topic)
    reader = ns.DataReader(dds.Subscriber(p2), sub_topic)
    wait.for_discovery(reader, writer)
    subscriptions = writer.matched_subscriptions
    assert subscriptions[0] == reader.instance_handle
    data = writer.matched_subscription_participant_data(subscriptions[0])
    assert data is not None
    assert data.domain_id == p2.domain_id


# --- Key Value tests ---------------------------------------------------------

def test_key_value(pub):
    sample = get_sample_value(pub.data_type)
    instance = pub.writer.register_instance(sample)
    assert instance != dds.InstanceHandle.nil()
    result = pub.writer.key_value(instance)
    key_holder = get_sample_keys(pub.data_type)
    assert keys_equal(result, key_holder)
    assert instance == pub.writer.lookup_instance(result)

# --- Other tests -------------------------------------------------------------


@pytest.mark.parametrize("test_type", get_test_types_generator())
def test_wait_for_ack_returns_after_ack(shared_participant, test_type):
    writer_protocol = dds.DataWriterProtocol()
    writer_protocol.rtps_reliable_writer.fast_heartbeat_period = dds.Duration.from_milliseconds(
        10)
    writer_protocol.rtps_reliable_writer.heartbeat_period = dds.Duration.from_milliseconds(
        10)
    writer_protocol.rtps_reliable_writer.late_joiner_heartbeat_period = dds.Duration.from_milliseconds(
        10)
    pubsub = PubSubFixture(
        shared_participant,
        test_type(),
        writer_policies=[writer_protocol])

    pubsub.writer.write(get_sample_value(pubsub.data_type))
    pubsub.writer.wait_for_acknowledgments(dds.Duration(10, 0))


@pytest.mark.parametrize("ack_response_data", [True, False])
def test_is_sample_app_acknowledged(ack_response_data, shared_participant):
    callback_count = 0
    received_ack_info: dds.AcknowledgmentInfo = None

    class AppAckListener(dds.NoOpDataWriterListener):
        def on_application_acknowledgment(self, writer, info):
            nonlocal callback_count, received_ack_info
            callback_count += 1
            received_ack_info = info

    topic = dds.Topic(shared_participant, "TestTopic", KeyedString)

    app_ack_reliability = dds.Reliability.reliable()
    app_ack_reliability.acknowledgment_kind = dds.AcknowledgmentKind.APPLICATION_EXPLICIT
    writer_qos = dds.DataWriterQos()

    writer_qos.reliability.kind = dds.ReliabilityKind.RELIABLE
    writer_qos.history.kind = dds.HistoryKind.KEEP_ALL
    writer_qos.durability.kind = dds.DurabilityKind.TRANSIENT_LOCAL
    writer_qos << app_ack_reliability

    writer_property = dds.Property()
    writer_property.set(
        "dds.data_writer.ack_mode_in_wait_for_acknowledgments", "APP_ACK")
    writer_qos.property = writer_property
    writer = dds.DataWriter(
        dds.Publisher(shared_participant),
        topic,
        writer_qos,
        AppAckListener(),
        dds.StatusMask.DATAWRITER_APPLICATION_ACKNOWLEDGMENT)

    reader_qos = dds.DataReaderQos()
    reader_qos.reliability.kind = dds.ReliabilityKind.RELIABLE
    reader_qos.history.kind = dds.HistoryKind.KEEP_ALL
    reader_qos.durability.kind = dds.DurabilityKind.TRANSIENT_LOCAL
    reader_qos << app_ack_reliability
    reader_qos.data_reader_resource_limits.max_app_ack_response_length = 10

    reader = dds.DataReader(dds.Subscriber(
        shared_participant), topic, reader_qos)
    wait.for_discovery(reader, writer)
    writer.write([KeyedString("key1", "value1"), KeyedString(
        "key2", "value2"), KeyedString("key3", "value3")])
    wait.for_data(reader, 3)
    samples = reader.take()
    assert len(samples) == 3
    assert not writer.is_sample_app_acknowledged(
        samples[1][1].original_publication_virtual_sample_identity)

    response_data = Bytes()
    response_data.value.append(3)
    response_data.value.append(4)
    response_data.value.append(8)
    response_data.value.append(9)
    response = dds.AckResponseData()
    response.value = response_data.value

    if ack_response_data:
        reader.acknowledge_sample(samples[1][1], response)
    else:
        reader.acknowledge_sample(samples[1][1])

    writer._wait_for_sample_acknowledgment(
        samples[1][1].original_publication_virtual_sample_identity,
        dds.Duration(10, 0))
    wait.until_equal(1, lambda: callback_count)
    assert received_ack_info is not None
    assert received_ack_info.subscription_handle == reader.instance_handle
    assert received_ack_info.sample_identity == samples[
        1][1].original_publication_virtual_sample_identity
    assert received_ack_info.cookie == dds.Cookie()
    assert received_ack_info.valid_response_data
    if ack_response_data:
        received_ack_info.response_data == response

    assert writer.is_sample_app_acknowledged(
        samples[1][1].original_publication_virtual_sample_identity)
    callback_count = 0
    if ack_response_data:
        reader.acknowledge_sample(samples[1][1], response)
        response.value.append(1)
        response.value.append(2)
        response.value.append(6)
        response.value.append(7)
        reader.acknowledge_all(response)
    else:
        reader.acknowledge_all()
    wait.until_equal(2, lambda: callback_count)
    assert received_ack_info is not None
    assert received_ack_info.subscription_handle == reader.instance_handle
    assert received_ack_info.sample_identity == samples[
        2][1].original_publication_virtual_sample_identity
    assert received_ack_info.valid_response_data
    if ack_response_data:
        assert received_ack_info.response_data.value == response.value


def test_wait_for_async_publisher(shared_participant):
    topic = dds.Topic(shared_participant, "TestTopic", String)
    reader = dds.DataReader(dds.Subscriber(shared_participant), topic)
    writer = dds.DataWriter(dds.Publisher(shared_participant), topic)
    wait.for_discovery(reader, writer)

    writer.write(String("Hello World"))
    writer.wait_for_asynchronous_publishing(dds.Duration(10))
    wait.for_data(reader, 1)
    assert reader.read_data() == [String("Hello World")]


def test_writer_cast(shared_participant):
    fixture = PubSubFixture(shared_participant, PointIDL, create_reader=False)
    writer = fixture.writer
    any_writer = dds.AnyDataWriter(writer)
    assert writer == dds.DataWriter(any_writer)


def test_writer_exit(shared_participant):
    fixture = PubSubFixture(shared_participant, PointIDL,
                            create_reader=False, create_writer=False)
    with dds.DataWriter(fixture.publisher, fixture.topic) as writer:
        assert writer is not None
        assert not writer.closed
        # By using a with statement we can ensure writer.__exit__ is called
    assert writer.closed
