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

from test_utils.fixtures import *
from rti.types.builtin import String, KeyedString, Bytes, KeyedBytes
import rti.connextdds as dds

def test_builtin_string_pubsub(shared_participant):
    fixture = PubSubFixture(shared_participant, String)
    fixture.send_and_check(String("h"))
    fixture.send_and_check(String(value="a" * 100))
    fixture.send_and_check(String())


def test_builtin_keyed_string_pubsub(shared_participant):
    fixture = PubSubFixture(shared_participant, KeyedString)
    fixture.send_and_check(KeyedString(value="key1"))
    fixture.send_and_check(KeyedString())
    fixture.send_and_check(KeyedString(key="hello 2", value="foo"))
    fixture.send_and_check(KeyedString("hello 3", "a" * 100))


def test_builtin_bytes_pubsub(shared_participant):
    fixture = PubSubFixture(shared_participant, Bytes)
    fixture.send_and_check(Bytes([3] * 100))
    fixture.send_and_check(Bytes(value=[]))
    fixture.send_and_check(Bytes(dds.Uint8Seq([1, 4, 2, 3])))


def test_builtin_keyedbytes_pubsub(shared_participant):
    fixture = PubSubFixture(shared_participant, KeyedBytes)
    fixture.send_and_check(KeyedBytes(key="k2", value=[1, 7, 5] * 300))
    fixture.send_and_check(KeyedBytes("a", value=[]))
    fixture.send_and_check(KeyedBytes())
    fixture.send_and_check(KeyedBytes("b", dds.Uint8Seq([1, 4, 2, 3])))
