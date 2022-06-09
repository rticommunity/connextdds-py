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
import pathlib
import array
from test_utils import log_capture

@pytest.fixture
def types():
    class TestTypes:
        def __init__(self):

            FILE = str(pathlib.Path(__file__).parent.absolute()) + \
                "/../../xml/UnionWithEnum.xml"

            self.PROVIDER = dds.QosProvider(FILE)
            self.UNION = self.PROVIDER.type("TestUnionWithEnum")
            self.ENUM_TYPE = self.PROVIDER.type("TestEnum")
            self.UNION_DEFAULT_TYPE = self.PROVIDER.type(
                "TestUnionWithDefault")

            self.PRIMITIVES = dds.StructType("Primitives")
            self.PRIMITIVES.add_member(dds.Member("myLong", dds.Int32Type()))
            self.PRIMITIVES.add_member(dds.Member("myDouble", dds.Float64Type()))

            COMPLEX = dds.StructType("Complex")
            COMPLEX.add_member(dds.Member(
                "myLongSeq", dds.SequenceType(dds.Int32Type(), 10)))
            COMPLEX.add_member(dds.Member(
                "myLongArray", dds.ArrayType(dds.Int32Type(), 10)))
            COMPLEX.add_member(dds.Member(
                "myOptional", dds.Int32Type(), is_optional=True))
            COMPLEX.add_member(dds.Member("myString", dds.StringType(100)))
            COMPLEX.add_member(dds.Member(
                "myStringSeq", dds.SequenceType(dds.StringType(10), 10)))
            COMPLEX.add_member(dds.Member("myEnum", self.ENUM_TYPE))
            COMPLEX.add_member(dds.Member(
                "myEnumSeq", dds.SequenceType(self.ENUM_TYPE, 10)))
            COMPLEX.add_member(dds.Member("myMultiDimArray",
                                          dds.ArrayType(dds.Int32Type(), [2, 2, 2])))
            self.COMPLEX = COMPLEX

            self.SIMPLE = self.PROVIDER.type("SimpleType")

        def finalize_types(self):
            # This ensures that pytest doesn't keep references that case heap
            # monitoring to report leaks
            self.PROVIDER = None
            self.PRIMITIVES = None
            self.UNION = None
            self.ENUM_TYPE = None
            self.UNION_DEFAULT_TYPE = None
            self.COMPLEX = None
            self.SIMPLE = None

    types = TestTypes()
    yield types
    types.finalize_types()

def test_clear_all_members(types):
    data = dds.DynamicData(types.PRIMITIVES)
    data["myLong"] = 3
    data["myDouble"] = 7.7
    assert data["myLong"] == 3
    assert data["myDouble"] == 7.7

    data.clear_all_members()
    assert data["myLong"] == 0
    assert data["myDouble"] == 0.0

    data = dds.DynamicData(types.COMPLEX)
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


def test_clear_member(types):
    data = dds.DynamicData(types.COMPLEX)
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


def test_is_member_key(types):
    sample = dds.DynamicData(types.SIMPLE)
    assert sample.is_member_key("key")
    assert not sample.is_member_key("value")
    assert sample.is_member_key(0)
    assert not sample.is_member_key(1)


def test_member_info(types):
    data = dds.DynamicData(types.SIMPLE)
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

    data2 = dds.DynamicData(types.COMPLEX)
    info = data2.member_info("myLongSeq")

    assert info.kind == dds.SequenceType(dds.Int32Type()).kind
    assert info.element_kind == dds.Int32Type().kind
    assert data2.member_index("myLongSeq") == 0

    with data2.loan_value("myLongArray") as loan:
        assert loan.data.member_exists(1)
        info = loan.data.member_info(1)
        assert info.kind == dds.Int32Type().kind

    data2["myEnumSeq"] = [types.ENUM_TYPE["BLUE"], types.ENUM_TYPE["RED"]]
    info = data2.member_info("myEnumSeq")
    assert info.kind == dds.TypeKind.SEQUENCE_TYPE
    
    with data2.loan_value("myEnumSeq") as loan:
        assert loan.data.member_exists(1)
        info = loan.data.member_info(1)
        assert info.kind == dds.TypeKind.ENUMERATION_TYPE
        assert loan.data[1] == types.ENUM_TYPE["RED"]
        loan.data[0] = 3
        info = loan.data.member_info(0)
        assert info.kind == dds.TypeKind.ENUMERATION_TYPE
        assert loan.data[0] == types.ENUM_TYPE["GREEN"]

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
    union_sample = dds.DynamicData(types.UNION_DEFAULT_TYPE)
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

    with log_capture.expected_exception(dds.PreconditionNotMetError):
        data2.member_info("myLongSeq[12]")

    loan.return_loan()

    with log_capture.expected_errors() as errinfo:
        assert not data2.member_exists("myLongSeq[12]")
    assert "Cannot find a member" in errinfo.logs
    assert "in type sequence" in errinfo.logs

    array = data2.loan_value("myLongArray")
    with pytest.raises(dds.InvalidArgumentError):
        array.data.member_info(11)


def test_dynamic_data_info(types):
    data = dds.DynamicData(types.SIMPLE)
    info = data.info
    assert info.member_count == 2
    assert info.is_optimized_storage


def test_dynamic_data_buffer_numpy(types):
    np = pytest.importorskip("numpy")
    data = dds.DynamicData(types.COMPLEX)
    my_array_unidim = np.array([1, 2, 3, 4, 5, 6], np.int32)
    my_array_multidim = np.reshape(np.array([1, 2, 3, 4, 5, 6], np.int32), (2, 3))
    my_array_short = np.array([1, 2, 3, 4, 5, 6], np.int16)
    
    data["myLongSeq"] = my_array_unidim
    assert data["myLongSeq[2]"] == 3

    with pytest.raises(TypeError):
        data["myLongSeq"] = my_array_multidim
    
    with pytest.raises(TypeError):
        data["myLongSeq"] = my_array_short


def test_union(types):
    test_union = dds.DynamicData(types.UNION)
    simple = dds.DynamicData(types.SIMPLE)
    simple["key"] = 10
    simple["value"] = 20
    test_union["red_green"] = simple

    assert test_union.discriminator_value == types.ENUM_TYPE["RED"]
    member_value = test_union.get_value(test_union.discriminator_value)

    assert member_value.get_value(0) == 10
    assert member_value.get_value(1) == 20

    with pytest.raises(dds.InvalidArgumentError):
        test_union.get_value("blue")

    test_union["blue"] = 123
    assert test_union.discriminator_value == types.ENUM_TYPE["BLUE"]
    assert 123 == test_union.get_value(test_union.discriminator_value)

    test_union.clear_member("blue")
    assert test_union.discriminator_value == types.ENUM_TYPE["BLUE"]
    assert 0 == test_union.get_value(test_union.discriminator_value)


def test_multidim_array(types):
    data = dds.DynamicData(types.COMPLEX)

    for i in range(2):
        for j in range(2):
            for k in range(2):
                data[f"myMultiDimArray[{i}, {j}, {k}]"] = i * j * k
                assert(data[f"myMultiDimArray[{i},{j},{k}]"] == i * j * k)

    with pytest.raises(dds.InvalidArgumentError):
        my_val = data[f"myMultiDimArray[0][0][0]"]

    info = data.member_info("myMultiDimArray")
    assert(info.element_count == 8)

    info = data.member_info("myMultiDimArray[0,0,1]")
    assert(info.index == 1)


