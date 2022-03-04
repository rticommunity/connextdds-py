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

from typing import Optional

import rti.connextdds as dds
import rti.idl as idl

import pytest
from test_utils.fixtures import *


@idl.struct(
    member_annotations={
        'bounded_str': [idl.bound(7)],
        'bounded_wstr': [idl.bound(8), idl.utf16],
        'unbounded_wstr': [idl.utf16],
        'opt_bounded_str': [idl.bound(7)],
        'opt_bounded_wstr': [idl.bound(8), idl.utf16],
        'opt_unbounded_wstr': [idl.utf16]
    }
)
class StringTest:
    bounded_str: str = ""
    unbounded_str: str = ""
    bounded_wstr: str = ""
    unbounded_wstr: str = ""
    opt_bounded_str: Optional[str] = None
    opt_unbounded_str: Optional[str] = None
    opt_bounded_wstr: Optional[str] = None
    opt_unbounded_wstr: Optional[str] = None


@pytest.fixture
def str_sample():
    return StringTest(
        "hello 1",
        "hello world 2",
        "hello 3",
        "hello world 4",
        "hello 1",
        "hello world 2",
        "hello 3",
        "hello world 4")


def test_string_plugin():
    ts = idl.get_type_support(StringTest)
    assert ts.type is StringTest

    dt = ts.dynamic_type
    assert dt.name == "StringTest"
    assert len(dt.members()) == 8

    assert dt["bounded_str"].type == dds.StringType(7)
    assert dt["unbounded_str"].type == dds.StringType()
    assert dt["bounded_wstr"].type == dds.WStringType(8)
    assert dt["unbounded_wstr"].type == dds.WStringType()

    assert dt["opt_bounded_str"].type == dds.StringType(7)
    assert dt["opt_unbounded_str"].type == dds.StringType()
    assert dt["opt_bounded_wstr"].type == dds.WStringType(8)
    assert dt["opt_unbounded_wstr"].type == dds.WStringType()

    assert dt["opt_bounded_str"].optional
    assert dt["opt_unbounded_str"].optional
    assert dt["opt_bounded_wstr"].optional
    assert dt["opt_unbounded_wstr"].optional


def test_string_serialization(str_sample):
    ts = idl.get_type_support(StringTest)
    buffer = ts.serialize(str_sample)
    deserialized_sample = ts.deserialize(buffer)
    assert str_sample == deserialized_sample


def test_string_pubsub(shared_participant, str_sample):
    fixture = PubSubFixture(shared_participant, StringTest)
    fixture.writer.write(str_sample)
    wait.for_data(fixture.reader)
    fixture.writer.write(StringTest())
    wait.for_data(fixture.reader)
    fixture.writer.write(str_sample)
    wait.for_data(fixture.reader)
    assert fixture.reader.take_data() == [str_sample, StringTest(), str_sample]


def test_string_serialization_fails_when_out_of_bounds():
    ts = idl.get_type_support(StringTest)

    ts.serialize(StringTest(bounded_str="a" * 7))
    with pytest.raises(Exception) as ex:
        ts.serialize(StringTest(bounded_str="b" * 8))
    assert "Error processing field 'bounded_str'" in str(ex.value)

    ts.serialize(StringTest(bounded_wstr="a" * 8))
    with pytest.raises(Exception) as ex:
        ts.serialize(StringTest(bounded_wstr="b" * 9))
    assert "Error processing field 'bounded_wstr'" in str(ex.value)

    ts.serialize(StringTest(opt_bounded_str="a" * 7))
    with pytest.raises(Exception) as ex:
        ts.serialize(StringTest(opt_bounded_str="b" * 8))
    assert "Error processing field 'opt_bounded_str'" in str(ex.value)

    ts.serialize(StringTest(opt_bounded_wstr="a" * 8))
    with pytest.raises(Exception) as ex:
        ts.serialize(StringTest(opt_bounded_wstr="b" * 9))
    assert "Error processing field 'opt_bounded_wstr'" in str(ex.value)
