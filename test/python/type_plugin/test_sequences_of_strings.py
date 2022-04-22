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

from typing import Sequence, Optional
from dataclasses import field

import pytest

import rti.connextdds as dds
import rti.idl as idl

from test_utils.fixtures import *

import test_strings

@idl.struct(
    member_annotations={
        'b_b': [idl.bound(3), idl.element_annotations([idl.bound(6)])],
        'b_unb': [idl.bound(4)],
        'unb_b': [idl.element_annotations([idl.bound(10)])],
        'w_b_b': [idl.bound(3), idl.element_annotations([idl.bound(6), idl.utf16])],
        'w_b_unb': [idl.bound(4), idl.element_annotations([idl.utf16])],
        'w_unb_b': [idl.element_annotations([idl.bound(10), idl.utf16])],
        'w_unb_unb': [idl.element_annotations([idl.utf16])],
        'strings': [idl.bound(3)]
    }
)
class SequenceTest:
    # sequences of strings
    b_b: Sequence[str] = field(default_factory=list)
    b_unb: Sequence[str] = field(default_factory=list)
    unb_b: Sequence[str] = field(default_factory=list)
    unb_unb: Sequence[str] = field(default_factory=list)

    # sequences of wide strings
    w_b_b: Sequence[str] = field(default_factory=list)
    w_b_unb: Sequence[str] = field(default_factory=list)
    w_unb_b: Sequence[str] = field(default_factory=list)
    w_unb_unb: Sequence[str] = field(default_factory=list)

    # sequence of struct with strings/wide strings
    strings: Sequence[test_strings.StringTest] = field(default_factory=list)


@idl.struct(
    member_annotations={
        'b_b': [idl.bound(3), idl.element_annotations([idl.bound(6)])],
        'b_unb': [idl.bound(4)],
        'unb_b': [idl.element_annotations([idl.bound(10)])],
        'w_b_b': [idl.bound(3), idl.element_annotations([idl.bound(6), idl.utf16])],
        'w_b_unb': [idl.bound(4), idl.element_annotations([idl.utf16])],
        'w_unb_b': [idl.element_annotations([idl.bound(10), idl.utf16])],
        'w_unb_unb': [idl.element_annotations([idl.utf16])],
        'strings': [idl.bound(3)]
    }
)
class OptionalSequenceTest:
    b_b: Optional[Sequence[str]] = None
    b_unb: Optional[Sequence[str]] = None
    unb_b: Optional[Sequence[str]] = None
    unb_unb: Optional[Sequence[str]] = None

    w_b_b: Optional[Sequence[str]] = None
    w_b_unb: Optional[Sequence[str]] = None
    w_unb_b: Optional[Sequence[str]] = None
    w_unb_unb: Optional[Sequence[str]] = None

    strings: Optional[Sequence[test_strings.StringTest]] = None


def create_sample(SequenceType):
    return SequenceType(
        b_b=["hello", ",", "world!"],
        b_unb=["a" * 10, "b" * 20],
        unb_b=["hello", ",", "world!"] * 3,
        unb_unb=["a" * 10, "b" * 20] * 10,
        w_b_b=["hello", ",", "world!"],
        w_b_unb=["a" * 10, "b" * 20],
        w_unb_b=["hello", ",", "world!"] * 3,
        w_unb_unb=["a" * 10, "b" * 20] * 10,
        strings=[test_strings.StringTest(
            "hello 1",
            "hello world 2",
            "hello 3",
            "hello world 4",
            "hello 1",
            "hello world 2",
            "hello 3",
            "hello world 4")] * 3)

@pytest.fixture
def sequence_sample():
    return create_sample(SequenceTest)


@pytest.fixture
def optional_sequence_sample():
    return create_sample(OptionalSequenceTest)

def test_sequence_plugin():
    ts = idl.get_type_support(SequenceTest)
    assert ts.type is SequenceTest

    dt = ts.dynamic_type
    assert dt.name == "SequenceTest"
    assert len(dt.members()) == 9

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


def test_optional_sequence_serialization(optional_sequence_sample):
    ts = idl.get_type_support(OptionalSequenceTest)
    buffer = ts.serialize(optional_sequence_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert optional_sequence_sample == deserialized_sample


def test_sequence_pubsub(shared_participant, sequence_sample):
    fixture = PubSubFixture(shared_participant, SequenceTest, reader_policies=[
                            dds.ResourceLimits(1, 1, 1)])
    fixture.send_and_check(SequenceTest(b_b=['a']))
    fixture.send_and_check(SequenceTest(b_b=['a' * 6]))
    fixture.send_and_check(sequence_sample)


def test_optional_sequence_pubsub(shared_participant, optional_sequence_sample):
    fixture = PubSubFixture(
        shared_participant,
        OptionalSequenceTest,
        reader_policies=[dds.ResourceLimits(1, 1, 1)])
    fixture.send_and_check(optional_sequence_sample)
    fixture.send_and_check(OptionalSequenceTest())
    fixture.send_and_check(optional_sequence_sample)


@pytest.mark.parametrize("SequenceTestType", [SequenceTest, OptionalSequenceTest])
def test_sequence_serialization_fails_when_out_of_bounds(SequenceTestType):
    ts = idl.get_type_support(SequenceTestType)

    sample = SequenceTestType(b_b=["hello!"] * 3)
    ts.serialize(sample)
    sample.b_b += "world!"
    with pytest.raises(Exception) as ex:
        ts.serialize(sample)
    assert "Error processing field 'b_b'" in str(ex.value)

    sample = SequenceTestType(b_b=["hello!!"])
    with pytest.raises(Exception) as ex:
        ts.serialize(sample)
    assert "Error processing field 'b_b'" in str(ex.value)


def test_to_dynamic_data(sequence_sample):
    ts = idl.get_type_support(SequenceTest)
    assert sequence_sample == ts.from_dynamic_data(
        ts.to_dynamic_data(sequence_sample))
