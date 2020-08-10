import rti.connextdds as dds
import utils
import pytest
import pathlib
import array

PRIMITIVES = dds.StructType("Primitives")
PRIMITIVES.add_member(dds.Member("myLong", dds.Int32Type()))
PRIMITIVES.add_member(dds.Member("myDouble", dds.Float64Type()))

COMPLEX = dds.StructType("Complex")
COMPLEX.add_member(dds.Member("myLongSeq", dds.SequenceType(dds.Int32Type(), 10)))
COMPLEX.add_member(dds.Member("myLongArray", dds.ArrayType(dds.Int32Type(), 10)))


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
    pass


def test_is_member_key():
    pass


def test_member_info():
    pass


def test_dynamic_data_info():
    pass


def test_union():
    pass
