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


from typing import Sequence, Union
from dataclasses import field

import rti.connextdds as dds
import rti.types as idl

from common_types import Point
from test_utils.fixtures import *


@idl.struct(type_annotations=[idl.allowed_data_representation(xcdr1=True, xcdr2=False)])
class TypeXcdr1:
    a: Sequence[Point] = field(default_factory=list)


@idl.struct(type_annotations=[idl.allowed_data_representation(xcdr1=False, xcdr2=True)])
class TypeXcdr2:
    a: Sequence[Point] = field(default_factory=list)


@idl.struct(type_annotations=[idl.allowed_data_representation(xcdr1=True, xcdr2=True)])
class TypeXcdrAll:
    a: Sequence[Point] = field(default_factory=list)


@idl.union(type_annotations=[idl.allowed_data_representation(xcdr2=False, xcdr1=True)])
class UnionXcdr1:

    discriminator: idl.int32 = 1
    value: Union[Sequence[Point], idl.int32] = field(default_factory=list)

    a: Sequence[Point] = idl.case(1)
    b: idl.int32 = idl.case(2)


@idl.union(type_annotations=[idl.allowed_data_representation(xcdr2=True, xcdr1=False)])
class UnionXcdr2:

    discriminator: idl.int32 = 1
    value: Union[Sequence[Point], idl.int32] = field(default_factory=list)

    a: Sequence[Point] = idl.case(1)
    b: idl.int32 = idl.case(2)


@pytest.mark.parametrize("types", [(TypeXcdr1, TypeXcdr2),  (UnionXcdr1, UnionXcdr2)])
def test_annotation_selects_type_support_data_representation(types):

    type1, type2 = types

    xcdr1_ts = idl.get_type_support(type1)
    xcdr2_ts = idl.get_type_support(type2)

    sample1 = type1(a=[Point(x=1, y=2)])
    xcdr1_buffer = xcdr1_ts.serialize(sample1)
    sample2 = type2(a=[Point(x=1, y=2)])
    xcdr2_buffer = xcdr2_ts.serialize(sample2)

    # The buffer is different only because the data representation is different
    assert xcdr1_buffer != xcdr2_buffer

    assert xcdr1_ts.deserialize(xcdr1_buffer) == sample1
    assert xcdr2_ts.deserialize(xcdr2_buffer) == sample2

def test_annotation_selects_data_representation_qos_policy(shared_participant: dds.DomainParticipant):
    participant1 = shared_participant
    participant2 = create_participant()

    topic1 = dds.Topic(participant1, "XcdrTest", TypeXcdr1)
    topic2 = dds.Topic(participant2, "XcdrTest", TypeXcdr2)

    writer = dds.DataWriter(participant1.implicit_publisher, topic1)
    reader = dds.DataReader(participant2.implicit_subscriber, topic2)

    # The writer only supports XCDR1 and the reader only supports XCDR2
    wait.until(
        lambda: reader.requested_incompatible_qos_status.last_policy is dds.DataRepresentation)
    wait.until(
        lambda: writer.offered_incompatible_qos_status.last_policy is dds.DataRepresentation)

    # The new reader supports both XCDR1 and 2:
    participant2.close_contained_entities()
    topic2 = dds.Topic(participant2, "XcdrTest", TypeXcdrAll)
    reader = dds.DataReader(participant2.implicit_subscriber, topic2)
    wait.for_discovery(reader, writer)

    writer.write(TypeXcdr1(a=[Point(x=1, y=2)] * 2))
    wait.for_data(reader)

    assert reader.take_data()[0].a == [Point(x=1, y=2)] * 2
