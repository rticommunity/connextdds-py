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

    loaned_nested = sample.loan_value("a")
    loaned_nested.data["x"] = 234234
    loaned_nested.return_loan()
    assert sample["a.x"] == 234234


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
