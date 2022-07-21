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
        lambda: dds.StringTopicType,
        lambda: dds.KeyedStringTopicType,
    ]


def get_keyed_types_generator():
    return [
        lambda: PointIDL,
        lambda: idl.get_type_support(PointIDLForDD).dynamic_type,
        lambda: dds.KeyedStringTopicType,
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
        shared_participant, data_type, create_reader=False)
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
    elif type is dds.StringTopicType:
        return dds.StringTopicType("Hello World")
    elif type is dds.KeyedStringTopicType:
        return dds.KeyedStringTopicType("Hello World", "Hello World")
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
    elif type is dds.KeyedStringTopicType:
        return dds.KeyedStringTopicType("Hello World", "")
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
    elif type(a) is dds.KeyedStringTopicType:
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
    elif type is dds.StringTopicType:
        return (dds.StringTopicType.NoOpDataWriterListener(),
                dds.StringTopicType.NoOpDataWriterListener())
    elif type is dds.KeyedStringTopicType:
        return (dds.KeyedStringTopicType.NoOpDataWriterListener(),
                dds.KeyedStringTopicType.NoOpDataWriterListener())
    else:
        raise TypeError(f"Unsupported type: {type}")


def get_writer_w_listeners(participant, topic, type, listener):
    ns = test_utils._get_topic_namespace(topic, type)
    return ns.DataWriter(participant, topic, dds.DataWriterQos(), listener)


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
                       dds.DynamicData.DataWriterListener,
                       dds.StringTopicType.DataWriterListener,
                       dds.KeyedStringTopicType.DataWriterListener))

    # Test constructor
    new_writer = get_writer_w_listeners(
        pubsub.participant, pubsub.topic, pubsub.data_type, listener)
    assert new_writer.listener == listener
    new_writer.close()

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


# --- Key Value tests ---------------------------------------------------------

def test_key_value(pub):
    if pub.data_type == dds.KeyedStringTopicType:
        pytest.skip("TODO: KeyedStringTopicType has known bug")
    sample = get_sample_value(pub.data_type)
    instance = pub.writer.register_instance(sample)
    assert instance != dds.InstanceHandle.nil()
    result = pub.writer.key_value(instance)
    key_holder = get_sample_keys(pub.data_type)
    assert keys_equal(result, key_holder)
    assert instance == pub.writer.lookup_instance(result)

# --- Other tests -------------------------------------------------------------


def test_writer_cast(shared_participant):
    fixture = PubSubFixture(shared_participant, PointIDL, create_reader=False)
    writer = fixture.writer
    any_writer = dds.AnyDataWriter(writer)
    assert writer == dds.DataWriter(any_writer)
