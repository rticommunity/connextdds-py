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
from rti.types.builtin import String, KeyedString

import pytest
from rti.idl_impl.test_utils import wait
from test_utils.fixtures import *


# --- Helpers and fixtures ----------------------------------------------------
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
        lambda: String
    ]


def get_keyed_types_generator():
    return [
        lambda: PointIDL,
        lambda: idl.get_type_support(PointIDLForDD).dynamic_type,
        lambda: KeyedString
    ]


def get_sample_value(type: type, not_selected=False):
    if not_selected:
        x = 2
    else:
        x = 1
    y = 2
    if type is PointIDL:
        return PointIDL(x=x, y=y)
    elif type == idl.get_type_support(PointIDLForDD).dynamic_type:
        point_dynamic_data = dds.DynamicData(
            idl.get_type_support(PointIDLForDD).dynamic_type)
        point_dynamic_data["x"] = x
        point_dynamic_data["y"] = y
        return point_dynamic_data
    elif type is String:
        if not_selected:
            return String("not_selected")
        return String("Hello World")
    elif type is KeyedString:
        if not_selected:
            return KeyedString("not_selected", "not_selected")
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


def get_selector_query_str(type: type):
    if type is PointIDL:
        return "x = 1"
    elif type == idl.get_type_support(PointIDLForDD).dynamic_type:
        return "x = 1"
    elif type is String:
        return "value = 'Hello World'"
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


def same_elements(a, b):
    return len(a) == len(b) and all(x in b for x in a)


def check_expected_data(reader: dds.DataReader, expected_samples: list):
    wait.for_data(reader, count=len(expected_samples))
    received = reader.take()
    assert same_elements([data for (data, _) in received], expected_samples)


@pytest.fixture(scope="function", params=get_test_types_generator())
def pubsub_idl_dd_builtin_no_samples(request, shared_participant):
    """This fixture provides a test with a writer and a reader that have already
       been discovered. The participant is shared within the module to speed up
       execution, but the contained entities are deleted after each test
       function."""

    data_type = request.param()
    fixture = PubSubFixture(shared_participant, data_type)
    return fixture


@pytest.fixture(scope="function", params=get_keyed_types_generator())
def pubsub_keyed_idl_dd_builtin_no_samples(request, shared_participant):
    """This fixture provides a test with a writer and a reader that have already
       been discovered. The participant is shared within the module to speed up
       execution, but the contained entities are deleted after each test
       function."""

    data_type = request.param()
    fixture = PubSubFixture(shared_participant, data_type)
    return fixture


@pytest.fixture(scope="function")
def pubsub_idl_point_with_data(shared_participant):
    """ This fixture is for tests where we want 3 samples published and want 
    their instance handles. """

    fixture = PubSubFixture(shared_participant, PointIDL)
    samples = [PointIDL(x=1, y=2), PointIDL(x=3, y=4), PointIDL(x=5, y=6)]

    fixture.writer.write(samples)
    instances = [fixture.writer.lookup_instance(sample) for sample in samples]
    wait.for_data(fixture.reader, count=len(samples))
    return (fixture, instances)


@pytest.fixture(scope="function")
def pubsub_idl_point_no_samples(shared_participant):
    """ This fixture is for when we want to have a pubsub but not publish 
    any samples at the start."""
    fixture = PubSubFixture(shared_participant, PointIDL)
    return fixture


class PointListener(dds.NoOpDataReaderListener):
    def on_data_available(self, reader):
        pass


class OtherPointListener(dds.NoOpDataReaderListener):
    def on_data_available(self, reader):
        pass


def get_reader_listeners(type: type):
    if type is PointIDL:
        return (PointListener(), OtherPointListener())
    elif type == idl.get_type_support(PointIDLForDD).dynamic_type:
        return (dds.DynamicData.NoOpDataReaderListener(), dds.DynamicData.NoOpDataReaderListener())
    elif type is String:
        return (dds.NoOpDataReaderListener(), dds.NoOpDataReaderListener())
    else:
        raise TypeError("Unsupported type: {}".format(type))


def get_reader_w_listeners(participant, topic, type, listener):
    ns = test_utils._get_topic_namespace(topic, type)
    return ns.DataReader(participant, topic, dds.DataReaderQos(), listener)


# --- Listener tests ---------------------------------------------------------


def test_datareader_listener_can_be_set(pubsub_idl_dd_builtin_no_samples):

    pubsub = pubsub_idl_dd_builtin_no_samples
    listener, other_listener = get_reader_listeners(pubsub.data_type)

    assert pubsub.reader.listener is None

    pubsub.reader.listener = listener
    assert pubsub.reader.listener == listener

    # Test set_listener fn
    pubsub.reader.set_listener(other_listener, dds.StatusMask.ALL)
    assert pubsub.reader.listener == other_listener

    # Test it can be set to none
    pubsub.reader.listener = None
    assert pubsub.reader.listener is None

    pubsub.reader.listener = get_reader_listeners(pubsub.data_type)[0]
    assert isinstance(pubsub.reader.listener,
                      (dds.DataReaderListener, dds.DynamicData.DataReaderListener, dds.DataReaderListener))

    # Test constructor
    new_reader = get_reader_w_listeners(
        pubsub.participant, pubsub.topic, pubsub.data_type, listener)

    assert new_reader.listener == listener
    new_reader.close()


def test_datareader_on_data_available_listener_is_invoked(pubsub_idl_point_no_samples):
    pubsub = pubsub_idl_point_no_samples

    class TestListener(dds.NoOpDataReaderListener):
        on_data_available_called = False

        def on_data_available(self, reader):
            assert len(reader.take()) == 1
            self.on_data_available_called = True

    pubsub.reader.listener = TestListener()
    pubsub.writer.write(PointIDL(x=1, y=2))

    wait.until(lambda: pubsub.reader.listener.on_data_available_called)


def test_datareader_on_requested_incompatible_qos_listener_is_invoked(shared_participant):

    topic = dds.Topic(shared_participant, "listener_test_topic", PointIDL)
    writer_qos = dds.DataWriterQos()
    writer_qos << dds.Reliability(dds.ReliabilityKind.BEST_EFFORT)
    reader_qos = dds.DataReaderQos()
    reader_qos << dds.Reliability(dds.ReliabilityKind.RELIABLE)
    pub = dds.Publisher(shared_participant)
    sub = dds.Subscriber(shared_participant)

    class TestListener(dds.NoOpDataReaderListener):
        on_requested_incompatible_qos_called = False

        def on_requested_incompatible_qos(self, reader, status):
            assert status.last_policy is dds.Reliability
            self.on_requested_incompatible_qos_called = True

    # Create a writer with incompatible qos
    reader = dds.DataReader(sub, topic, reader_qos, TestListener())
    writer = dds.DataWriter(pub, topic, writer_qos)

    wait.until(lambda: reader.listener.on_requested_incompatible_qos_called)


# --- Reader tests -------------------------------------------------------------


def test_datareader_key_value_and_lookup_instance(pubsub_keyed_idl_dd_builtin_no_samples):
    pubsub = pubsub_keyed_idl_dd_builtin_no_samples
    sample = get_sample_value(pubsub.data_type)
    pubsub.writer.write(sample)
    wait.for_data(pubsub.reader, count=1)
    instance = pubsub.reader.lookup_instance(sample)
    assert instance != dds.InstanceHandle.nil()
    result = pubsub.reader.key_value(instance)
    key_holder = get_sample_keys(pubsub.data_type)
    assert keys_equal(result, key_holder)
    assert instance == pubsub.reader.lookup_instance(result)
    assert hash(instance) == hash(pubsub.reader.lookup_instance(result))


def test_selector(pubsub_idl_dd_builtin_no_samples):
    pubsub = pubsub_idl_dd_builtin_no_samples
    selector = pubsub.reader.select()
    selector.content(
        dds.Query(pubsub.reader, get_selector_query_str(pubsub.data_type)))

    sample_to_be_selected = get_sample_value(pubsub.data_type)
    sample_to_be_rejected = get_sample_value(pubsub.data_type, True)
    # Write a sample that will be selected and one that will be rejected
    pubsub.writer.write(sample_to_be_selected)
    pubsub.writer.write(sample_to_be_rejected)
    wait.until_equal(2, lambda: len(pubsub.reader.read()))
    wait.until_equal(1, lambda: len(selector.read()))
    received = selector.take()
    same_elements([data for (data, _) in received], [sample_to_be_selected])
    received = pubsub.reader.take()
    same_elements([data for (data, _) in received], [
                  sample_to_be_selected, sample_to_be_rejected])


# --- IDL selector tests ---------------------------------------------------------

def test_max_samples(pubsub_idl_point_with_data):
    pubsub, _ = pubsub_idl_point_with_data

    # Set the max samples to 1
    assert 1 == len(pubsub.reader.select().max_samples(1).read())
    assert 2 == len(pubsub.reader.select().max_samples(2).read())
    assert 3 == len(pubsub.reader.select().max_samples(10).read())
    assert 2 == len(pubsub.reader.select().max_samples(2).take())
    assert 1 == len(pubsub.reader.select().max_samples(1).take())
    assert 0 == len(pubsub.reader.select().max_samples(0).take())


def test_take_selector_returns_and_removes_samples(pubsub_idl_point_with_data):
    pubsub, _ = pubsub_idl_point_with_data

    # Make sure read doesn't modify the queue
    assert 3 == len(pubsub.reader.select().read())
    assert 3 == len(pubsub.reader.select().read())

    # Make sure reader and selector are the same
    assert pubsub.reader.read_data() == pubsub.reader.select().read_data()

    received = pubsub.reader.select().take()
    assert 3 == len(received)
    # make sure read and take get nothing
    assert 0 == len(pubsub.reader.take())
    assert 0 == len(pubsub.reader.read())


def test_selector_datastate_filters(pubsub_idl_point_with_data):
    pubsub, _ = pubsub_idl_point_with_data

    # Nothing should be read yet
    assert 0 == len(pubsub.reader.select().state(dds.SampleState.READ).read())
    assert 1 == len(pubsub.reader.select().state(
        dds.SampleState.ANY).max_samples(1).read())
    pubsub.reader.select().max_samples(10)
    assert 1 == len(pubsub.reader.select().state(dds.SampleState.READ).read())
    assert 2 == len(pubsub.reader.select().state(
        dds.SampleState.NOT_READ).read())

    assert 3 == len(pubsub.reader.select().state(
        dds.SampleState.ANY).max_samples(10).read_data())


def test_selector_can_combine_states(pubsub_idl_point_with_data):
    pubsub, instances = pubsub_idl_point_with_data

    pubsub.writer.dispose_instance(instances[0])
    pubsub.writer.dispose_instance(instances[1])
    # Need to use wait.for_samples here because the the dispose will not trigger
    # the wait to end if we use wait.for_data
    wait.for_samples(pubsub.reader, 5)

    assert 4 == len(pubsub.reader.select().state(
        dds.DataState(
            dds.SampleState.READ,
            dds.ViewState.ANY,
            dds.InstanceState.NOT_ALIVE_DISPOSED)).read())
    assert 0 == len(pubsub.reader.select().state(
        dds.DataState(
            dds.SampleState.NOT_READ,
            dds.ViewState.ANY,
            dds.InstanceState.NOT_ALIVE_DISPOSED)).read())
    assert 1 == len(pubsub.reader.select().state(
        dds.DataState(
            dds.SampleState.READ,
            dds.ViewState.ANY,
            dds.InstanceState.ALIVE)).read())


def test_select_filters_by_instance(pubsub_idl_point_with_data):
    pubsub, instances = pubsub_idl_point_with_data

    # Publish another sample with the same key as the second sample (x=3)
    pubsub.writer.write(PointIDL(x=3, y=100))
    wait.for_data(pubsub.reader, 4)

    assert 1 == len(pubsub.reader.select().instance(instances[0]).read())
    assert 2 == len(pubsub.reader.select().instance(instances[1]).read())


def test_selector_allows_iterating_by_next_instance(pubsub_idl_point_with_data):
    pubsub, _ = pubsub_idl_point_with_data

    seen_instances = []
    next_instance = dds.InstanceHandle.nil()

    while True:
        received = pubsub.reader.select().next_instance(next_instance).read()
        if len(received) == 0:
            break
        current_instance = received[0][1].instance_handle
        seen_instances.append(current_instance)
        assert next_instance != current_instance
        next_instance = current_instance

    assert len(seen_instances) == 3


def test_selector_allows_iterating_by_next_instance_with_state(pubsub_idl_point_no_samples):
    pubsub = pubsub_idl_point_no_samples
    sample1 = PointIDL(x=1, y=2)
    sample2 = PointIDL(x=3, y=4)
    sample3 = PointIDL(x=5, y=6)
    pubsub.writer.write([sample1, sample2, sample3])
    instance1 = pubsub.writer.lookup_instance(sample1)
    instance2 = pubsub.writer.lookup_instance(sample2)
    instance3 = pubsub.writer.lookup_instance(sample3)
    seen_instances = []
    next_instance = dds.InstanceHandle.nil()
    x_is_1_or_5 = dds.QueryCondition(
        dds.Query(pubsub.reader, "x = 1 or x = 5"), dds.DataState.any)
    while True:
        samples = pubsub.reader.select().condition(
            x_is_1_or_5).next_instance(next_instance).read()
        if len(samples) == 0:
            break
        current_instance = samples[0][1].instance_handle
        seen_instances.append(current_instance)
        assert next_instance != current_instance
        next_instance = current_instance

    assert same_elements(seen_instances, [instance1, instance3])


def test_select_instance_and_next_instance_are_mutually_exclusive(pubsub_idl_point_with_data):
    pubsub, instances = pubsub_idl_point_with_data

    selector = pubsub.reader.select().next_instance(instances[1])
    selector.instance(instances[0])
    received = selector.read_data()
    assert 1 == len(received)
    assert PointIDL(x=1, y=2) == received[0]
    assert 1 == len(selector.read())
    assert 1 == len(selector.take())
    assert 0 == len(selector.take())


def test_select_with_max_samples_and_datastate_filters_samples(pubsub_idl_point_with_data):
    pubsub, _ = pubsub_idl_point_with_data
    assert 1 == len(pubsub.reader.select().max_samples(1).read())
    assert 1 == len(pubsub.reader.select().max_samples(
        1).state(dds.SampleState.NOT_READ).read())
    assert 1 == len(pubsub.reader.select().max_samples(
        1).state(dds.SampleState.READ).read())
    assert 2 == len(pubsub.reader.select().max_samples(
        2).state(dds.SampleState.READ).read())
    assert 2 == len(pubsub.reader.select().max_samples(
        3).state(dds.SampleState.READ).read())


def test_selector_can_be_reused_with_new_max_samples(pubsub_idl_point_with_data):
    pubsub, _ = pubsub_idl_point_with_data
    selector = pubsub.reader.select()
    assert 1 == len(selector.max_samples(1).read())
    assert 1 == len(selector.read())
    assert 2 == len(pubsub.reader.select().max_samples(2).read())


def test_select_with_read_condition_applies_to_state(pubsub_idl_point_with_data):
    pubsub, _ = pubsub_idl_point_with_data

    condition = dds.ReadCondition(pubsub.reader, dds.SampleState.READ)
    assert 0 == len(pubsub.reader.select().condition(condition).read())
    pubsub.reader.select().condition(dds.ReadCondition(
        pubsub.reader, dds.SampleState.NOT_READ))
    assert 2 == len(pubsub.reader.select().max_samples(2).read())
    assert 1 == len(pubsub.reader.select().max_samples(
        1).condition(condition).read())
    assert 2 == len(pubsub.reader.select().max_samples(
        2).condition(condition).read())


def test_select_with_query_condition_applies_filter(pubsub_idl_point_no_samples):
    pubsub = pubsub_idl_point_no_samples
    x_is_1 = dds.QueryCondition(
        dds.Query(pubsub.reader, "x = 1"), dds.DataState.any)
    x_greater_than_3 = dds.QueryCondition(
        dds.Query(pubsub.reader, "x > 3"), dds.DataState.any)

    sample1 = PointIDL(x=1, y=2)
    sample2 = PointIDL(x=1, y=4)
    sample3 = PointIDL(x=5, y=6)
    pubsub.writer.write([sample1, sample2, sample3])
    wait.for_data(pubsub.reader, 3)
    assert 2 == len(pubsub.reader.select().condition(x_is_1).read())
    assert 1 == len(pubsub.reader.select().condition(x_greater_than_3).read())


def test_select_with_query_condition_and_instance_handle_applies_both(pubsub_idl_point_no_samples):
    pubsub = pubsub_idl_point_no_samples
    y_is_2 = dds.QueryCondition(
        dds.Query(pubsub.reader, "y = 2"), dds.DataState.any)

    samples = [PointIDL(x=1, y=2), PointIDL(x=2, y=2),
               PointIDL(x=2, y=6), PointIDL(x=6, y=7)]
    # Two samples with the same instance handle, 1 and 2
    # Two samples will match query, samples 0 and 1
    # One sample will match both, sample 1
    # One sample will not match
    pubsub.writer.write(samples)
    wait.for_data(pubsub.reader, 4)
    instance = pubsub.writer.lookup_instance(samples[1])
    assert instance != dds.InstanceHandle.nil()

    selector = pubsub.reader.select().condition(y_is_2).instance(instance)

    assert 1 == len(selector.read())
    assert 2 == len(selector.instance(dds.InstanceHandle.nil()).read())
    assert 1 == len(selector.instance(instance).read())
    # Clear it and confirm that you can get both samples again
    assert 2 == len(selector.condition(None).read())


def test_select_instance_not_returning_properly(pubsub_idl_point_no_samples):
    pubsub = pubsub_idl_point_no_samples
    y_is_2 = dds.QueryCondition(
        dds.Query(pubsub.reader, "y = 2"), dds.DataState.any)

    samples = [PointIDL(x=1, y=2), PointIDL(x=2, y=2),
               PointIDL(x=5, y=6), PointIDL(x=6, y=7)]
    pubsub.writer.write(samples)
    wait.for_data(pubsub.reader, 4)
    instance = pubsub.writer.lookup_instance(samples[1])
    assert instance != dds.InstanceHandle.nil()

    selector = pubsub.reader.select()
    selector.condition(y_is_2).instance(instance)

    # make sure that only one is selected
    assert 1 == len(selector.read())


def test_reader_cast(pubsub_idl_point_no_samples):
    reader = pubsub_idl_point_no_samples.reader
    any_reader = dds.AnyDataReader(reader)
    assert reader == dds.DataReader(any_reader)
