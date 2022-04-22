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

from dataclasses import field, asdict
from typing import Union, Sequence, Optional
from enum import IntEnum
import copy

import rti.connextdds as dds
import rti.idl as idl

from common_types import Point
from test_utils.fixtures import *


@idl.union(
    member_annotations={
        "my_str": [idl.bound(10)]
    }
)
class MyUnion:
    discriminator: idl.int16 = 0
    value: Union[int, str, Point] = 0

    my_int: int = idl.case(0)
    my_str: str = idl.case(1)
    my_point: Point = idl.case(2, 3)


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
class UnionWithEnumDefaultSeq:
    value: Sequence[UnionWithEnumDefault] = field(default_factory=list)

UNION_TYPES = [MyUnion, UnionWithDefault,
               UnionWithDefault2, UnionWithEnum, UnionWithEnumDefault]

UNIONS_WITH_DEFAULT = [UnionWithDefault,
                       UnionWithDefault2, UnionWithEnumDefault]


def create_union_sample(UnionType, index):
    if index == 0:
        return UnionType(my_int=42)
    elif index == 1:
        return UnionType(my_str="hello")
    elif index == 2:
        return UnionType(my_point=Point(x=1, y=2))
    else:
        return UnionType()

@idl.struct(
    member_annotations={
        "my_union_with_default_seq": [idl.bound(5)],
        'my_union_array': [idl.array(3)],
        'my_union_with_default_array': [idl.array(3)],
        'my_union_with_enum_array': [idl.array(3)],
        'my_union_with_enum_default_array': [idl.array(3)]
    }
)
class TypeWithUnions:
    my_union: MyUnion = MyUnion()
    my_union_with_default: UnionWithDefault = UnionWithDefault()
    my_union_with_enum: UnionWithEnum = UnionWithEnum()
    my_union_with_enum_default: UnionWithEnumDefault = UnionWithEnumDefault()

    # sequences of unions
    my_union_seq: Sequence[MyUnion] = field(default_factory=list)
    my_union_with_default_seq: Sequence[UnionWithDefault] = field(default_factory=list)
    my_union_with_enum_seq: Sequence[UnionWithEnum] = field(default_factory=list)
    my_union_with_enum_default_seq: Sequence[UnionWithEnumDefault] = field(default_factory=list)
    my_union_with_enum_default_2seq: Sequence[UnionWithEnumDefaultSeq] = field(default_factory=list)

    # arrays of unions
    my_union_array: Sequence[MyUnion] = field(
        default_factory=idl.list_factory(MyUnion, 3))
    my_union_with_default_array: Sequence[UnionWithDefault] = field(
        default_factory=idl.list_factory(UnionWithDefault, 3))
    my_union_with_enum_array: Sequence[UnionWithEnum] = field(
        default_factory=idl.list_factory(UnionWithEnum, 3))
    my_union_with_enum_default_array: Sequence[UnionWithEnumDefault] = field(
        default_factory=idl.list_factory(UnionWithEnumDefault, 3))

    # optional unions
    my_union_opt: Optional[MyUnion] = None
    my_union_with_default_opt: Optional[UnionWithDefault] = None
    my_union_with_enum_opt: Optional[UnionWithEnum] = None
    my_union_with_enum_default_opt: Optional[UnionWithEnumDefault] = None


def create_type_with_unions_sample(index):
    u = create_union_sample(MyUnion, index)
    uwd = create_union_sample(UnionWithDefault, index)
    uwe = create_union_sample(UnionWithEnum, index)
    uwed = create_union_sample(UnionWithEnumDefault, index)

    return TypeWithUnions(
        my_union=u,
        my_union_with_default=uwd,
        my_union_with_enum=uwe,
        my_union_with_enum_default=uwed,
        my_union_seq=[u] * (index + 1),
        my_union_with_default_seq=[uwd] * (index + 1),
        my_union_with_enum_seq=[uwe] * (index + 1),
        my_union_with_enum_default_seq=[uwed] * (index + 1),
        my_union_with_enum_default_2seq=[
            UnionWithEnumDefaultSeq([uwed, uwed]), UnionWithEnumDefaultSeq([uwed])],
        my_union_array=[u] * 3,
        my_union_with_default_array=[uwd] * 3,
        my_union_with_enum_array=[uwe] * 3,
        my_union_with_enum_default_array=[uwed] * 3,
        my_union_opt=u,
        my_union_with_default_opt=uwd,
        my_union_with_enum_opt=uwe,
        my_union_with_enum_default_opt=uwed
    )

def test_union_default_values():
    assert MyUnion.default_discriminator == 0
    assert MyUnion.default_value() == 0

    assert UnionWithDefault.default_discriminator == 2
    assert UnionWithDefault.default_value() == Point()

    assert UnionWithEnum.default_discriminator == Option.OPTION2
    assert UnionWithEnum.default_value() == 0

    assert UnionWithEnumDefault.default_discriminator == Numbers.TWO
    assert UnionWithEnumDefault.default_value() == Point()


def test_union_plugin():
    ts = idl.get_type_support(MyUnion)
    assert ts.type is MyUnion

    dt = ts.dynamic_type
    assert dt.name == "MyUnion"
    assert len(dt.members()) == 3

    assert dt.discriminator == dds.Int16Type()
    assert dt["my_int"].type == dds.Int64Type()
    assert dt["my_int"].labels == [0]
    assert dt["my_str"].type == dds.StringType(10)
    assert dt["my_str"].labels == [1]
    assert dt["my_point"].type == idl.get_type_support(Point).dynamic_type
    assert dt["my_point"].labels == [2, 3]


def test_default_union_plugin():
    dt = idl.get_type_support(UnionWithDefault).dynamic_type
    assert dt["my_point"].labels == [2, dds.UnionMember.DEFAULT_LABEL]
    dt = idl.get_type_support(UnionWithDefault2).dynamic_type
    assert dt["my_point"].labels == [dds.UnionMember.DEFAULT_LABEL]
    dt = idl.get_type_support(UnionWithEnumDefault).dynamic_type
    assert dt["my_point"].labels == [dds.UnionMember.DEFAULT_LABEL]


def test_enum_union_plugin():
    dt: dds.UnionType = idl.get_type_support(UnionWithEnum).dynamic_type
    assert dt.discriminator == idl.get_type_support(Option).dynamic_type
    assert dt["my_int"].labels == [Option.OPTION2]
    assert dt["my_str"].labels == [Option.OPTION3]
    assert dt["my_point"].labels == [Option.OPTION5, Option.OPTION6]


def test_union_creation():
    u = MyUnion()
    assert u.my_int == 0
    assert u.discriminator == 0

    u = MyUnion(my_int=1)
    assert u.my_int == 1
    assert u.discriminator == 0

    u = MyUnion(my_str="hello")
    assert u.my_str == "hello"
    assert u.discriminator == 1

    u = MyUnion(my_point=Point(3, 4))
    assert u.my_point == Point(3, 4)
    assert u.discriminator == 2


@pytest.mark.parametrize("UnionType", UNIONS_WITH_DEFAULT)
def test_default_union_creation(UnionType: type):
    u = UnionType()
    assert u.my_point == Point()
    assert u.discriminator == 2

    u = UnionType(my_int=1)
    assert u.my_int == 1
    assert u.discriminator == 0

    u = UnionType(my_str="hello")
    assert u.my_str == "hello"
    assert u.discriminator == 1

    u = UnionType(my_point=Point(3, 4))
    assert u.my_point == Point(3, 4)
    assert u.discriminator == 2

def test_enum_union_creation():
    u = UnionWithEnum()
    assert u.my_int == 0
    assert u.discriminator == Option.OPTION2

    u = UnionWithEnum(my_int=3)
    assert u.my_int == 3
    assert u.discriminator == Option.OPTION2

    u = UnionWithEnum(my_str="hello")
    assert u.my_str == "hello"
    assert u.discriminator == Option.OPTION3

    u = UnionWithEnum(my_point=Point(3, 4))
    assert u.my_point == Point(3, 4)
    assert u.discriminator == Option.OPTION5


def test_union_setter():
    u = MyUnion()
    u.my_int = 3
    assert u.my_int == 3
    assert u.discriminator == 0

    u.my_point = Point(3, 4)
    assert u.my_point == Point(3, 4)
    assert u.discriminator == 2

    u.my_str = "hello"
    assert u.my_str == "hello"
    assert u.discriminator == 1

    u.my_point = (Point(3, 5), 3)
    assert u.my_point == Point(3, 5)
    assert u.discriminator == 3

    u.my_int = 4
    with pytest.raises(ValueError):
        u.my_point = (Point(3, 4), 4)
    assert u.my_int == 4


@pytest.mark.parametrize("UnionType", UNIONS_WITH_DEFAULT)
def test_default_union_setter(UnionType: type):
    u = UnionType()
    u.my_int = 3
    assert u.my_int == 3
    assert u.discriminator == 0

    u.my_point = Point(3, 4)
    assert u.my_point == Point(3, 4)
    assert u.discriminator == 2

    u.my_str = "hello"
    assert u.my_str == "hello"
    assert u.discriminator == 1

    u.my_point = (Point(5, 6), 123)
    assert u.my_point == Point(5, 6)
    assert u.discriminator == 123

    with pytest.raises(ValueError):
        u.my_point = (Point(3, 4), 1)
    assert u.my_point == Point(5, 6)


def test_enum_union_setters():
    u = UnionWithEnum()
    u.my_int = 3
    assert u.my_int == 3
    assert u.discriminator == Option.OPTION2

    u.my_point = Point(3, 4)
    assert u.my_point == Point(3, 4)
    assert u.discriminator == Option.OPTION5

    u.my_str = "hello"
    assert u.my_str == "hello"
    assert u.discriminator == Option.OPTION3

    u.my_point = (Point(5, 6), Option.OPTION6)
    assert u.my_point == Point(5, 6)
    assert u.discriminator == Option.OPTION6

    with pytest.raises(ValueError):
        u.my_point = (Point(3, 4), Option.OPTION9)
    assert u.my_point == Point(5, 6)


def test_union_repr_contains_discr_and_value_only():
    u = MyUnion(my_str="hello")
    assert repr(u) == "MyUnion(discriminator=1, value='hello')"


@pytest.mark.parametrize("UnionType", UNION_TYPES)
def test_none_construction(UnionType: type):
    u = UnionType(None)
    assert u.value is None


def test_union_as_dict_contains_discr_and_value_only():
    u = MyUnion(my_str="hello")
    assert asdict(u) == {"discriminator": 1, "value": "hello"}


@pytest.mark.parametrize("UnionType", UNION_TYPES)
def test_union_serialization(UnionType: type):
    ts = idl.get_type_support(UnionType)
    union_sample = UnionType(my_point=Point(3, 4))
    assert union_sample == ts.deserialize(ts.serialize(union_sample))


@pytest.mark.parametrize("UnionType", UNION_TYPES)
def test_default_union_serialization(UnionType: type):
    ts = idl.get_type_support(UnionType)
    sample = UnionType()
    assert sample == ts.deserialize(ts.serialize(sample))


@pytest.mark.parametrize("UnionType", UNION_TYPES)
def test_union_serialization(UnionType: type):
    ts = idl.get_type_support(UnionType)
    sample = UnionType(my_point=Point(3, 4))
    assert sample == ts.deserialize(ts.serialize(sample))
    sample = UnionType(my_str="hello")
    assert sample == ts.deserialize(ts.serialize(sample))
    sample = UnionType(my_int=3)
    assert sample == ts.deserialize(ts.serialize(sample))


@pytest.mark.parametrize("UnionType", UNION_TYPES)
def test_union_pubsub(shared_participant, UnionType: type):
    fixture = PubSubFixture(shared_participant, UnionType, reader_policies=[
                            dds.ResourceLimits(1, 1, 1)])
    fixture.send_and_check(UnionType(my_point=Point(3, 4)))
    fixture.send_and_check(UnionType(my_str="hello"))
    fixture.send_and_check(UnionType(my_int=3))

def test_type_with_unions_pubsub(shared_participant):
    fixture = PubSubFixture(shared_participant, TypeWithUnions, reader_policies=[
                            dds.ResourceLimits(1, 1, 1)])
    fixture.send_and_check(create_type_with_unions_sample(1))
    fixture.send_and_check(create_type_with_unions_sample(0))
    fixture.send_and_check(create_type_with_unions_sample(2))
    fixture.send_and_check(TypeWithUnions())
    fixture.send_and_check(create_type_with_unions_sample(3))
    fixture.send_and_check(create_type_with_unions_sample(1))
    fixture.send_and_check(create_type_with_unions_sample(-1))

def test_bad_union():
    with pytest.raises(TypeError):
        @idl.union
        class NoDiscriminatorUnion:
            value: Union[int, str] = 0

            my_int: int = idl.case(0)
            my_str: str = idl.case(1)

    with pytest.raises(TypeError):
        @idl.union
        class NoCasesUnion:
            discriminator: idl.int16 = 0
            value: Union[int, str, Point] = 0

    with pytest.raises(TypeError):
        @idl.union
        class MultipleDefaults:
            discriminator: idl.int16 = 2
            value: Union[int, str, Point] = field(default_factory=Point)

            my_int: int = idl.case(0, is_default=True)
            my_str: str = idl.case(1)
            my_point: Point = idl.case(2, is_default=True)

    with pytest.raises(TypeError):
        @idl.union
        class CaseWithNoLabel:
            discriminator: idl.int16 = 0
            value: Union[int, str] = 0

            my_int: int = idl.case()
            my_str: str = idl.case(1)


def test_creating_union_with_bad_field_fails():
    with pytest.raises(TypeError) as excinfo:
        MyUnion(nonexistent=3)

    assert "__init__() got an unexpected keyword argument 'nonexistent'" in str(excinfo.value)


def test_union_deepcopy():
    u = MyUnion(my_str="hello")
    u_copy = copy.deepcopy(u)
    assert u_copy == u
    assert u_copy is not u

    u = MyUnion(my_point=Point(3, 4))
    u_copy = copy.deepcopy(u)
    assert u_copy == u
    assert u_copy is not u

    assert u_copy.discriminator == u.discriminator
    assert u_copy.value == u.value
    assert u_copy.value is not u.value
    assert u_copy.my_point == u.my_point
    assert u_copy.my_point is not u.my_point

    s = create_type_with_unions_sample(1)
    s_copy = copy.deepcopy(s)
    assert s_copy == s
    assert s_copy is not s
    assert s_copy.my_union is not s.my_union
    assert s_copy.my_union_opt is not s.my_union_opt
    assert s_copy.my_union_seq is not s.my_union_seq

