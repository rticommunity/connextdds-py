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
from typing import Union, ClassVar

import rti.connextdds as dds
import rti.idl as idl

from common_types import Point
from test_utils.fixtures import *


@idl.union
class MyUnion:
    discriminator: idl.int16 = 0
    value: Union[int, str, Point] = 0

    my_int: ClassVar[int] = idl.case(0)
    my_str: ClassVar[str] = idl.case(1)
    my_point: ClassVar[Point] = idl.case(2, 3)


@idl.union
class UnionWithDefault:
    discriminator: idl.int16 = 2
    value: Union[int, str, Point] = field(default_factory=Point)

    my_int: ClassVar[int] = idl.case(0)
    my_str: ClassVar[str] = idl.case(1)
    my_point: ClassVar[Point] = idl.case(2, is_default=True)


@idl.union
class UnionWithDefault2:
    discriminator: idl.int16 = 2
    value: Union[int, str, Point] = field(default_factory=Point)

    my_int: ClassVar[int] = idl.case(0)
    my_str: ClassVar[str] = idl.case(1)
    my_point: ClassVar[Point] = idl.case(is_default=True)

def test_union_default_values():
    assert MyUnion.default_discriminator == 0
    assert MyUnion.default_value() == 0

    assert UnionWithDefault.default_discriminator == 2
    assert UnionWithDefault.default_value() == Point()

def test_union_plugin():
    ts = idl.get_type_support(MyUnion)
    assert ts.type is MyUnion

    dt = ts.dynamic_type
    assert dt.name == "MyUnion"
    assert len(dt.members()) == 3

    assert dt.discriminator == dds.Int16Type()
    assert dt["my_int"].type == dds.Int64Type()
    assert dt["my_int"].labels == [0]
    assert dt["my_str"].type == dds.StringType()
    assert dt["my_str"].labels == [1]
    assert dt["my_point"].type == idl.get_type_support(Point).dynamic_type
    assert dt["my_point"].labels == [2, 3]


def test_default_union_plugin():
    dt = idl.get_type_support(UnionWithDefault).dynamic_type
    assert dt["my_point"].labels == [2, dds.UnionMember.DEFAULT_LABEL]
    print(dt)
    dt = idl.get_type_support(UnionWithDefault2).dynamic_type
    assert dt["my_point"].labels == [dds.UnionMember.DEFAULT_LABEL]
    print(dt)

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


@pytest.mark.parametrize("union_type", [UnionWithDefault, UnionWithDefault2])
def test_default_union_creation(union_type):
    u = union_type()
    assert u.my_point == Point()
    assert u.discriminator == 2

    u = union_type(my_int=1)
    assert u.my_int == 1
    assert u.discriminator == 0

    u = union_type(my_str="hello")
    assert u.my_str == "hello"
    assert u.discriminator == 1

    u = union_type(my_point=Point(3, 4))
    assert u.my_point == Point(3, 4)
    assert u.discriminator == 2

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


@pytest.mark.parametrize("union_type", [UnionWithDefault, UnionWithDefault2])
def test_default_union_setter(union_type: type):
    u = union_type()
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


def test_union_repr_contains_discr_and_value_only():
    u = MyUnion(my_str="hello")
    assert repr(u) == "MyUnion(discriminator=1, value='hello')"


@pytest.mark.parametrize("union_type", [MyUnion, UnionWithDefault])
def test_none_construction(union_type):
    u = union_type(None)
    assert u.value is None


def test_union_serialization():
    union_sample = MyUnion(my_point=Point(3, 4))
    ts = idl.get_type_support(MyUnion)
    buffer = ts.serialize(union_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert union_sample == deserialized_sample


def test_union_pubsub(shared_participant):
    fixture = PubSubFixture(shared_participant, MyUnion, reader_policies=[
                            dds.ResourceLimits(1, 1, 1)])
    fixture.send_and_check(MyUnion(my_point=Point(3, 4)))
    fixture.send_and_check(MyUnion(my_str="hello"))
    fixture.send_and_check(MyUnion(my_int=3))

def test_bad_union():
    with pytest.raises(TypeError):
        @idl.union
        class NoDiscriminatorUnion:
            value: Union[int, str] = 0

            my_int: ClassVar[int] = idl.case(0)
            my_str: ClassVar[str] = idl.case(1)

    with pytest.raises(TypeError):
        @idl.union
        class NoCasesUnion:
            discriminator: idl.int16 = 0
            value: Union[int, str, Point] = 0


def test_creating_union_with_bad_field_fails():
    with pytest.raises(TypeError) as excinfo:
        MyUnion(nonexistent=3)

    assert "__init__() got an unexpected keyword argument 'nonexistent'" in str(excinfo.value)


