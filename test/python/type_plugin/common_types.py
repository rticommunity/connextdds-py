#
# (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

import rti.idl as idl
from dataclasses import field
from typing import *

## TODO: Remove all comments from this file


@idl.struct
class Point:
    x: int = 0
    y: int = 0


@idl.struct
class Point3D:
    x: float = 0
    y: float = 0
    z: float = 0


@idl.struct
class Line:
    pointA: Point = Point(0, 0)
    pointB: Point = Point(1, 1)


@idl.struct(
    type_annotations=[idl.mutable],
    member_annotations={
        'my_seq': [idl.bound(123), idl.id(2)],
        #'my_key': [idl.key],
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


@idl.struct(member_annotations={
    'my_seq': [idl.bound(100)],
})
class ComplexSequence:
    my_seq: Sequence[Point] = field(default_factory=list)


@idl.struct()
class SequenceArrayTest:
    int_array: Sequence[idl.int32] = field(
        default_factory=idl.array_factory(idl.int32))


@idl.struct(member_annotations={
    'int_array': [idl.bound(123)],
})
class BoundSequenceArrayTest:
    int_array: Sequence[idl.int32] = field(
        default_factory=idl.array_factory(idl.int32))


@idl.struct(member_annotations={
    'value': [idl.bound(4)]
})
class BoundString:
    value: str = ""


@idl.struct(member_annotations={})
class UnboundString:
    value: str = ""


# TODO: Uncomment this when ranges are available
"""
@idl.struct(member_annotations={
    'x': [idl.min(1), idl.max(10)]
})
class RangedPoint:
    x: int = 2
    y: int = 2
"""


@idl.struct()
class Person:
    first_name: str = ""
    middle_name: Optional[str] = None
    last_name: str = ""


@idl.struct(
    member_annotations={
        "opt_primitive_b_sequence": [idl.bound(4)],
        "opt_complex_b_sequence": [idl.bound(3)]
    }
)
class OptionalsTest:
    opt_primitive: Optional[float] = None
    opt_simple_struct: Optional[Point] = None
    opt_complex_struct: Optional[Line] = None
