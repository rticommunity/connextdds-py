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

import pytest
import ctypes

from dataclasses import dataclass
import rti.idl_impl.csequence as cseq
import rti.connextdds.core_utils as core_utils

@dataclass
class Point:
    x: int = 0
    y: int = 0

class PointNative(ctypes.Structure):
    _fields_ = [
        ("x", ctypes.c_int32),
        ("y", ctypes.c_byte)
    ]

PointNativeSeq = cseq.create_sequence_type(PointNative)

def initialize_sequence(seq: cseq.Sequence, length: int) -> None:
    seq._contiguous_buffer = core_utils.malloc(length * seq._element_size)
    seq._length = length
    seq._maximum = length

def finalize_sequence(seq: cseq.Sequence) -> None:
    core_utils.free(seq._contiguous_buffer)
    seq._contiguous_buffer = None


@pytest.fixture(scope="module")
def point_seq():
    point_seq = PointNativeSeq()
    initialize_sequence(point_seq, 3)
    yield point_seq
    finalize_sequence(point_seq)


def test_sequence_basic(point_seq):
    assert type(point_seq)._element_size == ctypes.sizeof(PointNative)
    assert point_seq._element_type == PointNative
    assert len(point_seq) == 3

def test_sequence_items(point_seq):
    point_seq[0].x, point_seq[0].y = 2, 3
    point_seq[1].x, point_seq[1].y = 4, 5
    point_seq[2].x, point_seq[2].y = 6, 7

    assert (point_seq[0].x, point_seq[0].y) == (2, 3)
    assert (point_seq[1].x, point_seq[1].y) == (4, 5)
    assert (point_seq[2].x, point_seq[2].y) == (6, 7)

    point_seq[1].x, point_seq[1].y = 40, 50
    assert (point_seq[0].x, point_seq[0].y) == (2, 3)
    assert (point_seq[1].x, point_seq[1].y) == (40, 50)
    assert (point_seq[2].x, point_seq[2].y) == (6, 7)


def test_sequence_iter(point_seq):
    point_seq[0].x, point_seq[0].y = 2, 3
    point_seq[1].x, point_seq[1].y = 4, 5
    point_seq[2].x, point_seq[2].y = 6, 7

    assert [(p.x, p.y) for p in point_seq] == [(2, 3), (4, 5), (6, 7)
]


