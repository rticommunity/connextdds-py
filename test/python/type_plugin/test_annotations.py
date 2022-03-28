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

from typing import *
from dataclasses import field

import rti.connextdds as dds
import rti.idl as idl

import pytest
from common_types import Point
from test_utils.fixtures import *


@idl.struct(
    type_annotations=[idl.mutable],
    member_annotations={
        'my_seq': [idl.bound(123), idl.id(2)],
        'my_key': [idl.key],
        'my_int32': [idl.id(10)]
    }
)
class AnnotationsTest:
    my_seq: Sequence[Point] = field(default_factory=list)
    my_key: int = 0
    my_int8: idl.int8 = 0
    my_int16: idl.int16 = 0
    my_uint16: idl.uint16 = 0
    my_int32: idl.int32 = 0
    my_uint32: idl.uint32 = 0
    my_uint64: idl.uint64 = 0
    my_float32: idl.float32 = 0.0
    my_float64: idl.float64 = 0.0
    my_float64_2: float = 0.0
    my_bool: bool = False
    my_optional_int32: Optional[idl.int32] = None
    my_optional_float64: Optional[float] = None
    my_optional_seq: Optional[Sequence[Point]] = None


@idl.union(
    type_annotations=[idl.mutable],
    member_annotations={
        'my_seq': [idl.bound(123), idl.id(2)],
        'my_int32': [idl.id(10)]
    }
)
class UnionAnnotationsTest:
    discriminator: idl.int16 = 0
    value: Union[int, str, Point] = 0

    my_seq: ClassVar[Sequence[Point]] = idl.case(0)
    my_key: ClassVar[int] = idl.case(1)
    my_int8: ClassVar[idl.int8] = idl.case(2)
    my_int16: ClassVar[idl.int16] = idl.case(3)
    my_uint16: ClassVar[idl.uint16] = idl.case(4)
    my_int32: ClassVar[idl.int32] = idl.case(5)
    my_uint32: ClassVar[idl.uint32] = idl.case(6)
    my_uint64: ClassVar[idl.uint64] = idl.case(7)
    my_float32: ClassVar[idl.float32] = idl.case(8)
    my_float64: ClassVar[idl.float64] = idl.case(9)
    my_float64_2: ClassVar[float] = idl.case(10)
    my_bool: ClassVar[bool] = idl.case(11)


@pytest.fixture(scope="module", params=[AnnotationsTest, UnionAnnotationsTest])
def dt(request):
    return idl.get_type_support(request.param).dynamic_type


def test_mutable_annotation(dt):
    assert dt.extensibility_kind == dds.ExtensibilityKind.MUTABLE
    assert dt["my_seq"].type.content_type.extensibility_kind == dds.ExtensibilityKind.EXTENSIBLE


def test_id_annotation(dt):
    assert dt["my_seq"].id == 2
    assert dt["my_key"].id == 3
    assert dt["my_int32"].id == 10
    assert dt["my_uint32"].id == 11


def test_key_annotation():
    dt = idl.get_type_support(AnnotationsTest).dynamic_type
    assert not dt["my_seq"].is_key
    assert dt["my_key"].is_key
    assert not dt["my_int32"].is_key


def test_bound_annotation(dt):
    assert dt["my_seq"].type.bounds == 123


def test_primitive_types(dt):
    assert dt["my_key"].type == dds.Int64Type()
    assert dt["my_int8"].type == dds.Int8Type()
    assert dt["my_int16"].type == dds.Int16Type()
    assert dt["my_uint16"].type == dds.Uint16Type()
    assert dt["my_int32"].type == dds.Int32Type()
    assert dt["my_uint32"].type == dds.Uint32Type()
    assert dt["my_uint64"].type == dds.Uint64Type()
    assert dt["my_float32"].type == dds.Float32Type()
    assert dt["my_float64"].type == dds.Float64Type()
    assert dt["my_float64_2"].type == dds.Float64Type()
    assert dt["my_bool"].type == dds.BoolType()


def test_optional_annotation():
    dt = idl.get_type_support(AnnotationsTest).dynamic_type
    assert dt["my_optional_int32"].type == dds.Int32Type()
    assert dt["my_optional_float64"].type == dds.Float64Type()
    assert dt["my_optional_int32"].optional
    assert dt["my_optional_float64"].optional
    assert dt["my_optional_seq"].optional
    assert not dt["my_int32"].optional
    assert not dt["my_float64"].optional

