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

import typing
import pytest

import rti.idl as idl
import rti.connextdds as rti
from rti.idl_impl.test_utils import *

import test_utils.wait as wait
from test_utils.fixtures import *

import common_types

def same_elements(a, b):
    return len(a) == len(b) and all(x in b for x in a)


def check_expected_data(reader: rti.DataReader, expected_samples: list):
    wait.for_data(reader, count=len(expected_samples))
    samples = reader.take_data()
    assert same_elements(samples, expected_samples)


def check_data_and_get_info(reader: rti.DataReader, expected_samples: list):
    wait.for_data(reader, count=len(expected_samples))
    samples = reader.take()
    assert same_elements([data for (data, _) in samples], expected_samples)
    return [info for (_, info) in samples]


# The types that we will be testing
idl_types = [
    common_types.Point,
    common_types.Point3D,
    common_types.Line,
    common_types.AnnotationsTest,
    common_types.ComplexSequence,
    common_types.SequenceArrayTest,
    common_types.BoundSequenceArrayTest,
    common_types.Person,
    common_types.BoundString,
    common_types.UnboundString,
    common_types.OptionalsTest,
]


class IdlTypeFixture:
    def __init__(self, sample_type, gen_fn=None):
        self.sample_type = sample_type
        self.default_value = sample_type()
        self.dynamic_type = idl.get_type_support(sample_type).dynamic_type
        if gen_fn is None:
            self.create_test_data = IdlValueGenerator(
                sample_type).create_test_data
        else:
            self.create_test_data = gen_fn

    def create_pubsub_fixture(self, participant, content_filter=None) -> PubSubFixture:
        return PubSubFixture(participant, self.sample_type, content_filter=content_filter)


@pytest.fixture(params=idl_types)
def type_fixture(request) -> IdlTypeFixture:
    return IdlTypeFixture(request.param)


types_with_expected_size = [
    (common_types.Point, 20),
    (common_types.Point3D, 28),
    (common_types.Line, 36),
    (common_types.ComplexSequence, 44)
]


@pytest.fixture(params=types_with_expected_size)
def type_size_fixture(request):
    return (IdlTypeFixture(request.param[0]), request.param[1])


seeds = [
    0,
    1,
    2,
    3,
    5,
    99,
    100,
    101,
    122,
    123,
    1000,
]


def test_generator_function(type_fixture: IdlTypeFixture):
    for seed in seeds:
        value = type_fixture.create_test_data(seed=seed)
        assert value is not None


def test_default_contructed_sample_can_be_serialized(type_fixture: IdlTypeFixture):
    ts = idl.get_type_support(type_fixture.sample_type)
    value = type_fixture.default_value
    assert value == ts.deserialize(ts.serialize(value))


def test_samples_with_different_test_values_can_be_serialized(type_fixture: IdlTypeFixture):
    ts = idl.get_type_support(type_fixture.sample_type)
    for seed in seeds:
        value = type_fixture.create_test_data(seed=seed)
        assert value == ts.deserialize(ts.serialize(value))


def test_types_can_be_converted_to_string(type_fixture: IdlTypeFixture):
    value = type_fixture.default_value
    string = str(value)
    assert string is not None
    assert "object at" not in string  # ensure str() doesn't use default impl


def test_types_can_be_converted_to_dynamic_data(type_fixture: IdlTypeFixture):
    value = type_fixture.create_test_data(seed=1)
    ts = idl.get_type_support(type_fixture.sample_type)
    dt = ts.to_dynamic_data(value)
    new_value = ts.from_dynamic_data(dt)
    assert value == new_value


@pytest.mark.skip(reason="Waiting for nddsgen types")
def test_cannot_deserialize_non_compatible_type():
    @idl.struct()
    class NotTheRightType:
        x: int = 0
    value = NotTheRightType()

    NotTheRightType_ts = idl.get_type_support(NotTheRightType)
    Point_ts = idl.get_type_support(common_types.Point3D)
    buffer = NotTheRightType_ts.serialize(value)

    with pytest.raises(TypeError):
        Point_ts.serialize(value)

    with pytest.raises(TypeError):
        Point_ts.deserialize(buffer)


@pytest.mark.skip(reason="Ranges not implemented in idl yet")
def test_cannot_deserialize_sample_out_of_range():
    ranged_point = common_types.RangedPoint()
    # First assert that the ranged point works by default
    ts = idl.get_type_support(common_types.RangedPoint)
    assert ranged_point == ts.deserialize(ts.serialize(ranged_point))

    ranged_point.x = 11  # Greatest value is 10
    with pytest.raises(TypeError):
        ts.deserialize(ts.serialize(ranged_point))

    ranged_point.x = 0  # Lowest value is 1
    with pytest.raises(TypeError):
        ts.deserialize(ts.serialize(ranged_point))


def test_cannot_deserialize_sample_with_out_of_bounds_string():
    bound_string = common_types.BoundString()
    ts = idl.get_type_support(common_types.BoundString)
    bound_string.value = "ab"
    with pytest.raises(idl.FieldSerializationError):
        bound_string.value = "abcde"
        ts.deserialize(ts.serialize(bound_string))


def test_serialization_routines(type_fixture: IdlTypeFixture):
    value = type_fixture.default_value
    ts = idl.get_type_support(type_fixture.sample_type)
    buffer = ts.serialize(value)
    assert ts.get_serialized_sample_size(value) == len(buffer)
    assert ts.get_serialized_sample_size(
        value) <= ts.max_serialized_sample_size


def test_serialized_sizes(type_size_fixture: typing.Tuple[IdlTypeFixture, int]):
    expected_size = type_size_fixture[1]
    type_fixture = type_size_fixture[0]
    ts = idl.get_type_support(type_fixture.sample_type)
    # If you change the seed you will have to change the expected sizes since
    # sequence lengths are seed dependant
    assert len(ts.serialize(type_fixture.create_test_data(seed=1))
               ) == expected_size


def test_pub_and_sub_communicate_using_idl_types_default(type_fixture: IdlTypeFixture, shared_participant):
    pubsub = type_fixture.create_pubsub_fixture(shared_participant)
    pubsub.send_and_check(type_fixture.default_value)


def test_pub_and_sub_communicate_using_idl_types_seeded_value(type_fixture: IdlTypeFixture, shared_participant):
    pubsub = type_fixture.create_pubsub_fixture(shared_participant)
    for seed in seeds:
        value = type_fixture.create_test_data(seed=seed)
        pubsub.send_and_check(value)


def test_pub_and_sub_communicate_using_idl_types_with_cft(type_fixture: IdlTypeFixture, shared_participant):
    pubsub = type_fixture.create_pubsub_fixture(
        shared_participant, content_filter="1=1")
    for seed in seeds:
        value = type_fixture.create_test_data(seed=seed)
        pubsub.send_and_check(value)


def test_idl_types_interoperate_with_dynamic_data(type_fixture: IdlTypeFixture, shared_participant):
    pubsub = type_fixture.create_pubsub_fixture(shared_participant)
    ts = idl.get_type_support(type_fixture.sample_type)
    dd_participant = dds.DomainParticipant(0)

    dd_topic = dds.DynamicData.Topic(
        dd_participant, f"Example {type_fixture.sample_type}", type_fixture.dynamic_type)

    writer_qos = dd_participant.implicit_publisher.default_datawriter_qos
    writer_qos << dds.Reliability.reliable()
    writer_qos << dds.History.keep_all
    writer_qos.property['dds.data_writer.history.memory_manager.fast_pool.pool_buffer_max_size'] = str(
        1000)
    dd_writer = dds.DynamicData.DataWriter(
        dds.Publisher(dd_participant), dd_topic, writer_qos)

    reader_qos = dd_participant.implicit_subscriber.default_datareader_qos
    reader_qos << dds.Reliability.reliable()
    reader_qos << dds.History.keep_all
    dd_reader = dds.DynamicData.DataReader(
        dds.Subscriber(dd_participant), dd_topic, reader_qos)

    wait.for_discovery(dd_reader, pubsub.writer)
    wait.for_discovery(pubsub.reader, dd_writer)

    pubsub.writer.write(type_fixture.default_value)

    # Using this since python doesn't have multiline lambdas
    def get_sent_value():
        with dd_reader.read() as samples:
            for sample, _ in samples:
                return sample

    wait.until_equal(
        ts.to_dynamic_data(type_fixture.default_value), get_sent_value)

    dd_writer.write(ts.to_dynamic_data(type_fixture.default_value))
    check_expected_data(
        pubsub.reader, [type_fixture.default_value, type_fixture.default_value])

    dd_participant.close_contained_entities()
    dd_participant.close()


@pytest.mark.skip(reason="to be implemented")
def test_get_key_value_returns_keys_for_registered_instance(type_fixture: IdlTypeFixture):
    pass


@pytest.mark.skip(reason="to be implemented")
def test_idl_types_generate_same_keyhashes_as_dynamicdata(type_fixture: IdlTypeFixture):
    ts = idl.get_type_support(type_fixture.sample_type)

    idl_topic = 1
    idl_writer = 1
    dd_topic = 1
    dd_writer = 1
    for seed in seeds:
        value = type_fixture.create_test_data(seed=seed)
