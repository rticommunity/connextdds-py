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

"""Tests the type plugin of a type with all combinations of sequences of
strings:
   (bounded sequence, unbounded sequence) x
   (bounded string, unbounded string) x
   (string, wstring)
"""

import typing
from dataclasses import field

import pytest

import rti.connextdds as dds
import rti.idl as idl

from test_utils.fixtures import *


@idl.struct(
    member_annotations={
        'b_b': [idl.bound(3), idl.element_annotations([idl.bound(6)])],
        'b_unb': [idl.bound(4)],
        'unb_b': [idl.element_annotations([idl.bound(10)])],
        'w_b_b': [idl.bound(3), idl.element_annotations([idl.bound(6), idl.utf16])],
        'w_b_unb': [idl.bound(4), idl.element_annotations([idl.utf16])],
        'w_unb_b': [idl.element_annotations([idl.bound(10), idl.utf16])],
        'w_unb_unb': [idl.element_annotations([idl.utf16])]
    }
)
class SequenceTest:
    b_b: typing.Sequence[str] = field(default_factory=list)
    b_unb: typing.Sequence[str] = field(default_factory=list)
    unb_b: typing.Sequence[str] = field(default_factory=list)
    unb_unb: typing.Sequence[str] = field(default_factory=list)

    w_b_b: typing.Sequence[str] = field(default_factory=list)
    w_b_unb: typing.Sequence[str] = field(default_factory=list)
    w_unb_b: typing.Sequence[str] = field(default_factory=list)
    w_unb_unb: typing.Sequence[str] = field(default_factory=list)

@pytest.fixture
def sequence_sample():
    return SequenceTest(
        b_b=["hello", ",", "world!"],
        b_unb=["a" * 10, "b" * 20],
        unb_b=["hello", ",", "world!"] * 3,
        unb_unb=["a" * 10, "b" * 20] * 10,
        w_b_b=["hello", ",", "world!"],
        w_b_unb=["a" * 10, "b" * 20],
        w_unb_b=["hello", ",", "world!"] * 3,
        w_unb_unb=["a" * 10, "b" * 20] * 10)


def test_sequence_plugin():
    ts = idl.get_type_support(SequenceTest)
    assert ts.type is SequenceTest

    dt = ts.dynamic_type
    assert dt.name == "SequenceTest"
    assert len(dt.members()) == 8

    assert dt["b_b"].type == dds.SequenceType(dds.StringType(6), 3)
    assert dt["b_unb"].type == dds.SequenceType(dds.StringType(), 4)
    assert dt["unb_b"].type == dds.SequenceType(dds.StringType(10))
    assert dt["unb_unb"].type == dds.SequenceType(dds.StringType())
    assert dt["w_b_b"].type == dds.SequenceType(dds.WStringType(6), 3)
    assert dt["w_b_unb"].type == dds.SequenceType(dds.WStringType(), 4)
    assert dt["w_unb_b"].type == dds.SequenceType(dds.WStringType(10))
    assert dt["w_unb_unb"].type == dds.SequenceType(dds.WStringType())


def test_sequence_serialization(sequence_sample):
    ts = idl.get_type_support(SequenceTest)
    buffer = ts.serialize(sequence_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert sequence_sample == deserialized_sample
    print(deserialized_sample)


def test_sequence_pubsub(shared_participant, sequence_sample):
    fixture = PubSubFixture(shared_participant, SequenceTest)
    fixture.writer.write(sequence_sample)
    wait.for_data(fixture.reader)
    fixture.writer.write(SequenceTest())
    wait.for_data(fixture.reader)
    assert fixture.reader.take_data() == [sequence_sample, SequenceTest()]


def test_sequence_serialization_fails_when_out_of_bounds():
    ts = idl.get_type_support(SequenceTest)

    sample = SequenceTest(b_b=["hello!"] * 3)
    ts.serialize(sample)
    sample.b_b += "world!"
    with pytest.raises(Exception) as ex:
        ts.serialize(sample)
    assert "Error processing field 'b_b'" in str(ex.value)

    sample = SequenceTest(b_b=["hello!!"])
    with pytest.raises(Exception) as ex:
        ts.serialize(sample)
    print()
    print(ex.value)
    print()
    assert "Error processing field 'b_b'" in str(ex.value)

