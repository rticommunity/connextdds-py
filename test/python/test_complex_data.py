 #
 # (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 #
 # RTI grants Licensee a license to use, modify, compile, and create derivative
 # works of the Software solely for use with RTI products.  The Software is
 # provided "as is", with no warranty of any type, including any warranty for
 # fitness for any purpose. RTI is under no obligation to maintain or support
 # the Software.  RTI shall not be liable for any incidental or consequential
 # damages arising out of the use or inability to use the software.
 #

import rti.connextdds as dds
import pytest
import utils
import pathlib

FILE = str(pathlib.Path(__file__).parent.absolute()) + "/../xml/PerformanceTester.xml"

PROVIDER = dds.QosProvider(FILE)
PRIMITIVE_ARRAY_TYPE_4 = PROVIDER.type("PrimitiveArrayType4")
NON_PRIMITIVE_TYPE = PROVIDER.type("NonPrimitiveType")


def test_sequence():
    sample = dds.DynamicData(PRIMITIVE_ARRAY_TYPE_4)
    sample["longArray"] = list(range(0, 4))
    print(sample["longArray"])
    for i in range(0, 4):
        assert i == sample["longArray"][i]


def test_non_primitive_type():
    sample = dds.DynamicData(NON_PRIMITIVE_TYPE)
    sample["x"] = 5
    sample["y"] = 9
    assert sample["x"] == 5
    assert sample["y"] == 9


def test_nested_struct():
    # Here we create a nested type. It has two members, both are the simple non-primitive type
    nested_type = dds.StructType("nested_type")
    nested_type.add_member(dds.Member("a", NON_PRIMITIVE_TYPE))
    nested_type.add_member(dds.Member("b", NON_PRIMITIVE_TYPE))
    sample = dds.DynamicData(nested_type)

    # Fill in the values
    sample["a.x"] = 1
    sample["a.y"] = 2
    sample["b.x"] = 3
    sample["b.y"] = 4

    # Assert they are correct
    assert sample["a.x"] == 1
    assert sample["a.y"] == 2
    assert sample["b.x"] == 3
    assert sample["b.y"] == 4


def test_nested_struct_copy():
    nested_type = dds.StructType("nested_type")
    nested_type.add_member(dds.Member("a", NON_PRIMITIVE_TYPE))
    nested_type.add_member(dds.Member("b", NON_PRIMITIVE_TYPE))
    sample = dds.DynamicData(nested_type)

    # Fill in the values
    sample["a.x"] = 1
    sample["a.y"] = 2
    sample["b.x"] = 3
    sample["b.y"] = 4

    nested = sample["a"]
    x = nested["x"]
    nested["y"] = 23432
    sample["a"] = nested
    assert nested["y"] == 23432
    assert nested["x"] == x


def test_nested_struct_loan():
    nested_type = dds.StructType("nested_type")
    nested_type.add_member(dds.Member("a", NON_PRIMITIVE_TYPE))
    nested_type.add_member(dds.Member("b", NON_PRIMITIVE_TYPE))
    sample = dds.DynamicData(nested_type)

    # Fill in the values
    sample["a.x"] = 1
    sample["a.y"] = 2
    sample["b.x"] = 3
    sample["b.y"] = 4

    with sample.loan_value("a") as loaned_nested:
        loaned_nested.data["x"] = 234234
        loaned_nested.data["y"] = 234235
    assert sample["a.x"] == 234234
    assert sample["a.y"] == 234235
    with sample.loan_value("a") as loaned_nested:
        loaned_nested.data["x"] = 3
    assert sample["a.x"] == 3
    assert sample["a.y"] == 234235


@pytest.mark.parametrize("size", [1, 4, 16])
def test_squence_of_nested_structs(size):
    nested_type = dds.StructType("nested_type")
    nested_type.add_member(dds.Member("a", NON_PRIMITIVE_TYPE))
    nested_type.add_member(dds.Member("b", NON_PRIMITIVE_TYPE))

    array_of_nested_type = dds.SequenceType(nested_type, size)

    sample = dds.DynamicData(array_of_nested_type)
    # Assign values
    for i in range(0, size):
        sample[i] = dds.DynamicData(
            nested_type, ({"a": {"x": i, "y": i + 1}, "b": {"x": i + 2, "y": i + 3}})
        )

    # Assert correct values
    for i in range(0, size):
        assert sample[i] == dds.DynamicData(
            nested_type, {"a": {"x": i, "y": i + 1}, "b": {"x": i + 2, "y": i + 3}}
        )

    # Assign values via a loan
    for i in range(0, size):
        loaned = sample.loan_value(i)
        loaned.data["a.x"] = -1 * i
        loaned.return_loan()

    # Assert correct values
    for i in range(0, size):
        assert sample[i] == dds.DynamicData(
            nested_type,
            {"a": {"x": (-1 * i), "y": i + 1}, "b": {"x": i + 2, "y": i + 3}},
        )

# Tests code examples used in types.rst
def test_documentation_examples():
    # struct Point {
    #     float x, y;
    # };
    point_type = dds.StructType("Point")
    point_type.add_member(dds.Member("x", dds.Float64Type()))
    point_type.add_member(dds.Member("y", dds.Float64Type()))

    # struct MyType {
    #     @key string<128> id;
    #     Point location;
    #     int32 int_array[5];
    #     sequence<Point, 10> path;
    # };
    my_type = dds.StructType("MyType")
    my_type.add_member(dds.Member(name="id", data_type=dds.StringType(128), is_key=True))
    my_type.add_member(dds.Member(name="location", data_type=point_type))
    my_type.add_member(dds.Member(name="int_array", data_type=dds.ArrayType(dds.Int32Type(), 5)))
    my_type.add_member(dds.Member(name="path", data_type=dds.SequenceType(point_type, 10)))

    sample = dds.DynamicData(my_type)
    sample["id"] = "object1"
    sample["location.x"] = 34.02
    sample["location.y"] = 13.65

    assert "object1" == sample["id"]
    assert 34.02 == sample["location.x"]
    assert 13.65 == sample["location.y"]

    with sample.loan_value("location") as location:
        location.data["x"] = 11
        location.data["y"] = 12

    assert 11 == sample["location.x"]
    assert 12 == sample["location.y"]

    sample["location"] = {"x": 4, "y": 5}

    assert 4 == sample["location.x"]
    assert 5 == sample["location.y"]
    assert {"x": 4, "y": 5} == sample["location"]

    sample["int_array"] = [11] * 5
    assert [11] * 5 == list(sample["int_array"])

    sample["path"] = [{"x": 1, "y": 2}, {"x": 3, "y": 4}]
    path = list(sample["path"])
    assert 4 == path[1]["y"]

    with sample.loan_value("path") as path:
        with path.data.loan_value(2) as point:
            point.data["x"] = 111
            point.data["y"] = 222
    print(sample["path[2].x"]) # prints 111

    with pytest.raises(dds.PreconditionNotMetError) as excinfo:
        sample["bad_member"] = "bad value"
    with pytest.raises(ValueError) as excinfo:
        sample["location"] = "bad value"

test_sequence()