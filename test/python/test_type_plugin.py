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
import pytest

import rti.connextdds as dds
import rti.idl as idl

from test_utils import fixtures

@idl.struct
class Point:
    x: int = 0
    y: int = 0

def test_type_plugin_basic():
    assert hasattr(Point, "type_support")
    assert Point.type_support is not None

    assert Point.type_support.type is Point
    assert isinstance(Point.type_support.c_type(), ctypes.Structure)

    dynamic_type = Point.type_support.dynamic_type
    assert dynamic_type is not None
    assert dynamic_type.name == "Point"
    assert len(dynamic_type.members()) == 2
    assert dynamic_type["x"].name == "x"
    assert dynamic_type["x"].type == dds.Int32Type()

def test_idl_topic():
    participant = fixtures.create_participant(domain_id=54)
    topic = dds.Topic(participant, "MyPoint", Point)

    assert topic.name == "MyPoint"
    assert topic.type_name == "Point"
    assert topic.type_support is Point.type_support
    assert topic.type is Point


def test_idl_topic_with_qos():
    participant = fixtures.create_participant(domain_id=54)
    qos = dds.TopicQos()
    qos.history.kind = dds.HistoryKind.KEEP_LAST
    qos.history.depth = 13
    topic = dds.Topic(participant, "MyPoint", Point, qos)

    assert topic.name == "MyPoint"
    assert topic.type_name == "Point"
    assert topic.type_support is Point.type_support
    assert topic.type is Point

    assert topic.qos.history.kind == dds.HistoryKind.KEEP_LAST
    assert topic.qos.history.depth == 13


def test_idl_any_topic():
    participant = fixtures.create_participant(domain_id=54)
    topic = dds.Topic(participant, "MyPoint", Point)

    as_any_topic = dds.AnyTopic(topic)
    assert as_any_topic.name == "MyPoint"
    assert dds.Topic(as_any_topic).type_support is Point.type_support


def test_serialization():
    ts = Point.type_support
    sample = Point(x=33, y=24)
    assert sample == ts.deserialize(ts.serialize(sample))

@idl.struct
class NotAPoint:
    a: int = 0

def test_serialization_fails_with_bad_sample_type():
    with pytest.raises(TypeError):
        Point.type_support.serialize(NotAPoint(2))

    with pytest.raises(TypeError):
        Point.type_support.serialize(3.14)

    with pytest.raises(TypeError):
        Point.type_support.serialize(None)


def test_deserialization_fails_with_bad_buffer_type():
    ts = Point.type_support

    with pytest.raises(TypeError):
        ts.deserialize([1, 2, 3])

    bad_element_buffer = dds.Int64Seq([1, 2, 3])
    with pytest.raises(TypeError):
        ts.deserialize(bad_element_buffer)

    with pytest.raises(TypeError):
        ts.deserialize(None)

def test_idl_writer_fails_with_bad_sample_type():
    participant = fixtures.create_participant(domain_id=0)
    topic = dds.Topic(participant, "MyPoint", Point)
    writer = dds.DataWriter(participant.implicit_publisher, topic)

    with pytest.raises(TypeError):
        writer.write(NotAPoint())

    with pytest.raises(TypeError):
        writer.write(4)

    with pytest.raises(TypeError):
        writer.write(None)

def test_nested_type():
    @idl.struct
    class Rectangle:
        a: Point = Point()
        b: Point = Point()

    ts = Rectangle.type_support
    assert ts.type is Rectangle
    assert type(ts.c_type().a) is Point.type_support.c_type
    assert ts.dynamic_type["a"].type == Point.type_support.dynamic_type

    orig_sample = Rectangle(a=Point(x=1, y=2), b=Point(x=3, y=4))
    c_sample = ts._create_c_sample(orig_sample)
    assert (c_sample.a.x, c_sample.a.y) == (1, 2)
    assert (c_sample.b.x, c_sample.b.y) == (3, 4)

    py_sample = ts._create_py_sample_no_ptr(c_sample)
    assert orig_sample == py_sample

