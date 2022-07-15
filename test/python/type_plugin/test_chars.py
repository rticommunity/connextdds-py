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

from typing import Optional, Sequence
from dataclasses import field

import rti.connextdds as dds
import rti.idl as idl

import pytest
from test_utils.fixtures import *

@idl.struct(
    member_annotations={
        "b_char_seq": [idl.bound(2)],
    }
)
class CharTest:
    my_char: idl.char = 0
    my_wchar: idl.wchar = 0
    opt_char: Optional[idl.char] = None
    opt_wchar: Optional[idl.wchar] = None
    unb_char_seq: Sequence[idl.char] = field(default_factory=list)
    b_char_seq: Sequence[idl.char] = field(
        default_factory=idl.array_factory(idl.char))
    wchar_seq: Sequence[idl.wchar] = field(
        default_factory=idl.array_factory(idl.wchar))


@pytest.fixture
def char_sample():
    return CharTest(
        my_char=idl.to_char("A"),
        my_wchar=idl.to_wchar("ñ"),
        opt_char=idl.to_char("B"),
        opt_wchar=idl.to_wchar("頃"),
        unb_char_seq=[idl.to_char(x) for x in ["C", "D", "E"]],
        b_char_seq=dds.CharSeq(["F", "G"]),
        wchar_seq=dds.Int16Seq(idl.to_array(idl.wchar, ["倍"] * 10)))

def test_char_plugin():
    ts = idl.get_type_support(CharTest)
    assert ts.type is CharTest

    dt = ts.dynamic_type
    assert dt.name == "CharTest"

    assert dt["my_char"].type == dds.CharType()
    assert dt["opt_char"].type == dds.CharType()
    assert dt["unb_char_seq"].type == dds.SequenceType(dds.CharType())

    # TODO PY-17: This is returning int16 even though type TypeCode is correct
    # assert dt["my_wchar"].type == dds.WcharType()
    # assert dt["opt_wchar"].type == dds.WcharType()

    assert dt["opt_char"].optional


def test_char_serialization(char_sample):
    ts = idl.get_type_support(CharTest)
    deserialized_sample = ts.deserialize(ts.serialize(char_sample))
    assert char_sample == deserialized_sample

    assert idl.from_char(deserialized_sample.my_char) == "A"
    assert idl.from_wchar(deserialized_sample.my_wchar) == "ñ"
    assert idl.from_wchar(deserialized_sample.opt_wchar) == "頃"
    assert idl.from_char(deserialized_sample.opt_char) == "B"
    assert idl.from_char(deserialized_sample.unb_char_seq[0]) == "C"
    assert idl.from_char(deserialized_sample.b_char_seq[0]) == "F"
    assert idl.from_wchar(deserialized_sample.wchar_seq[0]) == "倍"


def test_char_default_serialization():
    ts = idl.get_type_support(CharTest)
    deserialized_sample = ts.deserialize(ts.serialize(CharTest()))
    assert deserialized_sample.my_char == 0
    assert deserialized_sample.opt_char is None
    assert deserialized_sample.unb_char_seq == []


def test_char_pubsub(shared_participant, char_sample):
    fixture = PubSubFixture(shared_participant, CharTest, reader_policies=[
                            dds.ResourceLimits(1, 1, 1)])
    fixture.send_and_check(char_sample)
    fixture.send_and_check(CharTest())
    fixture.send_and_check(char_sample)
