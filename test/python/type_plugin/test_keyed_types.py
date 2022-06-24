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
import rti.connextdds as dds
from rti.idl_impl.test_utils import *
from test_utils.fixtures import *
from dataclasses import field

seeds = [0,
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


@idl.struct(member_annotations={"x": [idl.key]})
class A_keyed:
    x: idl.int32 = 11


@idl.struct()
class B:
    x: A_keyed = A_keyed(22)


@idl.struct(member_annotations={"x": [idl.key]})
class C_keyed:
    x: B = B(A_keyed(33))


@idl.struct()
class D:
    x: C_keyed = C_keyed(B(A_keyed(44)))


@idl.struct(member_annotations={"key": [idl.key]})
class InnerType:
    key: idl.int32 = 55
    value: idl.int32 = 999


@idl.struct()
class MiddleType1:
    x: idl.int32 = 66
    y: InnerType = InnerType(44, 99)


@idl.struct(member_annotations={"x": [idl.key]})
class MiddleType2:
    x: idl.int32 = 77
    y: InnerType = InnerType(88, -22)


@idl.struct(member_annotations={"a": [idl.key]})
class OuterType1:
    a: MiddleType1 = MiddleType1(-11, InnerType(22, 33))
    b: idl.int32 = 111


@idl.struct(member_annotations={"key": [idl.key], "not_key": [idl.bound(7)]})
class StringKeyType:
    key: str = "key"
    not_key: str = "not_key"


@idl.struct(member_annotations={"key": [idl.key, idl.bound(5)],
                                "not_key": [idl.bound(7)]})
class SeqKeyType:
    key: typing.Sequence[idl.int16] = field(default_factory=list)
    not_key: typing.Sequence[idl.int16] = field(default_factory=list)


@idl.struct
class Point:
    a: int = 0
    b: int = 0


@idl.struct(member_annotations={"b": [idl.key]})
class PointSeq:
    a: int = 0
    b: typing.Sequence[Point] = field(default_factory=list)


@idl.struct(member_annotations={"a_k": [idl.key], "b_k": [idl.key]})
class MultiKeyType:
    a_u: Point = field(default_factory=Point)
    a_k: Point = field(default_factory=Point)
    b_u: PointSeq = field(default_factory=PointSeq)
    b_k: typing.Sequence[PointSeq] = field(default_factory=list)


def get_inner_x_value(struct):
    if type(struct) is int:
        return struct
    else:
        return get_inner_x_value(struct.x)


class IdlTypeFixture:
    def __init__(self, sample_type, gen_fn=None, keys_only=True):
        self.sample_type = sample_type
        self.default_value = sample_type()
        self.dynamic_type = idl.get_type_support(sample_type).dynamic_type
        if gen_fn is None:
            self.create_test_data = IdlValueGenerator(
                sample_type, keys_only=keys_only).create_test_data
        else:
            self.create_test_data = gen_fn

    def create_pubsub_fixture(self, participant, content_filter=None) -> PubSubFixture:
        return PubSubFixture(participant, self.sample_type, content_filter=content_filter)


types_to_test = [
    A_keyed,
    C_keyed,
    InnerType,
    MiddleType2,
    OuterType1,
    StringKeyType,
    SeqKeyType,
    MultiKeyType,
]


@pytest.fixture(params=types_to_test)
def type_fixture(request) -> IdlTypeFixture:
    return IdlTypeFixture(request.param, keys_only=True)


def test_generate_expected_values_keys_only():
    assert get_inner_x_value(IdlValueGenerator(
        A_keyed, keys_only=True).create_test_data()) == 0
    assert get_inner_x_value(IdlValueGenerator(
        B, keys_only=True).create_test_data()) == 0
    assert get_inner_x_value(IdlValueGenerator(
        C_keyed, keys_only=True).create_test_data()) == 0
    assert get_inner_x_value(IdlValueGenerator(
        D, keys_only=True).create_test_data()) == 0


def test_nested_key_generation():
    inner_value = IdlValueGenerator(
        InnerType, keys_only=True).create_test_data(1)
    assert inner_value.key == 1  # Generated
    assert inner_value.value == 999

    middle_value = IdlValueGenerator(
        MiddleType1, keys_only=True).create_test_data(1)
    assert middle_value.x == 1  # Generated
    assert middle_value.y.key == 2  # Generated
    assert middle_value.y.value == 3  # Generated

    outer_value = IdlValueGenerator(
        OuterType1, keys_only=True).create_test_data(1)
    assert outer_value.a.x == 1  # Generated
    assert outer_value.a.y.key == 2  # Generated
    assert outer_value.a.y.value == 3  # Generated
    assert outer_value.b == 111


def test_key_generation_with_string():
    value = IdlValueGenerator(StringKeyType, keys_only=True).create_test_data()
    assert value.key == "A"  # generated
    assert value.not_key == "not_key"


def test_key_generation_with_seq():
    value = IdlValueGenerator(SeqKeyType, keys_only=True).create_test_data(1)
    assert value.key == [1]  # generated
    assert list(value.not_key) == []


def test_key_value(type_fixture, shared_participant):
    pubsub = type_fixture.create_pubsub_fixture(shared_participant)
    for seed in seeds:
        sample = type_fixture.create_test_data(seed=seed)
        instance = pubsub.writer.register_instance(sample)
        assert instance != dds.InstanceHandle.nil()
        result = pubsub.writer.key_value(instance)
        key_holder = IdlTypeFixture(
            type_fixture.sample_type, keys_only=True).create_test_data(seed=seed)
        assert test_utils.keys_equal(result, key_holder)
        assert instance == pubsub.writer.lookup_instance(result)


def test_idl_types_generate_same_keyhashes_as_dynamic_data(type_fixture: IdlTypeFixture, shared_participant: dds.DomainParticipant):
    ts = idl.get_type_support(type_fixture.sample_type)

    idl_pubsub = type_fixture.create_pubsub_fixture(shared_participant)
    dd_participant = create_participant()
    dd_topic = dds.DynamicData.Topic(dd_participant, idl_pubsub.topic.name,
                                     type_fixture.dynamic_type)
    writer_qos = dd_participant.implicit_publisher.default_datawriter_qos
    writer_qos.property['dds.data_writer.history.memory_manager.fast_pool.pool_buffer_max_size'] = "1000"
    dd_writer = dds.DynamicData.DataWriter(
        dd_participant, dd_topic, writer_qos)

    for seed in seeds:
        key_holder = type_fixture.create_test_data(seed=seed)
        idl_instance = idl_pubsub.writer.register_instance(key_holder)
        assert idl_instance != dds.InstanceHandle.nil()

        dd_key_holder = ts.to_dynamic_data(key_holder)
        dd_instance = dd_writer.register_instance(dd_key_holder)

        assert dd_instance == idl_instance


def test_serialized_sample_to_key_hash(type_fixture: IdlTypeFixture, shared_participant: dds.DomainParticipant):
    prop = dds.DataWriterProtocol()
    prop.disable_inline_keyhash = True
    pubsub = PubSubFixture(shared_participant, type_fixture.sample_type,
                           writer_policies=[prop])
    for seed in seeds:
        sample = type_fixture.create_test_data(seed)
        pubsub.writer.write(sample)
        wait.for_data(pubsub.reader, 1)
        samples = pubsub.reader.take()
        assert len(samples) == 1
        assert samples[0][0] == sample

        instance_handle = pubsub.writer.lookup_instance(samples[0][0])
        assert test_utils.keys_equal(
            pubsub.writer.key_value(instance_handle), sample)


def test_deserialize_key_sample(type_fixture: IdlTypeFixture, shared_participant: dds.DomainParticipant):
    w_prop = dds.DataWriterProtocol()
    w_prop.serialize_key_with_dispose = True
    pubsub = PubSubFixture(shared_participant, type_fixture.sample_type,
                           writer_policies=[w_prop])

    for seed in seeds:
        sample = type_fixture.create_test_data(seed)
        pubsub.writer.write(sample)
        wait.for_data(pubsub.reader, 1)
        pubsub.reader.take()
        disposed_instance = pubsub.writer.lookup_instance(sample)
        pubsub.writer.dispose_instance(disposed_instance)
        wait.until_equal(1, lambda: len(pubsub.reader.read()))
        samples = pubsub.reader.take()
        for s in samples:
            assert not s[1].valid
        key_holder = pubsub.writer.key_value(disposed_instance)
        assert test_utils.keys_equal(sample, key_holder)
