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
import copy

import pytest

import rti.connextdds as dds
import rti.idl as idl

from common_types import Point
from test_utils.fixtures import *

from test_sequences import SequenceTest, create_sequence_sample


@idl.alias(annotations=[idl.array(3)])
class MyPointArray:
    value: Sequence[Point] = field(default_factory=idl.list_factory(Point, 3))


@idl.alias(annotations=[idl.array(3)])
class MyIntArray:
    value: Sequence[int] = field(default_factory=idl.array_factory(int, 3))


@idl.struct(
    member_annotations={
        'vertices': [idl.array(3)],
        'weights': [idl.array(4)],
        'prices': [idl.array(5)],
        'custom_factory': [idl.array(3)],
        'strings': [idl.array(3), idl.element_annotations([idl.bound(5)])],
        'wstrings': [idl.array(3), idl.element_annotations([idl.utf16, idl.bound(5)])],
        'complex': [idl.array(3)],
        'multi_str': [idl.array([2, 3])],
        'multi_int': [idl.array([3, 2])],
        'int_array_array': [idl.array(2)],
        'point_array_array': [idl.array(2)],
    }
)
class ArrayTest:
    vertices: Sequence[Point] = field(default_factory=idl.list_factory(Point, 3))
    weights: Sequence[idl.int16] = field(default_factory=idl.array_factory(idl.int16, 4))
    prices: Sequence[float] = field(default_factory=idl.list_factory(float, 5))
    custom_factory: Sequence[int] = field(default_factory=lambda: [1, 2, 3])
    strings: Sequence[str] = field(default_factory=idl.list_factory(str, 3))
    wstrings: Sequence[str] = field(default_factory=idl.list_factory(str, 3))
    complex: Sequence[SequenceTest] = field(default_factory=idl.list_factory(SequenceTest, 3))
    multi_str: Sequence[str] = field(default_factory=idl.list_factory(str, [2, 3]))
    multi_int: Sequence[int] = field(
        default_factory=idl.array_factory(int, [3, 2]))
    int_array_array: Sequence[MyIntArray] = field(default_factory=idl.list_factory(MyIntArray, 2))
    point_array_array: Sequence[MyPointArray] = field(default_factory=idl.list_factory(MyPointArray, 2))
    opt_int_seq_array: Optional[MyIntArray] = None
    opt_point_seq_array: Optional[MyPointArray] = None


def create_array_sample():
    return ArrayTest(
        vertices=[Point(10, 20), Point(30, 40), Point(50, 60)],
        weights=array('h', [1, 2, 3, 4]),
        prices=[1.5, 2.5, 3.5, 4.5, 5.5],
        strings=["hi", "world", "!"],
        wstrings=["hi", "world", "!"],
        complex=[create_sequence_sample()] * 2 + [SequenceTest(vertices_bounded=[Point(1, 1)])],
        multi_str=["hello", "world", "!!"] * 2,
        multi_int=array('q', [1, 2, 3] * 2),
        int_array_array=[
            MyIntArray(value=array('q', [1, 2, 3])),
            MyIntArray(value=array('q', [4, 5, 6]))],
        point_array_array=[
            MyPointArray(value=[Point(1, 1), Point(2, 2), Point(3, 3)]),
            MyPointArray(value=[Point(4, 4), Point(5, 5), Point(6, 6)])],
        opt_int_seq_array=MyIntArray(value=array('q', [1, 2, 3])),
        opt_point_seq_array=MyPointArray([Point(31, 14), Point(15, 16), Point(17, 18)]))


@pytest.fixture
def array_sample():
    return create_array_sample()


def test_array_plugin():
    ts = idl.get_type_support(ArrayTest)
    assert ts.type is ArrayTest

    dt = ts.dynamic_type
    assert dt.name == "ArrayTest"
    assert len(dt.members()) == 13

    point_ts = idl.get_type_support(Point)
    assert dt["vertices"].type == dds.ArrayType(point_ts.dynamic_type, 3)
    assert dt["weights"].type == dds.ArrayType(dds.Int16Type(), 4)
    assert dt["prices"].type == dds.ArrayType(dds.Float64Type(), 5)
    assert dt["custom_factory"].type == dds.ArrayType(dds.Int64Type(), 3)
    assert dt["strings"].type == dds.ArrayType(dds.StringType(5), 3)
    assert dt["wstrings"].type == dds.ArrayType(dds.WStringType(5), 3)
    element_ts = idl.get_type_support(SequenceTest)
    assert dt["complex"].type == dds.ArrayType(element_ts.dynamic_type, 3)
    assert dt["multi_str"].type == dds.ArrayType(dds.StringType(), 2 * 3)
    assert dt["multi_int"].type == dds.ArrayType(dds.Int64Type(), 3 * 2)
    assert dt["int_array_array"].type == dds.ArrayType(idl.get_type_support(MyIntArray).dynamic_type, 2)
    assert dt["point_array_array"].type == dds.ArrayType(idl.get_type_support(MyPointArray).dynamic_type, 2)


@pytest.mark.xfail(reason="TODO PY-17: multi_str-dimensional arrays are created flat for now")
def test_multi_dim_array_dynamic_type():
    dt = idl.get_type_support(ArrayTest).dynamic_type
    assert dt["multi_str"].type == dds.ArrayType(dds.StringType(), [2, 3])
    assert dt["multi_int"].type == dds.ArrayType(dds.Int32Type(), [3, 2])

def test_array_default_creation():
    sample = ArrayTest()
    assert sample.vertices == [Point(0, 0)] * 3
    assert sample.weights == array('h', [0] * 4)
    assert sample.prices == [0.0] * 5
    assert sample.custom_factory == [1, 2, 3]
    assert sample.strings == [""] * 3
    assert sample.wstrings == [""] * 3
    assert sample.complex == [SequenceTest()] * 3
    assert sample.multi_str == [""] * 6
    assert sample.multi_int == array('q', [0] * 6)
    assert sample.opt_int_seq_array is None
    assert sample.opt_point_seq_array is None

    # The elements are copies
    assert sample.vertices[0] is not sample.vertices[1]
    assert sample.complex[0] is not sample.complex[1]

    # Except for the strings
    assert sample.strings[0] is sample.strings[1]

def test_array_deep_copy(array_sample: ArrayTest):
    array_copy = copy.deepcopy(array_sample)
    assert array_sample == array_copy

    # Sequences are deep-copied
    assert array_sample.vertices[0] == array_copy.vertices[0]
    assert array_sample.vertices[0] is not array_copy.vertices[0]
    assert array_sample.complex[0] == array_copy.complex[0]
    assert array_sample.complex[0] is not array_copy.complex[0]
    assert array_sample.opt_point_seq_array.value[0] == array_copy.opt_point_seq_array.value[0]
    assert array_sample.opt_point_seq_array.value[0] is not array_copy.opt_point_seq_array.value[0]



def test_array_shallow_copy(array_sample: ArrayTest):
    array_shallow = copy.copy(array_sample)
    assert array_sample == array_shallow

    # Shallow copy
    assert array_sample.vertices is array_shallow.vertices
    assert array_sample.complex is array_shallow.complex

def test_array_serialization(array_sample: ArrayTest):
    ts = idl.get_type_support(ArrayTest)
    buffer = ts.serialize(array_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert array_sample == deserialized_sample


def test_array_pubsub(shared_participant, array_sample: ArrayTest):
    fixture = PubSubFixture(shared_participant, ArrayTest)
    fixture.send_and_check(array_sample)



def test_array_default_pubsub(shared_participant):
    fixture = PubSubFixture(shared_participant, ArrayTest)
    fixture.send_and_check(ArrayTest())


def test_array_serialization_fails_with_bad_length():
    ts = idl.get_type_support(ArrayTest)

    sample = create_array_sample()
    sample.prices.append(1.5)
    with pytest.raises(idl.FieldSerializationError) as ex:
        ts.serialize(sample)
    assert "Error processing field 'prices'" in str(ex.value)

    sample = create_array_sample()
    sample.vertices.append(Point(10, 20))
    with pytest.raises(idl.FieldSerializationError) as ex:
        ts.serialize(sample)
    assert "Error processing field 'vertices'" in str(ex.value)

    sample = create_array_sample()
    sample.weights.append(1)
    with pytest.raises(idl.FieldSerializationError) as ex:
        ts.serialize(sample)
    assert "Error processing field 'weights'" in str(ex.value)


def test_array_serialization_fails_with_incorrect_element_type():
    ts = idl.get_type_support(ArrayTest)

    sample = create_array_sample()
    sample.prices = [1.5] * 4 + ["hello"]
    with pytest.raises(idl.FieldSerializationError) as ex:
        ts.serialize(sample)
    assert "Error processing field 'prices'" in str(ex.value)

    sample = create_array_sample()
    sample.vertices = [Point(1, 2)] * 3 + [4]
    with pytest.raises(idl.FieldSerializationError) as ex:
        ts.serialize(sample)
    assert "Error processing field 'vertices'" in str(ex.value)


def test_array_publication_fails_when_out_of_bounds(shared_participant):
    fixture = PubSubFixture(shared_participant, ArrayTest)
    sample = create_array_sample()
    sample.prices = [1.5] * 4 + ["hello"]
    with pytest.raises(idl.FieldSerializationError) as ex:
        fixture.writer.write(sample)
    assert "Error processing field 'prices'" in str(ex.value)


def test_array_publication_fails_when_string_out_of_bounds(shared_participant):
    fixture = PubSubFixture(shared_participant, ArrayTest)
    sample = create_array_sample()
    sample.strings[1] = "too long"
    with pytest.raises(idl.FieldSerializationError) as ex:
        fixture.writer.write(sample)
    assert "Error processing field 'strings'" in str(ex.value)


def test_arrays_of_sequences_not_supported():
    with pytest.raises(TypeError) as ex:
        @idl.struct(
            member_annotations={
                's': [idl.array(3)]
            }
        )
        class ArraySeqTest:
            s: Sequence[Sequence[Point]]

    assert "Arrays of sequences are not supported" in str(ex.value)
