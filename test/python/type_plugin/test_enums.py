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

from dataclasses import field
import ctypes
from typing import Sequence, Optional
from enum import IntEnum, auto
from test_utils import log_capture

import pytest

import rti.connextdds as dds
import rti.idl as idl

from test_utils.fixtures import *


@idl.enum
class Color(IntEnum):
    RED = 1
    GREEN = auto()
    BLUE = auto()


@idl.enum(type_annotations=[idl.mutable])
class Shape(IntEnum):
    CIRCLE = 3
    SQUARE = 5


@idl.struct(
    member_annotations={
        "shape_sequence": [idl.bound(3)],
        "color_array": [idl.array(2)]
    }
)
class EnumTest:
    color: Color = Color.RED
    shape: Shape = Shape.CIRCLE
    shape_sequence: Sequence[Shape] = field(default_factory=list)
    color_array: Sequence[Color] = field(default_factory=idl.list_factory(Color.RED, 2))
    opt_enum: Optional[Shape] = None


def create_enum_sample():
    return EnumTest(
        Color.GREEN,
        Shape.SQUARE,
        [Shape.CIRCLE, Shape.SQUARE, Shape.CIRCLE],
        [Color.RED, Color.BLUE],
        Shape.SQUARE)


@pytest.fixture
def enum_sample():
    return create_enum_sample()


def test_enum_plugin():
    color_ts = idl.get_type_support(Color)
    assert color_ts.type is Color
    assert color_ts.c_type is ctypes.c_int32
    assert not color_ts.is_valid_topic_type
    color_dt: dds.EnumType = color_ts.dynamic_type
    assert color_dt.extensibility_kind == dds.ExtensibilityKind.EXTENSIBLE
    assert len(color_dt.members()) == 3
    assert color_dt["RED"].ordinal == 1
    assert color_dt["GREEN"].ordinal == 2
    assert color_dt["BLUE"].ordinal == 3

    shape_ts = idl.get_type_support(Shape)
    assert shape_ts.type is Shape
    assert shape_ts.c_type is ctypes.c_int32
    shape_dt: dds.EnumType = shape_ts.dynamic_type
    assert shape_dt.extensibility_kind == dds.ExtensibilityKind.MUTABLE
    assert len(shape_dt.members()) == 2
    assert shape_dt["CIRCLE"].ordinal == 3
    assert shape_dt["SQUARE"].ordinal == 5

    ts = idl.get_type_support(EnumTest)
    assert ts.type is EnumTest
    dt = ts.dynamic_type
    assert dt.name == "EnumTest"
    assert len(dt.members()) == 5
    assert dt["color"].type == color_ts.dynamic_type
    assert dt["shape"].type == shape_ts.dynamic_type
    assert dt["shape_sequence"].type == dds.SequenceType(shape_ts.dynamic_type, 3)
    assert dt["color_array"].type == dds.ArrayType(color_ts.dynamic_type, 2)
    assert dt["opt_enum"].optional


def test_enum_default_serialization():
    ts = idl.get_type_support(EnumTest)
    buffer = ts.serialize(EnumTest())
    deserialized_sample = ts.deserialize(buffer)
    assert EnumTest() == deserialized_sample

def test_enum_serialization(enum_sample):
    ts = idl.get_type_support(EnumTest)
    buffer = ts.serialize(enum_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert enum_sample == deserialized_sample


def test_enum_pubsub(shared_participant, enum_sample):
    fixture = PubSubFixture(shared_participant, EnumTest)
    fixture.send_and_check([enum_sample, EnumTest()])


def test_enum_serialization_with_unknown_enumerator_fails(enum_sample):
    ts = idl.get_type_support(EnumTest)
    enum_sample.color = 777

    with log_capture.expected_exception(dds.Error) as errinfo:
        ts.serialize(enum_sample)
    # This error causes the Core to print a log message
    assert "Invalid enumerator value 777" in errinfo.logs
    # And also raises an exception
    assert "Error serializing data" in errinfo.exception_msg


def test_enum_deserialization_with_unknown_enumerator_succeeds():
    # Deserializing an unknown enumerator is allowed, since it's an option that
    # can be set in the type compatibility Qos

    ts = idl.get_type_support(EnumTest)
    enum_sample = EnumTest(shape = Shape.SQUARE)
    buffer: Sequence[int] = ts.serialize(enum_sample)

    assert buffer.count(Shape.SQUARE.value) == 1
    # Replace the legal enumerator with an unknown one
    found = False
    for i in range(len(buffer)):
        if buffer[i] == Shape.SQUARE.value:
            buffer[i] = 2
            found = True
            break
    assert found

    # The deserialization will assign the default (CIRCLE)
    deserialized_sample = ts.deserialize(buffer)
    assert deserialized_sample.shape == Shape.CIRCLE


def test_enum_decorator_applied_to_nonenum_fails():
    with pytest.raises(TypeError) as ex:
        @idl.enum
        class NonEnum:
            pass

    assert "is not an IntEnum" in str(ex.value)

def test_creating_enum_topic_fails(shared_participant):
    with pytest.raises(TypeError) as ex:
        dds.Topic(shared_participant, "Color", Color)

    assert "Incompatible 'type' argument: an @idl.struct or @idl.union is required" in str(
        ex.value)
