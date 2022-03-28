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


def test_union_serialization():

    union_sample = MyUnion()
    union_sample.my_point = Point(3,4)

    ts = idl.get_type_support(MyUnion)
    buffer = ts.serialize(union_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert union_sample == deserialized_sample

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
