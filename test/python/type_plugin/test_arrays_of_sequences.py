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

import rti.idl as idl

from test_utils.fixtures import *
from test_strings import StringTest, create_string_sample

@idl.alias(annotations=[idl.bound(4)])
class ComplexSequence:
    value: Sequence[StringTest] = field(default_factory=list)


@idl.alias
class ComplexUnbSequence:
    value: Sequence[StringTest] = field(default_factory=list)

@idl.alias(annotations=[idl.array(3)])
class ArrayOfComplexSequences:
    value: Sequence[ComplexSequence] = field(
        default_factory=idl.list_factory(ComplexSequence, 3))


@idl.alias(annotations=[idl.array(3)])
class ArrayOfComplexUnbSequences:
    value: Sequence[ComplexUnbSequence] = field(
        default_factory=idl.list_factory(ComplexUnbSequence, 3))

@idl.struct
class ArrayTest:
    opt_array_of_complex_seq: Optional[ArrayOfComplexSequences] = None
    opt_array_of_u_complex_seq: Optional[ArrayOfComplexUnbSequences] = None

def create_array_sample():
    return ArrayTest(
        opt_array_of_complex_seq=ArrayOfComplexSequences([
            ComplexSequence(
                [create_string_sample(), StringTest(), create_string_sample()]),
            ComplexSequence([]),
            ComplexSequence([create_string_sample()] * 4)
        ]),
        opt_array_of_u_complex_seq=ArrayOfComplexUnbSequences([
            ComplexUnbSequence(
                [create_string_sample(), StringTest(), create_string_sample()] * 2),
            ComplexUnbSequence([]),
            ComplexUnbSequence([create_string_sample()])
        ])
    )


@pytest.fixture
def array_sample():
    return create_array_sample()


def test_array_serialization(array_sample: ArrayTest):
    ts = idl.get_type_support(ArrayTest)
    buffer = ts.serialize(array_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert array_sample == deserialized_sample
    print(deserialized_sample)

def test_array_pubsub(shared_participant, array_sample: ArrayTest):
    fixture = PubSubFixture(shared_participant, ArrayTest)

    # First sample
    fixture.send_and_check(array_sample)

    # Unset optionals
    fixture.send_and_check(ArrayTest())

    # Expanded sequences
    array_sample.opt_array_of_complex_seq.value[0].value.append(create_string_sample())
    array_sample.opt_array_of_u_complex_seq.value[0].value.append(create_string_sample())
    fixture.send_and_check(array_sample)

    # Arrays of empty sequences
    fixture.send_and_check(
        ArrayTest(
            opt_array_of_complex_seq=ArrayOfComplexSequences([ComplexSequence()] * 3),
            opt_array_of_u_complex_seq=ArrayOfComplexUnbSequences([ComplexUnbSequence()] * 3))
    )


def test_array_publication_fails_with_incorrect_element_type(shared_participant, array_sample):
    fixture = PubSubFixture(shared_participant, ArrayTest)
    array_sample.opt_array_of_complex_seq.value[2].value[0] = "bad"
    with pytest.raises(idl.FieldSerializationError) as ex:
        fixture.writer.write(array_sample)
    assert "Error processing field 'opt_array_of_complex_seq'" in str(ex.value)



def test_array_publication_fails_when_out_of_bounds(shared_participant, array_sample):
    fixture = PubSubFixture(shared_participant, ArrayTest)
    array_sample.opt_array_of_complex_seq.value[0].value.append(
        [StringTest()] * 2)
    with pytest.raises(idl.FieldSerializationError) as ex:
        fixture.writer.write(array_sample)
    assert "Error processing field 'opt_array_of_complex_seq'" in str(ex.value)
