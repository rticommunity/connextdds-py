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

from typing import Optional, Union
from dataclasses import field

import rti.connextdds as dds
import rti.idl as idl

from test_utils.fixtures import *
from test_sequences import SequenceTest, create_sequence_sample
from test_enums import Shape

@idl.struct(type_annotations=[idl.mutable])
class BigType:
    common_member: int = 0
    extended_primitive: int = 0
    extended_optional: Optional[int] = None
    extended_complex: SequenceTest = field(default_factory=SequenceTest)
    extended_enum: Shape = Shape.CIRCLE # Note that the default value is not 0


@idl.struct(type_annotations=[idl.mutable])
class SmallType:
    common_member: int = 0


@idl.union(type_annotations=[idl.mutable])
class BigUnion:
    discriminator: int = 0
    value: Union[int, BigType, idl.int16] = 0

    a: int = idl.case(0)
    b: BigType = idl.case(1)
    c: idl.int16 = idl.case(2)


@idl.union(type_annotations=[idl.mutable])
class SmallUnion:
    discriminator: int = 0
    value: Union[int, SmallType, idl.int16] = 0

    a: int = idl.case(0)
    b: SmallType = idl.case(1)
    c: idl.int16 = idl.case(2)

def test_big_sub_receives_correct_default_values_from_small_pub(shared_participant, participant):
    big_fx = PubSubFixture(
        shared_participant,
        BigType,
        topic_name="Mutability",
        reader_policies=[dds.ResourceLimits(1, 1, 1)])
    small_fx = PubSubFixture(participant, SmallType,
                             topic_name="Mutability", create_reader=False)
    wait.for_discovery(big_fx.reader, small_fx.writer)

    big_sample = BigType(common_member=5, extended_primitive=305419896,
                         extended_optional=20,
                         extended_complex=create_sequence_sample(),
                         extended_enum=Shape.SQUARE)
    big_fx.send_and_check(big_sample)

    small_sample = SmallType(common_member=44)
    small_fx.writer.write(small_sample)
    wait.for_data(big_fx.reader)
    received_sample = big_fx.reader.take_data()[0]

    # This is the only data that was actually published
    assert received_sample.common_member == small_sample.common_member

    # All members that are not defined in SmallType are initialized to their
    # expected default values
    assert received_sample.extended_primitive == 0
    assert received_sample.extended_optional is None
    assert received_sample.extended_complex == SequenceTest()
    assert received_sample.extended_enum == Shape.CIRCLE


def test_union_big_sub_receives_correct_default_values_from_small_pub(shared_participant, participant):
    big_fx = PubSubFixture(
        shared_participant,
        BigUnion,
        topic_name="Mutability",
        reader_policies=[dds.ResourceLimits(1, 1, 1)])
    small_fx = PubSubFixture(participant, SmallUnion,
                             topic_name="Mutability", create_reader=False)
    wait.for_discovery(big_fx.reader, small_fx.writer)

    big_sample = BigUnion(b=BigType(common_member=5, extended_primitive=305419896,
                         extended_optional=20,
                         extended_complex=create_sequence_sample(),
                         extended_enum=Shape.SQUARE))
    big_fx.send_and_check(big_sample)
    big_fx.send_and_check(BigUnion(c=5))

    small_sample = SmallUnion(b=SmallType(common_member=44))
    small_fx.writer.write(small_sample)
    wait.for_data(big_fx.reader)
    received_sample = big_fx.reader.take_data()[0]

    # This is the only data that was actually published
    assert received_sample.b.common_member == small_sample.b.common_member

    # All members that are not defined in SmallType are initialized to their
    # expected default values
    assert received_sample.b.extended_primitive == 0
    assert received_sample.b.extended_optional is None
    assert received_sample.b.extended_complex == SequenceTest()
    assert received_sample.b.extended_enum == Shape.CIRCLE
