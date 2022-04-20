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

from enum import IntEnum, auto
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


@idl.struct(member_annotations={
    'my_seq': [idl.bound(100)],
})
class ComplexSequence:
    my_seq: Sequence[Point] = field(default_factory=list)


@idl.struct
class StringSequence:
    my_seq: Sequence[str] = field(default_factory=list)


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


@idl.struct(
    member_annotations={
        'prices': [idl.bound(4)],
        'prices_array': [idl.bound(4)],
        'vertices_bounded': [idl.bound(2)]
    }
)
class SequenceTest:
    vertices: Sequence[Point] = field(default_factory=list)
    weights: Sequence[int] = field(default_factory=list)
    prices: Sequence[float] = field(default_factory=list)
    ready: Sequence[bool] = field(default_factory=list)

    # These are IDL sequences, but use array instead of list in py for better
    # performance. Our mapping supports both for primitive types.
    weights_array: Sequence[idl.int16] = field(
        default_factory=idl.array_factory(idl.int16))
    prices_array: Sequence[float] = field(
        default_factory=idl.array_factory(float))
    ready_array: Sequence[bool] = field(
        default_factory=idl.array_factory(bool))

    vertices_bounded: Sequence[Point] = field(default_factory=list)


@idl.struct(
    member_annotations={
        'vertices': [idl.array(3)],
        'weights': [idl.array(4)],
        'prices': [idl.array(5)],
        'custom_factory': [idl.array(3)],
        'strings': [idl.array(3), idl.element_annotations([idl.bound(5)])],
        'wstrings': [idl.array(3), idl.element_annotations([idl.utf16, idl.bound(5)])],
        'complex': [idl.array(3)],
        'multi_str': [idl.array([2, 3])],
        'multi_int': [idl.array([3, 2])]
    }
)
class ArrayTest:
    vertices: Sequence[Point] = field(
        default_factory=idl.list_factory(Point, 3))
    weights: Sequence[idl.int16] = field(
        default_factory=idl.array_factory(idl.int16, 4))
    prices: Sequence[float] = field(default_factory=idl.list_factory(float, 5))
    custom_factory: Sequence[int] = field(default_factory=lambda: [1, 2, 3])
    strings: Sequence[str] = field(default_factory=idl.list_factory(str, 3))
    wstrings: Sequence[str] = field(default_factory=idl.list_factory(str, 3))
    complex: Sequence[SequenceTest] = field(
        default_factory=idl.list_factory(SequenceTest, 3))
    multi_str: Sequence[str] = field(
        default_factory=idl.list_factory(str, [2, 3]))
    multi_int: Sequence[int] = field(
        default_factory=idl.array_factory(int, [3, 2]))


@idl.struct(member_annotations={
    "sequences_b": [idl.bound(3)],
    "sequences_b_o": [idl.bound(3)]
}
)
class SequenceOfSeqTest:
    sequences_u: Sequence[SequenceTest] = field(default_factory=list)
    sequences_b: Sequence[SequenceTest] = field(default_factory=list)
    sequences_u_o: Optional[Sequence[SequenceTest]] = None
    sequences_b_o: Optional[Sequence[SequenceTest]] = None


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
class StrSequenceTest:
    b_b: Sequence[str] = field(default_factory=list)
    b_unb: Sequence[str] = field(default_factory=list)
    unb_b: Sequence[str] = field(default_factory=list)
    unb_unb: Sequence[str] = field(default_factory=list)

    w_b_b: Sequence[str] = field(default_factory=list)
    w_b_unb: Sequence[str] = field(default_factory=list)
    w_unb_b: Sequence[str] = field(default_factory=list)
    w_unb_unb: Sequence[str] = field(default_factory=list)


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
class OptionalStrSequenceTest:
    b_b: Optional[Sequence[str]] = None
    b_unb: Optional[Sequence[str]] = None
    unb_b: Optional[Sequence[str]] = None
    unb_unb: Optional[Sequence[str]] = None

    w_b_b: Optional[Sequence[str]] = None
    w_b_unb: Optional[Sequence[str]] = None
    w_unb_b: Optional[Sequence[str]] = None
    w_unb_unb: Optional[Sequence[str]] = None


@idl.enum
class Color(IntEnum):
    RED = auto()
    GREEN = auto()
    BLUE = auto()


@idl.struct()
class EnumTest:
    color: Color = field(default=Color.RED)
