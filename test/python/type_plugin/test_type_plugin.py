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

import ctypes
from collections import namedtuple
from dataclasses import dataclass
import gc, sys

import pytest

import rti.connextdds as dds
import rti.idl as idl

from test_utils.helpers import type_support_with_duck_typing
from test_utils.fixtures import *

@idl.struct
class Point:
    x: int = 0
    y: int = 0

def test_type_plugin_basic():
    assert hasattr(Point, "type_support")
    type_support = idl.get_type_support(Point)
    assert type_support is not None

    assert type_support.type is Point
    assert type_support._plugin_dynamic_type is not None
    assert type_support.is_valid_topic_type
    assert isinstance(type_support.c_type(), ctypes.Structure)

    dynamic_type = type_support.dynamic_type
    assert dynamic_type is not None
    assert dynamic_type.name == "Point"
    assert len(dynamic_type.members()) == 2
    assert dynamic_type["x"].name == "x"
    assert dynamic_type["x"].type == dds.Int64Type()
    assert dynamic_type["x"].id == 0
    assert dynamic_type["y"].id == 1

def test_idl_topic(shared_participant):
    topic = dds.Topic(shared_participant, "MyPoint", Point)

    assert topic.name == "MyPoint"
    assert topic.type_name == "Point"
    assert topic.type_support is idl.get_type_support(Point)
    assert topic.type is Point


def test_idl_topic_with_qos(shared_participant):
    qos = dds.TopicQos()
    qos.history.kind = dds.HistoryKind.KEEP_LAST
    qos.history.depth = 13
    topic = dds.Topic(shared_participant, "MyPoint", Point, qos)

    assert topic.name == "MyPoint"
    assert topic.type_name == "Point"
    assert topic.type_support is idl.get_type_support(Point)
    assert topic.type is Point

    assert topic.qos.history.kind == dds.HistoryKind.KEEP_LAST
    assert topic.qos.history.depth == 13


def test_idl_topic_with_type_name(participant: dds.DomainParticipant):
    assert not participant.is_type_registered("MyTypeName")
    assert not participant.is_type_registered("Point")
    topic = dds.Topic(participant, "MyPoint", Point, type_name="MyTypeName")
    assert participant.is_type_registered("MyTypeName")
    assert not participant.is_type_registered("Point")
    assert topic.type_name == "MyTypeName"


def test_idl_topic_with_all_args(shared_participant: dds.DomainParticipant):
    qos = dds.TopicQos()
    qos.history.kind = dds.HistoryKind.KEEP_LAST
    qos.history.depth = 13
    listener = dds.NoOpTopicListener()
    assert not shared_participant.is_type_registered("MyTypeName2")
    topic = dds.Topic(shared_participant, "MyPoint",
                      Point, 
                      type_name="MyTypeName2",
                      qos=qos,
                      listener=listener,
                      mask=dds.StatusMask.INCONSISTENT_TOPIC)
    assert topic.type_name == "MyTypeName2"
    assert topic.qos.history.depth == 13
    assert topic.listener is listener

def test_idl_any_topic(shared_participant):
    topic = dds.Topic(shared_participant, "MyPoint", Point)

    as_any_topic = dds.AnyTopic(topic)
    assert as_any_topic.name == "MyPoint"
    assert dds.Topic(as_any_topic).type_support is idl.get_type_support(Point)


def test_serialization():
    ts = idl.get_type_support(Point)
    sample = Point(x=33, y=24)
    assert sample == ts.deserialize(ts.serialize(sample))

def test_pubsub(shared_participant):
    fixture = PubSubFixture(shared_participant, Point)
    fixture.send_and_check(Point(3, 4))
    fixture.send_and_check(Point(5, 7))

@idl.struct
class NotAPoint:
    a: int = 0

def test_serialization_fails_with_bad_sample_type():
    ts = idl.get_type_support(Point)

    ExceptionType = TypeError if not ts.allow_duck_typing else idl.FieldSerializationError

    with pytest.raises(ExceptionType):
        ts.serialize(NotAPoint(2))

    with pytest.raises(ExceptionType):
        ts.serialize(3.14)

    with pytest.raises(ExceptionType):
        ts.serialize(None)


def test_deserialization_fails_with_bad_buffer_type():
    ts = idl.get_type_support(Point)

    with pytest.raises(TypeError):
        ts.deserialize([1, 2, 3])

    bad_element_buffer = dds.Int64Seq([1, 2, 3])
    with pytest.raises(TypeError):
        ts.deserialize(bad_element_buffer)

    with pytest.raises(TypeError):
        ts.deserialize(None)

def test_idl_writer_fails_with_bad_sample_type():
    participant = create_participant()
    topic = dds.Topic(participant, "MyPoint", Point)
    writer = dds.DataWriter(participant.implicit_publisher, topic)

    ts = idl.get_type_support(Point)
    with pytest.raises(idl.FieldSerializationError):
        writer.write(NotAPoint())

    with pytest.raises(idl.FieldSerializationError):
        writer.write(4)

    with pytest.raises(idl.FieldSerializationError):
        writer.write(None)

def test_nested_type():
    @idl.struct
    class Rectangle:
        a: Point = Point()
        b: Point = Point()

    ts = idl.get_type_support(Rectangle)
    assert ts.type is Rectangle

    point_ts = idl.get_type_support(Point)
    assert type(ts.c_type().a) is point_ts.c_type
    assert ts.dynamic_type["a"].type == point_ts.dynamic_type

    orig_sample = Rectangle(a=Point(x=1, y=2), b=Point(x=3, y=4))
    assert orig_sample == ts.deserialize(ts.serialize(orig_sample))


def test_serialization_with_duck_typing():

    # We'll publish a sample of a different type that has at least the same
    # fields as Point
    Point3D = namedtuple("Point3D", ["x", "y", "z"])
    sample = Point3D(x=33, y=24, z=5)

    with type_support_with_duck_typing(Point) as ts:
        assert Point(33, 24) == ts.deserialize(ts.serialize(sample))


def test_pubsub_with_duck_typing(shared_participant):
    fixture = PubSubFixture(shared_participant, Point)

    class Point3D:
        def __init__(self):
            self.z = 5
            self.y = 4
            self.x = 3

    sample = Point3D()

    with type_support_with_duck_typing(Point):
        fixture.writer.write(sample)
    wait.for_data(fixture.reader)
    assert fixture.reader.take_data() == [Point(3, 4)]


def test_cft(shared_participant):
    pubsub = PubSubFixture(shared_participant, Point, content_filter="x >= 0")
    pubsub.writer.write(Point(-1, 1))
    pubsub.send_and_check(Point(1, 1))  # only this sample is received


def test_topic_retains_type_support(shared_participant):

    @idl.struct
    class Foo:
        a: int = 0

    ts = idl.get_type_support(Foo)
    initial_ref_count = sys.getrefcount(ts)

    # A topic object increases the refcount of its type support
    topic1 = dds.Topic(shared_participant, "MyFooTest", Foo)
    assert topic1.name == "MyFooTest" # suppress unused warning
    assert sys.getrefcount(ts) == initial_ref_count + 1

    topic2 = dds.Topic(shared_participant, "MyFooTest2", Foo)
    assert topic2.name == "MyFooTest2"  # suppress unused warning
    assert sys.getrefcount(ts) == initial_ref_count + 2

    # The refcount is decremented when the topic is deleted
    topic1 = None
    assert sys.getrefcount(ts) == initial_ref_count + 1
    topic2 = None
    assert sys.getrefcount(ts) == initial_ref_count


def test_use_type_support_after_type(participant):
    def create_topic():
        @idl.struct
        class FooIn:
            x: int = 0

        ts = idl.get_type_support(FooIn)
        ts.allow_duck_typing = True # To allow writing FooOut

        return dds.Topic(participant, "MyFooTest", FooIn)

    writer = dds.DataWriter(dds.Publisher(participant), create_topic())

    # force garbage collection to make sure the type support is not kept alive
    gc.collect()

    @dataclass
    class FooOut:
        x: int = 0

    # This verifies that the type support outlived FooIn and can be used as
    # long as the topic exists
    writer.write(FooOut(4))


# Run test with selector and without selector
@pytest.mark.parametrize("use_selector", [False, True])
def test_native_read(shared_participant, use_selector):
    point_ts = idl.get_type_support(Point)
    fixture = PubSubFixture(shared_participant, Point)
    fixture.writer.write([Point(3, 4), Point(4, 5)])
    wait.for_data(fixture.reader, 2)

    reader = fixture.reader if not use_selector else fixture.reader.select()

    with reader.read_loaned() as samples:
        assert len(samples) == 2
        assert samples[0].info.valid
        assert samples[0].info.state.sample_state == dds.SampleState.NOT_READ
        sample_ptr = samples[0].data._get_ptr()
        sample = ctypes.cast(sample_ptr, point_ts.c_type_ptr).contents
        assert sample.x == 3
        assert sample.y == 4

    with reader.take_loaned() as samples:
        assert len(samples) == 2
        assert samples[0].info.valid
        assert samples[0].info.state.sample_state == dds.SampleState.READ
        sample = point_ts.get_c_data(samples[0].data)
        assert sample.x == 3
        assert sample.y == 4

    with reader.read_loaned() as samples:
        assert len(samples) == 0
