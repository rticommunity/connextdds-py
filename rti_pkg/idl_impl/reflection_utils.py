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

import sys
import typing
import collections.abc as collections
from rti.idl_impl.type_hints import *

SUPPORT_GET_ORIGIN = sys.version_info >= (3, 8)

def _get_origin_workaround(t):
    return t.__origin__ if hasattr(t, '__origin__') else None

def _get_args_workaround(t):
    return t.__args__ if hasattr(t, '__args__') else None

get_origin = typing.get_origin if SUPPORT_GET_ORIGIN else _get_origin_workaround
get_args = typing.get_args if SUPPORT_GET_ORIGIN else _get_args_workaround

def is_optional_type(t):
    return get_origin(t) is typing.Union

def is_sequence_type(t):
    origin = get_origin(t)
    return origin in (typing.Sequence, collections.Sequence, typing.List, list)

def is_constructed_type(t):
    return hasattr(t, 'type_support')

def get_underlying_type(t):
    args = get_args(t)
    return args[0] if args is not None else None


_PRIMITIVE_TYPES = (bool, int8, int16, uint16, int32,
                    uint32, int64, uint64, float32, float64)

def is_primitive(t: type):
    """Returns True if the type is a primitive type"""
    return t in _PRIMITIVE_TYPES

_TYPE_TO_ARRAY_TYPE_MAP: typing.Dict[type, str] = {
    int8: 'b',
    int16: 'h',
    uint16: 'H',
    int32: 'i',
    uint32: 'I',
    int64: 'q',
    uint64: 'Q',
    float32: 'f',
    float64: 'd',
}

def get_array_type(t: type) -> str:
    """Translates from an IDL primitive type to the string used by Python's
    array.array
    """
    return _TYPE_TO_ARRAY_TYPE_MAP.get(t)
