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

import typing
from dataclasses import field
from array import array

import pytest

import rti.connextdds as dds
import rti.idl as idl

from common_types import Point
from test_utils.fixtures import *
import test_utils.wait


@idl.struct(member_annotations={'prices': [idl.bound(4)]})
class SequenceTest:
    vertices: typing.Sequence[Point] = field(default_factory=list)
    weights: typing.Sequence[int] = field(default_factory=list)
    prices: typing.Sequence[float] = field(default_factory=list)
    ready: typing.Sequence[bool] = field(default_factory=list)

    # These are IDL sequences, but use array instead of list in py for better
    # performance. Our mapping supports both for primitive types.
    weights_array: typing.Sequence[idl.int32] = field(
        default_factory=idl.array_factory(idl.int32))
    prices_array: typing.Sequence[float] = field(
        default_factory=idl.array_factory(float))
    ready_array: typing.Sequence[bool] = field(
        default_factory=idl.array_factory(bool))

@pytest.fixture
def sequence_sample():
    return SequenceTest(
        vertices=[Point(10, 20), Point(30, 40)],
        weights=[1, 2, 3],
        prices=[1.5, 2.5, 3.5, 4.5],
        ready=[True, False, True, True],
        weights_array=array('i', [111, 222]),
        prices_array=array('d', [11.5, 22.5, 33.5]),
        ready_array=array('b', [False, True]))


def test_sequence_plugin():
    ts = idl.get_type_support(SequenceTest)
    assert ts.type is SequenceTest

    dt = ts.dynamic_type
    assert dt.name == "SequenceTest"
    assert len(dt.members()) == 7

    point_ts = idl.get_type_support(Point)
    assert dt["vertices"].type == dds.SequenceType(
        point_ts.dynamic_type, 100)  # TODO: unbounded
    assert dt["weights"].type == dds.SequenceType(
        dds.Int64Type(), 100)  # TODO: unbounded
    assert dt["prices"].type == dds.SequenceType(
        dds.Float64Type(), 4)
    assert dt["ready"].type == dds.SequenceType(
        dds.BoolType(), 100)  # TODO: unbounded
    assert dt["weights_array"].type == dds.SequenceType(
        dds.Int32Type(), 100)  # TODO: unbounded
    assert dt["prices_array"].type == dds.SequenceType(
        dds.Float64Type(), 100)  # TODO: unbounded
    assert dt["ready_array"].type == dds.SequenceType(
        dds.BoolType(), 100)  # TODO: unbounded


def test_sequence_serialization(sequence_sample):
    ts = idl.get_type_support(SequenceTest)
    buffer = ts.serialize(sequence_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert sequence_sample == deserialized_sample


def test_sequence_pubsub(shared_participant, sequence_sample):
    fixture = PubSubFixture(shared_participant, SequenceTest)
    fixture.writer.write(sequence_sample)
    wait.for_data(fixture.reader)
    fixture.writer.write(SequenceTest())
    wait.for_data(fixture.reader)
    assert fixture.reader.take_data() == [sequence_sample, SequenceTest()]


def test_sequence_serialization_fails_when_out_of_bounds():
    ts = idl.get_type_support(SequenceTest)
    sample = SequenceTest(prices=[1.5, 2.5, 3.5, 4.5, 7.7])

    with pytest.raises(dds.Exception) as ex:
        ts.serialize(sample)

    assert "Error processing field 'prices'" in str(ex.value)

