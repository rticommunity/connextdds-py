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
from enum import IntEnum

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


@idl.union
class UnionTest:
    discriminator: idl.int16 = 0
    value: Union[int, str, Point, Person] = 0

    my_int: int = idl.case(0)
    my_str: str = idl.case(1)
    my_point: Point = idl.case(2, 3)
    my_person: Person = idl.case(4, 5)
    

@idl.union
class UnionWithDefault:
    discriminator: idl.int16 = 2
    value: Union[int, str, Point] = field(default_factory=Point)

    my_int: int = idl.case(0)
    my_str: str = idl.case(1)
    my_point: Point = idl.case(2, is_default=True)


@idl.union(
    member_annotations={
        "my_str": [idl.bound(10)]
    }
)
class UnionWithDefault2:
    discriminator: idl.int16 = 2
    value: Union[int, str, Point] = field(default_factory=Point)

    my_int: int = idl.case(0)
    my_str: str = idl.case(1)
    my_point: Point = idl.case(is_default=True)


@idl.enum(type_annotations=[idl.mutable])
class Option(IntEnum):
    OPTION2 = 2
    OPTION3 = 3
    OPTION5 = 5
    OPTION6 = 6
    OPTION9 = 9


@idl.union(
    member_annotations={
        "my_str": [idl.bound(10)]
    }
)
class UnionWithEnum:
    discriminator: Option = Option.OPTION2
    value: Union[int, str, Point] = 0

    my_int: int = idl.case(Option.OPTION2)
    my_str: str = idl.case(Option.OPTION3)
    my_point: Point = idl.case(Option.OPTION5, Option.OPTION6)


@idl.enum
class Numbers(IntEnum):
    ZERO = 0
    ONE = 1
    TWO = 2
    THREE = 3


@idl.union
class UnionWithEnumDefault:
    discriminator: Numbers = Numbers.TWO
    value: Union[int, str, Point] = field(default_factory=Point)

    my_int: int = idl.case(Numbers.ZERO)
    my_str: str = idl.case(Numbers.ONE)
    my_point: Point = idl.case(is_default=True)


@idl.alias
class UnionWithEnumDefaultSeqAlias:
    value: Sequence[UnionWithEnumDefault] = field(default_factory=list)


@idl.struct
class UnionWithEnumDefaultSeq:
    x: UnionWithEnumDefaultSeqAlias = field(
        default_factory=UnionWithEnumDefaultSeqAlias)
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


@idl.struct
class PrimitiveType:
    myInt8: idl.int8 = 0
    myUint8: idl.int8 = 0
    myChar: str = ""
    myWchar: str = ""
    myOctet: idl.int8 = 0
    myShort: idl.int16 = 0
    myUnsignedShort: idl.uint16 = 0
    myLong: idl.int32 = 0
    myUnsignedLong: idl.uint32 = 0
    myLongLong: idl.int64 = 0
    myUnsignedLongLong: int = 0
    myFloat: idl.float32 = 0.0
    myDouble: float = 0.0
    myLongDouble: float = 0.0
    myBoolean: bool = False


@idl.alias(
    annotations=[idl.array([4])]
)
class MyShortArray_4:
    value: Sequence[idl.int16] = field(
        default_factory=idl.array_factory(idl.int16, [4]))


@idl.alias(
    annotations=[idl.bound(4)]
)
class MyShortSeq_4:
    value: Sequence[idl.int16] = field(
        default_factory=idl.array_factory(idl.int16))


@idl.alias(
    annotations=[idl.bound(100)]
)
class MyShortSeq_Unbounded:
    value: Sequence[idl.int16] = field(
        default_factory=idl.array_factory(idl.int16))


@idl.struct(
    member_annotations={
        'myShortArraySeq': [idl.bound(2)],
        'myShortSeqSeq': [idl.bound(2)],
        'myShortSeqSeq_2': [idl.bound(2)],
    }
)
class SequenceAliasTest:
    myShortArraySeq: Sequence[MyShortArray_4] = field(default_factory=list)
    myShortSeqSeq: Sequence[MyShortSeq_4] = field(default_factory=list)
    myShortSeqSeq_2: Sequence[MyShortSeq_Unbounded] = field(
        default_factory=list)


@idl.struct(
    member_annotations={
        'key': [idl.key],
    }
)
class KeyTest:
    key: int = 0
    value: int = 0
    

@idl.union(
    member_annotations={
        'string_case_10_member': [idl.bound(20)],
        'float_arr_case_15_member': [idl.array([3])]
    }
)
class LongUnionType:

    discriminator: idl.int32 = 0

    value: Union[idl.int16, str, Sequence[idl.float32]] = 0

    short_case_0_1_member: idl.int16 = idl.case(0, 1)
    string_case_10_member: str = idl.case(10)
    float_arr_case_15_member: Sequence[idl.float32] = idl.case(15)
