import rti.connextdds as dds
import utils
import pytest
import pathlib
import array


FILE = str(pathlib.Path(__file__).parent.absolute()) + "/../xml/UnionWithEnum.xml"

PROVIDER = dds.QosProvider(FILE)
UNION = PROVIDER.type("TestUnionWithEnum")
ENUM_TYPE = PROVIDER.type("TestEnum")


PRIMITIVES = dds.StructType("Primitives")
PRIMITIVES.add_member(dds.Member("myLong", dds.Int32Type()))
PRIMITIVES.add_member(dds.Member("myDouble", dds.Float64Type()))

COMPLEX = dds.StructType("Complex")
COMPLEX.add_member(dds.Member("myLongSeq", dds.SequenceType(dds.Int32Type(), 10)))
COMPLEX.add_member(dds.Member("myLongArray", dds.ArrayType(dds.Int32Type(), 10)))
COMPLEX.add_member(dds.Member("myOptional", dds.Int32Type(), is_optional=True))
COMPLEX.add_member(dds.Member("myString", dds.StringType(100)))

SIMPLE = PROVIDER.type("SimpleType")


def test_clear_all_members():
    data = dds.DynamicData(PRIMITIVES)
    data["myLong"] = 3
    data["myDouble"] = 7.7
    assert data["myLong"] == 3
    assert data["myDouble"] == 7.7

    data.clear_all_members()
    assert data["myLong"] == 0
    assert data["myDouble"] == 0.0

    data = dds.DynamicData(COMPLEX)
    data["myLongSeq"] = list(range(1, 11))
    data["myLongArray"] = array.array("i", range(1, 11))

    for i in range(0, 10):
        assert data[f"myLongSeq[{i}]"] == i + 1
        assert data[f"myLongArray[{i}]"] == i + 1

    member = data.loan_value("myLongSeq")
    member.data.clear_all_members()
    member.return_loan()
    member = data.loan_value("myLongArray")
    member.data.clear_all_members()
    member.return_loan()

    print(data["myLongSeq"])
    assert data["myLongSeq"].member_count == 0
    for i in range(0, 10):
        assert data[f"myLongArray[{i}]"] == 0


def test_clear_member():
    data = dds.DynamicData(COMPLEX)
    data["myOptional"] = 3
    assert data["myOptional"] == 3
    data.clear_optional_member("myOptional")
    assert not data.member_exists("myOptional")

    data["myString"] = "aStringValue"
    assert data["myString"] == "aStringValue"
    data.clear_member(3)
    assert data["myString"] == ""

    data["myLongSeq"] = list(range(1, 11))
    assert data.get_values(0) == dds.Int32Seq(list(range(1, 11)))
    data.clear_member("myLongSeq")
    assert data["myLongSeq"].member_count == 0

    data["myLongSeq"] = list(range(1, 11))
    assert data.get_values(0) == dds.Int32Seq(list(range(1, 11)))

    seq_member = data.loan_value("myLongSeq")
    seq_member.data.clear_member(3)
    seq_member.return_loan()

    values = data.get_values(0)
    for i in range(0, 10):
        if i == 3:
            assert 0 == values[i]
        else:
            assert i + 1 == values[i]


def test_is_member_key():
    sample = dds.DynamicData(SIMPLE)
    assert sample.is_member_key("key")
    assert not sample.is_member_key("value")
    assert sample.is_member_key(0)
    assert not sample.is_member_key(1)


def test_member_info():
    data = dds.DynamicData(SIMPLE)
    info1 = data.member_info("key")
    info2 = data.member_info(0)
    assert info1 == info2

    assert info1.member_index == 0
    assert info1.member_name == "key"
    assert info1.element_count == 0
    assert info1.representation_count == 0

    name = info2.member_name
    assert name == "key"

    assert data.member_index("key") == 1

    # test_value_type?

    data2 = dds.DynamicData(COMPLEX)
    info = data2.member_info("myLongSeq")

    assert info.member_kind == dds.SequenceType()
    assert info.element_kind == dds.Int32Type()
    assert data2.member_index("myLongSeq") == 5

    info = data2.loan_value("myLongArray").data.member_info(1)
    assert info.member_exists
    assert info.member_kind == dds.Int32Type()

    # Cases where member doesn't exist in sample
    # but does in the type

    # 1) Unset Optional
    info = data2.member_info("myOptional")
    assert info.member_kind == dds.Int32Type()
    assert not info.member_exists
    assert not data2.member_exists("myOptional")
    data2["myOptional"] = 33
    info = data2.member_info("myOptional")
    assert info.member_exists
    assert data2.member_exists("myOptional")

    # 2) Unselected union member

    # 3) Sequence element i such that length <= i < max_length


def test_dynamic_data_info():
    data = dds.DynamicData(SIMPLE)
    info = data.info
    assert info.member_count == 2
    assert info.is_optimized_storage


def test_union():
    test_union = dds.DynamicData(UNION)
    simple = dds.DynamicData(SIMPLE)
    simple["key"] = 10
    simple["value"] = 20
    test_union["red_green"] = simple

    assert test_union.discriminator_value == int(ENUM_TYPE["RED"].ordinal)
    member_value = test_union.get_value(test_union.discriminator_value)

    assert member_value.get_value(0) == 10
    assert member_value.get_value(1) == 20

    try:
        test_union.get_value("blue")
        print("Failded to throw exception")
        assert False
    except dds.InvalidArgumentError:
        assert True
    except Exception as e:
        print("Failed to throw proper exception")
        print("Expected InvalidArgumentError, got " + str(type(e)))
        assert False

    test_union["blue"] = 15
    assert test_union.discriminator_value == int(ENUM_TYPE["BLUE"].ordinal)
    assert 15 == test_union.get_value(test_union.discriminator_value)

    test_union.clear_member("blue")
    assert test_union.discriminator_value == int(ENUM_TYPE["BLUE"].ordinal)
    assert 0 == test_union.get_value(test_union.discriminator_value)
