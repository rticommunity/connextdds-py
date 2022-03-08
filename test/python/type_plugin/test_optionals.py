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
from test_utils.helpers import type_support_with_duck_typing

from common_types import Point
from test_utils.fixtures import *

from test_sequences import SequenceTest, create_sequence_sample


@idl.struct
class OptionalsTest:
    opt_primitive: Optional[float] = None
    opt_simple_struct: Optional[Point] = None
    opt_complex_struct: Optional[SequenceTest] = None

# Note: optional sequences and strings are tested in their respective tests

def create_optionals_sample():
    return OptionalsTest(
        opt_primitive=3.4,
        opt_simple_struct=Point(1, 2),
        opt_complex_struct=create_sequence_sample())


@pytest.fixture
def optionals_sample():
    return create_optionals_sample()



def test_optionals_default_value_serialization():
    ts = idl.get_type_support(OptionalsTest)
    buffer = ts.serialize(OptionalsTest())
    deserialized_sample = ts.deserialize(buffer)
    assert OptionalsTest() == deserialized_sample
    assert deserialized_sample.opt_simple_struct is None
    assert deserialized_sample.opt_primitive is None
    assert deserialized_sample.opt_complex_struct is None


def test_optionals_serialization(optionals_sample: OptionalsTest):
    ts = idl.get_type_support(OptionalsTest)
    buffer = ts.serialize(optionals_sample)
    deserialized_sample: OptionalsTest = ts.deserialize(buffer)
    assert optionals_sample == deserialized_sample


def test_optionals_pubsub(shared_participant, optionals_sample: OptionalsTest):
    fixture = PubSubFixture(shared_participant, OptionalsTest)
    fixture.send_and_check(optionals_sample)


def test_optionals_default_pubsub(shared_participant):
    fixture = PubSubFixture(shared_participant, OptionalsTest)
    fixture.send_and_check(OptionalsTest())


def test_optionals_variable_length_pubsub(shared_participant, optionals_sample: OptionalsTest):
    fixture = PubSubFixture(shared_participant, OptionalsTest, reader_policies=[
                            dds.ResourceLimits(1, 1, 1)])
    fixture.send_and_check(optionals_sample) # all optionals are set
    fixture.send_and_check(OptionalsTest()) # all optionals are unset
    optionals_sample.opt_primitive = 0.0
    fixture.send_and_check(optionals_sample) # all optionals are set again


def test_optionals_member_can_be_undefined_with_duck_typing_enabled(shared_participant):

    # We define a type that only has one of the members of OptionalsTest
    class CompatibleType:
        def __init__(self):
            self.opt_primitive = 5.5

    fixture = PubSubFixture(shared_participant, OptionalsTest)
    with type_support_with_duck_typing(OptionalsTest):
        fixture.writer.write(CompatibleType())

    wait.for_data(fixture.reader)

    # We receive an OptionalsTest with all members set to None except for
    # the one that was present in CompatibleType
    assert fixture.reader.take_data() == [OptionalsTest(opt_primitive=5.5)]



def test_optional_arrays_not_supported():
    with pytest.raises(TypeError) as ex:
        @idl.struct(
            member_annotations={
                'vertices': [idl.array(3)]
            }
        )
        class ArrayTest:
            vertices: Optional[Sequence[Point]] = None

    assert "Optional arrays are not supported" in str(ex.value)