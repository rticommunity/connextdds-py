import rti.connextdds as dds
import utils
import pytest
import pathlib
import array


FILE = str(pathlib.Path(__file__).parent.absolute()) + "/../xml/UnionWithEnum.xml"

PROVIDER = dds.QosProvider(FILE)
UNION = PROVIDER.type("TestUnionWithEnum")
ENUM_TYPE = PROVIDER.type("TestEnum")
UNION_DEFAULT_TYPE = PROVIDER.type("TestUnionWithDefault")

PRIMITIVES = dds.StructType("Primitives")
PRIMITIVES.add_member(dds.Member("myLong", dds.Int32Type()))
PRIMITIVES.add_member(dds.Member("myDouble", dds.Float64Type()))

COMPLEX = dds.StructType("Complex")
COMPLEX.add_member(dds.Member("myLongSeq", dds.SequenceType(dds.Int32Type(), 10)))
COMPLEX.add_member(dds.Member("myLongArray", dds.ArrayType(dds.Int32Type(), 10)))
COMPLEX.add_member(dds.Member("myOptional", dds.Int32Type(), is_optional=True))
COMPLEX.add_member(dds.Member("myString", dds.StringType(100)))
COMPLEX.add_member(dds.Member("myStringSeq", dds.SequenceType(dds.StringType(10), 10)))
COMPLEX.add_member(dds.Member("myEnum", ENUM_TYPE))
COMPLEX.add_member(dds.Member("myEnumSeq", dds.SequenceType(ENUM_TYPE, 10)))

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

    assert len(data["myLongSeq"]) == 0
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

    data["myStringSeq"] = ["one", "two", "three", "four"]
    assert data["myStringSeq[1]"] == "two"
    data.clear_member(4)
    assert len(data["myStringSeq"]) == 0

    data["myLongSeq"] = list(range(1, 11))
    assert data.get_values(0) == dds.Int32Seq(list(range(1, 11)))
    data.clear_member("myLongSeq")
    assert len(data["myLongSeq"]) == 0

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

    assert info1.index == 0
    assert info1.name == "key"
    assert info1.element_count == 0

    # This function doesn't exist
    # assert info1.representation_count == 0

    name = info2.name
    assert name == "key"

    # Need it to be 0 since indexing is different
    assert data.member_index("key") == 0

    # test_value_type?

    data2 = dds.DynamicData(COMPLEX)
    info = data2.member_info("myLongSeq")

    assert info.kind == dds.SequenceType(dds.Int32Type()).kind
    assert info.element_kind == dds.Int32Type().kind
    assert data2.member_index("myLongSeq") == 0

    with data2.loan_value("myLongArray") as loan:
        assert loan.data.member_exists(1)
        info = loan.data.member_info(1)
        assert info.kind == dds.Int32Type().kind

    data2["myEnumSeq"] = [ENUM_TYPE["BLUE"], ENUM_TYPE["RED"]]
    info = data2.member_info("myEnumSeq")
    assert info.kind == dds.TypeKind.SEQUENCE_TYPE
    
    with data2.loan_value("myEnumSeq") as loan:
        assert loan.data.member_exists(1)
        info = loan.data.member_info(1)
        assert info.kind == dds.TypeKind.ENUMERATION_TYPE
        assert loan.data[1] == ENUM_TYPE["RED"]
        loan.data[0] = 3
        info = loan.data.member_info(0)
        assert info.kind == dds.TypeKind.ENUMERATION_TYPE
        assert loan.data[0] == ENUM_TYPE["GREEN"]

    # Cases where member doesn't exist in sample
    # but does in the type

    # 1) Unset Optional
    assert not data2.member_exists("myOptional")
    assert data2.member_exists_in_type("myOptional")
    data2["myOptional"] = 33
    assert data2.member_exists("myOptional")
    info = data2.member_info("myOptional")
    assert info.kind == dds.Int32Type().kind

    # 2) Unselected union member
    union_sample = dds.DynamicData(UNION_DEFAULT_TYPE)
    assert not union_sample.member_exists("case1")
    info = union_sample.member_info("case_default")
    assert info.kind == dds.TypeKind.ENUMERATION_TYPE
    assert union_sample.member_exists("case_default")
    assert info == union_sample.member_info(0)

    union_sample["case1.key"] = 234
    assert union_sample.member_exists("case1")
    info = union_sample.member_info("case1")
    assert info.kind == dds.TypeKind.STRUCTURE_TYPE
    assert info == union_sample.member_info(1)
    assert not union_sample.member_exists("case_default")

    # 3) Sequence element i such that length <= i < max_length
    loan = data2.loan_value("myLongSeq")
    assert not loan.data.member_exists(1)

    loan.data[:] = [3] * 3
    assert loan.data.member_exists(1)

    with pytest.raises(dds.InvalidArgumentError):
        data2.member_info("nonexistent")

    with pytest.raises(dds.InvalidArgumentError):
        loan.data.member_info(12)

    with pytest.raises(dds.PreconditionNotMetError):
        data2.member_info("myLongSeq[12]")

    loan.return_loan()

    assert not data2.member_exists("myLongSeq[12]")
    array = data2.loan_value("myLongArray")

    with pytest.raises(dds.InvalidArgumentError):
        array.data.member_info(11)


def test_dynamic_data_info():
    data = dds.DynamicData(SIMPLE)
    info = data.info
    assert info.member_count == 2
    assert info.is_optimized_storage


def test_dynamic_data_buffer_numpy():
    np = pytest.importorskip("numpy")
    data = dds.DynamicData(COMPLEX)
    my_array_unidim = np.array([1, 2, 3, 4, 5, 6], np.int32)
    my_array_multidim = np.reshape(np.array([1, 2, 3, 4, 5, 6], np.int32), (2, 3))
    my_array_short = np.array([1, 2, 3, 4, 5, 6], np.int16)
    
    data["myLongSeq"] = my_array_unidim
    assert data["myLongSeq[2]"] == 3

    with pytest.raises(TypeError):
        data["myLongSeq"] = my_array_multidim
    
    with pytest.raises(TypeError):
        data["myLongSeq"] = my_array_short


def test_union():
    test_union = dds.DynamicData(UNION)
    simple = dds.DynamicData(SIMPLE)
    simple["key"] = 10
    simple["value"] = 20
    test_union["red_green"] = simple

    assert test_union.discriminator_value == ENUM_TYPE["RED"]
    member_value = test_union.get_value(test_union.discriminator_value)

    assert member_value.get_value(0) == 10
    assert member_value.get_value(1) == 20

    with pytest.raises(dds.InvalidArgumentError):
        test_union.get_value("blue")

    test_union["blue"] = 123
    assert test_union.discriminator_value == ENUM_TYPE["BLUE"]
    assert 123 == test_union.get_value(test_union.discriminator_value)

    test_union.clear_member("blue")
    assert test_union.discriminator_value == ENUM_TYPE["BLUE"]
    assert 0 == test_union.get_value(test_union.discriminator_value)
