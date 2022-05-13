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

import pytest

import rti.connextdds as dds
import rti.idl as idl

from test_utils.fixtures import *
import test_sequences

InnerSeqTest = test_sequences.SequenceTest

# Tests a sequence of a type that contains more sequences. This is important
# to test that the inner sequences are properly initialized
@idl.struct(member_annotations={
        "sequences_b": [idl.bound(3)],
        "sequences_b_o": [idl.bound(3)]
    }
)
class SequenceTest:
    sequences_u: Sequence[InnerSeqTest] = field(default_factory=list)
    sequences_b: Sequence[InnerSeqTest] = field(default_factory=list)
    sequences_u_o: Optional[Sequence[InnerSeqTest]] = None
    sequences_b_o: Optional[Sequence[InnerSeqTest]] = None


def create_sequence_sample(seq_length: int = 3):
    return SequenceTest(
        sequences_u=[test_sequences.create_sequence_sample()] * seq_length,
        sequences_b=[test_sequences.create_sequence_sample()] * seq_length,
        sequences_u_o=[test_sequences.create_sequence_sample()] * seq_length,
        sequences_b_o=[test_sequences.create_sequence_sample()] * seq_length)


@pytest.fixture
def sequence_sample():
    return create_sequence_sample()


def test_sequence_plugin():
    ts = idl.get_type_support(SequenceTest)
    assert ts.type is SequenceTest

    dt = ts.dynamic_type
    assert dt.name == "SequenceTest"
    assert len(dt.members()) == 4

    element_ts = idl.get_type_support(test_sequences.SequenceTest)
    assert dt["sequences_u"].type == dds.SequenceType(element_ts.dynamic_type)
    assert dt["sequences_b"].type == dds.SequenceType(element_ts.dynamic_type, 3)
    assert dt["sequences_u_o"].type == dds.SequenceType(
        element_ts.dynamic_type)
    assert dt["sequences_b_o"].type == dds.SequenceType(
        element_ts.dynamic_type, 3)

    assert dt["sequences_u_o"].optional
    assert dt["sequences_b_o"].optional


def test_sequence_serialization(sequence_sample):
    ts = idl.get_type_support(SequenceTest)
    buffer = ts.serialize(sequence_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert sequence_sample == deserialized_sample


def test_empty_sequence_serialization():
    ts = idl.get_type_support(SequenceTest)
    buffer = ts.serialize(SequenceTest())
    deserialized_sample = ts.deserialize(buffer)
    assert SequenceTest() == deserialized_sample


def test_sequence_pubsub_basic(shared_participant, sequence_sample: SequenceTest):
    fixture = PubSubFixture(shared_participant, SequenceTest)
    fixture.send_and_check(sequence_sample)


def test_sequence_memory_is_managed_pubsub(shared_participant):
    fixture = PubSubFixture(shared_participant, SequenceTest, reader_policies=[
                            dds.ResourceLimits(1, 1, 1)])
    fixture.send_and_check(create_sequence_sample(2))
    fixture.send_and_check(create_sequence_sample(3))
    fixture.send_and_check(create_sequence_sample(0))
    fixture.send_and_check(create_sequence_sample(1))


def test_sequence_variable_lengths_pubsub(
    shared_participant: dds.DomainParticipant,
    sequence_sample: SequenceTest
):
    fixture = PubSubFixture(shared_participant, SequenceTest, reader_policies=[
                            dds.ResourceLimits(1, 1, 1)])
    fixture.writer.write(sequence_sample)
    wait.for_data(fixture.reader)
    assert fixture.reader.take_data() == [sequence_sample]
    fixture.writer.write(SequenceTest())
    wait.for_data(fixture.reader)
    assert fixture.reader.take_data() == [SequenceTest()]

    del sequence_sample.sequences_u[1].prices[1]
    sequence_sample.sequences_u[1].vertices.append(
        sequence_sample.sequences_u[1].vertices[0])
    sequence_sample.sequences_u[1].vertices_bounded.append(
        sequence_sample.sequences_u[1].vertices_bounded[0])

    del sequence_sample.sequences_b[1].prices[1]
    sequence_sample.sequences_b[1].vertices.append(
        sequence_sample.sequences_b[1].vertices[0])
    sequence_sample.sequences_b[1].vertices_bounded.append(
        sequence_sample.sequences_b[1].vertices_bounded[0])
    sequence_sample.sequences_b_o = None
    sequence_sample.sequences_u_o = None
    fixture.writer.write(sequence_sample)
    wait.for_data(fixture.reader)
    assert fixture.reader.take_data() == [sequence_sample]
