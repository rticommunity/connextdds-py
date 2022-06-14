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

from dataclasses import dataclass
from typing import Any, Sequence, Union, ClassVar, List
import array
import itertools
import rti.idl_impl.type_hints as type_hints
import rti.idl_impl.annotations as annotations
import rti.idl_impl.reflection_utils as reflection_utils

@dataclass
class PrimitiveArrayFactory:
    """A callable that creates an array for the default_factory of a dataclass
    field
    """

    element_type: type
    size: int
    supports_buffer_protocol: ClassVar[bool] = True

    def __post_init__(self):
        self.element_type_str = reflection_utils.get_array_type(
            self.element_type)

        if self.element_type_str is None:
            raise TypeError(
                f"'{self.element_type}' is not a valid primitive element type for an array")

    def __call__(self):
        if self.size == 0:
            return array.array(self.element_type_str)
        else:
            return array.array(self.element_type_str, itertools.repeat(0, self.size))


@dataclass
class ListFactory:
    """A callable that creates a list of a fixed size for the default_factory
    of a dataclass field
    """

    element_type: type
    size: int
    supports_buffer_protocol: ClassVar[bool] = False

    def __call__(self):
        return [self.element_type() for _ in range(self.size)]


@dataclass
class ValueListFactory:
    """A optimization of ListFactory for types types with immutable values"""

    element_value: Any
    size: int
    supports_buffer_protocol: ClassVar[bool] = False

    def __call__(self):
        # Optimization over ListFactory; strings are immutable so we don't need
        # a new object for each element
        return [self.element_value] * self.size

def array_factory(element_type: type, size: Union[int, List[int]] = 0):
    return PrimitiveArrayFactory(
        element_type,
        annotations.get_total_size_from_dimensions(size))


def list_factory(element_type_or_value: Any, size: Union[int, List[int]]):
    if type(element_type_or_value) is type:
        # If we pass a type, the factory instantiates new objects of that type
        return ListFactory(
            element_type=element_type_or_value,
            size=annotations.get_total_size_from_dimensions(size))
    else:
        # If we pass a value, the factory returns a list with the same value
        return ValueListFactory(
            element_value=element_type_or_value,
            size=annotations.get_total_size_from_dimensions(size))


# Gets the most efficient factory for a list or an array
def get_optimal_collection_factory(element_type: type, size: Union[int, List[int]]):
    if reflection_utils.is_primitive_or_enum(element_type):
        # For primitives, use a factory of compact Python arrays
        return array_factory(element_type, size)
    elif size != 0:
        # For IDL arrays of non-primitives, use a factory of lists of fixed size
        return list_factory(element_type, size)
    else:
        # For IDL sequences of non-primitives, use the list default ctor
        return list


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


WSTRING_ENCODING = 'utf-16-le'
WSTRING_BYTES_PER_CHAR = 2
STRING_ENCODING = 'utf-8'

def to_char(str_value: str) -> int:
    """Returns an int with the UTF-8 value of the first character of the input"""
    if str_value == "":
        return 0

    return str_value.encode(STRING_ENCODING)[0]


def to_wchar(str_value: str) -> int:
    """Returns an int with the UTF-16 value of the first character of the input"""
    if str_value == "":
        return 0

    return int.from_bytes(
        str_value.encode(WSTRING_ENCODING),
        byteorder='little')


def from_char(idl_char: int) -> str:
    """Returns a single-character string for the given UTF-8 IDL char"""

    return str(bytes((idl_char,)), STRING_ENCODING)


def from_wchar(idl_wchar: int) -> str:
    """Returns a single-character string for the given UTF-16 IDL wchar"""

    return str(
        idl_wchar.to_bytes(length=WSTRING_BYTES_PER_CHAR, byteorder='little'),
        WSTRING_ENCODING)
