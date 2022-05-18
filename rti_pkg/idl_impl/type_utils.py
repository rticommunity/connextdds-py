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

from typing import Any, Sequence
import array
import rti.idl_impl.type_hints as type_hints
import rti.idl_impl.reflection_utils as reflection_utils
from rti.idl_impl.sample_interpreter import WideStringConstantsMixin, StringConstantsMixin


def to_array(idl_primitive_type: type, lst: Sequence[Any]) -> array.array:
    """Creates an array (from the array module) of the given idl type from a
       list or iterable.

       For example:
         >>> int16_array = to_array(idl.int16, [1, 2, 3])
    """

    if idl_primitive_type is type_hints.char:
        return array.array(
            reflection_utils.get_array_type(type_hints.int8),
            [to_char(x) for x in lst])
    elif idl_primitive_type is type_hints.wchar:
        return array.array(
            reflection_utils.get_array_type(type_hints.uint16),
            [to_wchar(x) for x in lst])

    return array.array(
        reflection_utils.get_array_type(idl_primitive_type),
        lst)


def to_char(str_value: str) -> int:
    """Returns an int with the UTF-8 value of the first character of the input"""
    if str_value == "":
        return 0

    return str_value.encode(StringConstantsMixin.encoding)[0]


def to_wchar(str_value: str) -> int:
    """Returns an int with the UTF-16 value of the first character of the input"""
    if str_value == "":
        return 0

    return int.from_bytes(
        str_value.encode(WideStringConstantsMixin.encoding),
        byteorder='little')


def from_char(idl_char: int) -> str:
    """Returns a single-character string for the given UTF-8 IDL char"""

    return str(bytes((idl_char,)), StringConstantsMixin.encoding)


def from_wchar(idl_wchar: int) -> str:
    """Returns a single-character string for the given UTF-16 IDL wchar"""

    return str(
        idl_wchar.to_bytes(
            length=WideStringConstantsMixin.bytes_per_char, byteorder='little'),
        WideStringConstantsMixin.encoding)
