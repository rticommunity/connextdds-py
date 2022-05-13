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

from typing import Sequence, Optional
from dataclasses import field
from array import array

import pytest

import rti.connextdds as dds
import rti.idl as idl

from common_types import Point
from test_utils.fixtures import *
from test_utils import log_capture


@idl.struct(
    member_annotations={
        'prices': [idl.bound(4)],
        'prices_array': [idl.bound(4)],
        'vertices_bounded': [idl.bound(2)]
    }
)
class SequenceTest:
    vertices: Sequence[Point] = field(default_factory=list)
    weights: Sequence[int] = field(default_factory=list)
    prices: Sequence[float] = field(default_factory=list)
    ready: Sequence[bool] = field(default_factory=list)

    # These are IDL sequences, but use array instead of list in py for better
    # performance. Our mapping supports both for primitive types.
    weights_array: Sequence[idl.int16] = field(
        default_factory=idl.array_factory(idl.int16))
    prices_array: Sequence[float] = field(
        default_factory=idl.array_factory(float))
    ready_array: Sequence[bool] = field(
        default_factory=idl.array_factory(bool))

    vertices_bounded: Sequence[Point] = field(default_factory=list)


@idl.struct(
    member_annotations={
        'prices': [idl.bound(4)],
        'prices_array': [idl.bound(4)],
        'vertices_bounded': [idl.bound(2)]
    }
)
class OptionalSequenceTest:
    vertices: Optional[Sequence[Point]] = None
    weights: Optional[Sequence[int]] = None
    prices: Optional[Sequence[float]] = None
    ready: Optional[Sequence[bool]] = None
    weights_array: Optional[Sequence[idl.int16]] = None
    prices_array: Optional[Sequence[float]] = None
    ready_array: Optional[Sequence[bool]] = None
    vertices_bounded: Optional[Sequence[Point]] = None


def create_sequence_sample():
    return SequenceTest(
        vertices=[Point(10, 20), Point(30, 40)],
        weights=[1, 2, 3],
        prices=[1.5, 2.5, 3.5, 4.5],
        ready=[True, False, True, True],
        weights_array=array('h', [111, 222]),
        prices_array=array('d', [11.5, 22.5, 33.5]),
        ready_array=array('b', [False, True]),
        vertices_bounded=[Point(100, 200)])


def create_optional_sequence_sample():
    return OptionalSequenceTest(
        vertices=[Point(10, 20), Point(30, 40)],
        weights=[1, 2, 3],
        prices=[1.5, 2.5, 3.5, 4.5],
        ready=[True, False, True, True],
        weights_array=[111, 222],
        prices_array=[11.5, 22.5, 33.5],
        ready_array=[False, True],
        vertices_bounded=[Point(100, 200)])


@pytest.fixture
def sequence_sample():
    return create_sequence_sample()


@pytest.fixture
def optional_sequence_sample():
    return create_optional_sequence_sample()


def test_sequence_plugin():
    ts = idl.get_type_support(SequenceTest)
    assert ts.type is SequenceTest

    dt = ts.dynamic_type
    assert dt.name == "SequenceTest"
    assert len(dt.members()) == 8

    point_ts = idl.get_type_support(Point)
    assert dt["vertices"].type == dds.SequenceType(point_ts.dynamic_type)
    assert dt["weights"].type == dds.SequenceType(dds.Int64Type())
    assert dt["prices"].type == dds.SequenceType(dds.Float64Type(), 4)
    assert dt["ready"].type == dds.SequenceType(dds.BoolType())
    assert dt["weights_array"].type == dds.SequenceType(dds.Int16Type())
    assert dt["prices_array"].type == dds.SequenceType(dds.Float64Type(), 4)
    assert dt["ready_array"].type == dds.SequenceType(dds.BoolType())
    assert dt["vertices_bounded"].type == dds.SequenceType(point_ts.dynamic_type, 2)


def test_sequence_serialization(sequence_sample):
    ts = idl.get_type_support(SequenceTest)
    buffer = ts.serialize(sequence_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert sequence_sample == deserialized_sample


@pytest.mark.parametrize("SequenceTestType", [SequenceTest, OptionalSequenceTest])
def test_empty_sequence_serialization(SequenceTestType):
    ts = idl.get_type_support(SequenceTestType)
    buffer = ts.serialize(SequenceTestType())
    deserialized_sample = ts.deserialize(buffer)
    assert SequenceTestType() == deserialized_sample


def test_sequence_pubsub(shared_participant, sequence_sample):
    fixture = PubSubFixture(shared_participant, SequenceTest, reader_policies=[
                            dds.ResourceLimits(1, 1, 1)])
    fixture.send_and_check(SequenceTest())
    fixture.send_and_check(sequence_sample)
    fixture.send_and_check(SequenceTest())
    sequence_sample.vertices_bounded += [Point(7, 8)]
    sequence_sample.vertices += [Point(7, 8)]
    fixture.send_and_check(sequence_sample)


def test_optional_sequence_serialization(optional_sequence_sample):
    ts = idl.get_type_support(OptionalSequenceTest)
    buffer = ts.serialize(optional_sequence_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert optional_sequence_sample == deserialized_sample


def test_optional_sequence_pubsub(shared_participant, optional_sequence_sample):
    fixture = PubSubFixture(shared_participant, OptionalSequenceTest)
    fixture.send_and_check(optional_sequence_sample)
    fixture.send_and_check(OptionalSequenceTest())
    optional_sequence_sample.prices_array += [7.23]
    optional_sequence_sample.vertices_bounded += [Point(7, 8)]
    fixture.send_and_check(optional_sequence_sample)

@pytest.mark.parametrize("SequenceTestType", [SequenceTest, OptionalSequenceTest])
def test_sequence_serialization_fails_when_out_of_bounds(SequenceTestType):
    ts = idl.get_type_support(SequenceTestType)

    sample = SequenceTestType(prices=[1.5, 2.5, 3.5, 4.5])
    ts.serialize(sample)
    sample.prices += [5.5]

    # fail_if_no_logs is set to False because due to impl. details the bounds
    # check for optionals is done in C, but for non-optionals it is done in
    # Python, and therefore in both cases there's an exception, but only in the
    # C layer an additional stdout log message is printed.
    with log_capture.expected_exception(idl.FieldSerializationError, fail_if_no_logs=False) as errinfo:
        ts.serialize(sample)
    assert "Error processing field 'prices'" in str(errinfo.exception_msg)

    sample = SequenceTestType(vertices_bounded=[Point(1, 2), Point(3, 4)])
    ts.serialize(sample)
    sample.vertices_bounded += [Point(5, 6)]
    with log_capture.expected_exception(idl.FieldSerializationError, fail_if_no_logs=False) as errinfo:
        ts.serialize(sample)
    assert "Error processing field 'vertices_bounded'" in str(
        errinfo.exception_msg)


@pytest.mark.parametrize("SequenceTestType", [SequenceTest, OptionalSequenceTest])
def test_sequence_serialization_fails_with_incorrect_element_type(SequenceTestType):
    ts = idl.get_type_support(SequenceTestType)

    sample = SequenceTestType(prices=[1.5, "hello"])
    with pytest.raises(idl.FieldSerializationError) as ex:
        ts.serialize(sample)
    assert "Error processing field 'prices'" in str(ex.value)

    sample = SequenceTestType(vertices=[Point(1, 2), 3])
    with pytest.raises(idl.FieldSerializationError) as ex:
        ts.serialize(sample)
    assert "Error processing field 'vertices'" in str(ex.value)


@pytest.mark.skip("Enable once a buffer-like field can be serialized as a list")
def test_sequence_serialization_with_int_element_out_of_range_is_safe():
    ts = idl.get_type_support(SequenceTest)

    # The current implementation doesn't enforce int sizes, but elements within
    # the correct range will be correctly serialized an deserialized
    sample = SequenceTest(weights_array=array([2**15 - 1, 2**15, 4]))
    deserialized_sample = ts.deserialize(ts.serialize(sample))

    assert deserialized_sample.weights_array[0] == sample.weights_array[0]
    assert deserialized_sample.weights_array[1] != sample.weights_array[1]
    assert deserialized_sample.weights_array[2] == sample.weights_array[2]


def test_sequence_iterator_serialization():
    ts = idl.get_type_support(SequenceTest)

    # The serialization routines support any sequence of elements, including
    # a lazy iterator such as range()
    sample = SequenceTest(weights=range(2, 5))
    buffer = ts.serialize(sample)
    deserialized_sample = ts.deserialize(buffer)
    assert deserialized_sample.weights == [2, 3, 4]


@pytest.mark.parametrize("SequenceTestType", [SequenceTest, OptionalSequenceTest])
def test_sequence_publication_fails_when_out_of_bounds(shared_participant, SequenceTestType):
    fixture = PubSubFixture(shared_participant, SequenceTestType)
    with log_capture.expected_exception(idl.FieldSerializationError, fail_if_no_logs=False) as errinfo:
        fixture.writer.write(SequenceTestType(prices=[1.1] * 5))
    assert "Error processing field 'prices'" in str(errinfo.exception_msg)


def test_sequence_of_sequence_not_supported():
    with pytest.raises(TypeError) as ex:
        @idl.struct
        class SeqOfSeq:
            s: Sequence[Sequence[Point]]

    assert "Sequences of sequences are not supported" in str(ex.value)
