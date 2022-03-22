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

from dataclasses import field
from typing import Sequence, Optional

import pytest

import rti.connextdds as dds
import rti.idl as idl

from test_utils.fixtures import *
import test_utils.log_capture as log_capture

from common_types import Point


@idl.alias
class MyPoint:
    value: Point = field(default_factory=Point)


@idl.alias
class MyPointSeq:
    value: Sequence[Point] = field(default_factory=list)


@idl.alias(annotations=[idl.bound(5)])
class MyPoint05Seq:
    value: Sequence[Point] = field(default_factory=list)


@idl.alias(annotations=[idl.array(3)])
class MyPointArray:
    value: Sequence[Point] = field(default_factory=idl.list_factory(Point, 3))


@idl.struct(
    member_annotations={
        "arr_seq": [idl.array(2)]
    }
)
class AliasTest:
    point: MyPoint = field(default_factory=MyPoint)
    seq: Optional[Sequence[MyPoint]] = None
    seq_alias: MyPointSeq = field(default_factory=MyPointSeq)
    seq_seq: Sequence[MyPointSeq] = field(default_factory=list)
    opt_seq_seq: Optional[Sequence[MyPointSeq]] = None
    opt_seq_seq_b: Optional[Sequence[MyPoint05Seq]] = None
    arr_seq: Sequence[MyPointSeq] = field(
        default_factory=idl.list_factory(MyPointSeq, 2))
    seq_array: Sequence[MyPointArray] = field(default_factory=list)


def create_alias_sample():
    return AliasTest(
        point=MyPoint(Point(1, 2)),
        seq=[MyPoint(Point(3, 4)), MyPoint(Point(5, 6))],
        seq_alias=MyPointSeq([Point(3, 4), Point(5, 6)]),
        seq_seq=[
            MyPointSeq(value=[Point(3, 4), Point(5, 6)]),
            MyPointSeq(value=[])],
        opt_seq_seq=[
            MyPointSeq(value=[]),
            MyPointSeq(value=[Point(9, 4), Point(5, 8)]),
            MyPointSeq(value=[Point(3, 4)])],
        opt_seq_seq_b=[
            MyPoint05Seq(value=[]),
            MyPoint05Seq(value=[Point(9, 4), Point(5, 8)]),
            MyPoint05Seq(value=[Point(3, 4)])],
        arr_seq=[
            MyPointSeq(value=[Point(31, 14), Point(15, 16)]),
            MyPointSeq(value=[Point(13, 41)])],
        seq_array=[
            MyPointArray([Point(31, 14), Point(15, 16), Point(17, 18)]),
            MyPointArray([Point(13, 41), Point(19, 20), Point(21, 22)])]
    )


@pytest.fixture
def alias_sample():
    return create_alias_sample()


def test_alias_plugin():
    point_ts = idl.get_type_support(Point)
    point_seq_ts = idl.get_type_support(MyPointSeq)
    assert point_seq_ts.type is MyPointSeq
    point_seq_dt = point_seq_ts.dynamic_type
    assert point_seq_dt.kind == dds.TypeKind.ALIAS_TYPE
    assert point_seq_dt.name == "MyPointSeq"
    assert point_seq_dt.related_type() == dds.SequenceType(
        point_ts.dynamic_type)

    point_seq_b_ts = idl.get_type_support(MyPoint05Seq)
    assert point_seq_b_ts.type is MyPoint05Seq
    point_seq_b_dt = point_seq_b_ts.dynamic_type
    assert point_seq_b_dt.kind == dds.TypeKind.ALIAS_TYPE
    assert point_seq_b_dt.related_type() == dds.SequenceType(
        point_ts.dynamic_type, 5)

    point_array_ts = idl.get_type_support(MyPointArray)
    assert point_array_ts.type is MyPointArray
    point_array_dt = point_array_ts.dynamic_type
    assert point_array_dt.kind == dds.TypeKind.ALIAS_TYPE
    assert point_array_dt.related_type() == dds.ArrayType(
        point_ts.dynamic_type, 3)

    ts = idl.get_type_support(AliasTest)
    assert ts.dynamic_type["seq_seq"].type == dds.SequenceType(point_seq_dt)
    assert ts.dynamic_type["opt_seq_seq"].optional
    assert ts.dynamic_type["arr_seq"].type == dds.ArrayType(point_seq_dt, 2)
    assert ts.dynamic_type["seq_array"].type == dds.SequenceType(
        point_array_dt)



def test_alias_default_serialization():
    ts = idl.get_type_support(AliasTest)
    buffer = ts.serialize(AliasTest())
    deserialized_sample = ts.deserialize(buffer)
    assert AliasTest() == deserialized_sample


def test_alias_serialization(alias_sample):
    ts = idl.get_type_support(type(alias_sample))
    buffer = ts.serialize(alias_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert alias_sample == deserialized_sample


def test_alias_pubsub(shared_participant, alias_sample):
    fixture = PubSubFixture(shared_participant, AliasTest, reader_policies=[
                            dds.ResourceLimits(1, 1, 1)])
    fixture.send_and_check(alias_sample)
    fixture.send_and_check(AliasTest())
    fixture.send_and_check(alias_sample)


def test_invalid_alias_type_support_operations():
    ts = idl.get_type_support(MyPoint)

    with pytest.raises(dds.IllegalOperationError) as excinfo:
        ts.serialize(MyPoint())
    assert "Type 'MyPoint' doesn't support serialize" in str(excinfo.value)


def test_idl_alias_for_primitive_type_not_allowed():
    with log_capture.expected_exception(dds.IllegalOperationError) as errinfo:
        @idl.alias
        class BadAlias:
            value: int = 0
    # Check that the type name appears in both the exception and the Core logs
    assert "BadAlias" in errinfo.exception_msg
    assert "BadAlias" in errinfo.logs
