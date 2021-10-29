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

def test_idl_writer_fails_with_bad_sample_type():
    @idl.struct
    class NotAPoint:
        a: int = 0

    participant = fixtures.create_participant(domain_id=0)
    topic = dds.Topic(participant, "MyPoint", Point)
    writer = dds.DataWriter(participant.implicit_publisher, topic)

    with pytest.raises(TypeError):
        writer.write(NotAPoint())

    with pytest.raises(TypeError):
        writer.write(4)
