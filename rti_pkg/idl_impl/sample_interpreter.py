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

"""This module provides generic functions to convert IDL-based Python types to
 C types (ctypes) and vice-versa.

 There are two phases in this process:
   - Compilation phase: the types are inspected to generate a "program" that
     provides the steps to perform these convertions
   - Execution phase: the compiled program is executed to convert each object.
"""

from typing import Any, List, Dict, Tuple, Sequence, Callable, Optional
from dataclasses import dataclass, fields, MISSING
import itertools
import abc

import rti.connextdds as dds
import rti.idl_impl.reflection_utils as reflection_utils
import rti.idl_impl.annotations as annotations
import rti.connextdds.core_utils as core_utils
import rti.idl_impl.type_utils as type_utils
from rti.idl_impl.unions import union_cases, union_discriminator, DEFAULT_LABEL


class FieldSerializationError(RuntimeError):
    """Raised when a serialization error occurs serializing a struct or union
    member
    """

    def __init__(self, field_name: str) -> None:
        super().__init__(f"Error processing field '{field_name}'")
        self.field_name = field_name

def as_int_ptr(c_ptr):
    """Casts a ctypes pointer into an int that can be passed to c++"""
    return int(c_ptr) if c_ptr is not None else 0

class Instruction(metaclass=abc.ABCMeta):
    """Base class for instructions that can be executed by the interpreter"""

    def __init__(
        self,
        field_name: str,
        is_primitive: bool = False,
        field_index: int = -1,
        field_factory: Any = None,
        is_optional: bool = False
    ) -> None:
        self.field_name = field_name
        self.is_primitive = is_primitive and not is_optional
        self.field_index = field_index
        self.field_factory = field_factory
        if is_optional:
            self.is_optional = True
            self.get_c_attr = self._get_optional
            self.set_c_attr = self._set_optional
        else:
            self.is_optional = False
            self.get_c_attr = getattr
            self.set_c_attr = setattr

    @abc.abstractmethod
    def execute(self, dst: Any, src: Any) -> None:
        """Executes the instruction, copying src int dst"""

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}({self.field_name}, is_optional={self.is_optional})"

    def _get_optional(self, csample: Any, field_name: str) -> Any:
        value = getattr(csample, field_name)
        return value[0]

    def _set_optional(self, csample: Any, field_name: str, value: Any) -> None:
        getattr(csample, field_name)[0] = value


class CopyPrimitiveToPyInstruction(Instruction):
    """Instruction to copy a primitive type from one object to another."""

    def __init__(self, field_name: str, is_optional: bool = False, field_index: int = -1):
        super().__init__(field_name, is_primitive=True, is_optional=is_optional, field_index=field_index)

    def execute(self, dst: Any, src: Any) -> None:
        setattr(dst, self.field_name, self.get_c_attr(src, self.field_name))


class CopyPrimitiveToCInstruction(Instruction):
    """Instruction to copy a primitive type from one object to another."""

    def __init__(self, field_name: str, is_optional: bool = False, field_index: int = -1):
        super().__init__(field_name, is_primitive=True,
                         is_optional=is_optional, field_index=field_index)

    def execute(self, dst: Any, src: Any) -> None:
        self.set_c_attr(dst, self.field_name, getattr(src, self.field_name))



# --- Enums  ------------------------------------------------------------------

class CopyIntEnumToInt32Instruction(Instruction):
    """Python to C: copy an enum into an int32 member"""

    def execute(self, dst: Any, src: Any) -> None:
        self.set_c_attr(dst, self.field_name, getattr(src, self.field_name))


class CopyInt32ToIntEnumInstruction(Instruction):
    """C to Python: copy an int32 into an enum member"""

    def __init__(self, field_name: str, enum_type: type, is_optional: bool):
        super().__init__(field_name, is_optional=is_optional)
        self.enum_type = enum_type

    def execute(self, dst: Any, src: Any) -> None:
        int_value = self.get_c_attr(src, self.field_name)
        try:
            enum_value = self.enum_type(int_value)
        except:
            # Allow unknown enumerators as integers. Note that a DataReader may
            # forbid unknown enumerators; in that case the sample is dropped in
            # before getting here.
            enum_value = int_value

        setattr(dst, self.field_name, enum_value)

# --- Structs  ----------------------------------------------------------------


class CopyAggregationToPyInstruction(Instruction):
    """An instruction to recursively copy an aggregation type from one object
    to another.
    """

    def __init__(self, field_name: str, is_optional: bool, field_factory: Any, sample_program: 'SampleProgram'):
        super().__init__(field_name, is_optional=is_optional, field_factory=field_factory)
        self.sample_program = sample_program

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        dst_member = getattr(dst, field_name)
        src_member = self.get_c_attr(src, field_name)
        self.sample_program.execute(dst=dst_member, src=src_member)


class CopyAggregationToCInstruction(Instruction):
    """An instruction to recursively copy an aggregation type from one object
    to another.
    """

    def __init__(self, field_name: str, field_index: int, is_optional: bool, sample_program: 'SampleProgram'):
        super().__init__(field_name, field_index=field_index, is_optional=is_optional)
        self.sample_program = sample_program

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        dst_member = self.get_c_attr(dst, field_name)
        src_member = getattr(src, field_name)
        self.sample_program.execute(dst=dst_member, src=src_member)

# --- String instructions -----------------------------------------------------

class StringInstruction(Instruction):
    """Base for string instructions. String instructions can also be executed
    on elements of a sequence or array
    """

    @abc.abstractmethod
    def execute_on_sequence(self, dst: Sequence[str], src: Any, index: int) -> None:
        """Executes this instruction on a element of a sequence"""


class StringConstantsMixin:
    """Defines the parameters required by the instruction for UTF-8 strings."""
    encoding = type_utils.STRING_ENCODING
    get_memoryview_func = core_utils.get_memoryview_from_string
    strcpy_func = core_utils.strcpy_from_buffer_object
    realloc_func = core_utils.string_realloc
    bytes_per_char = 1


class WideStringConstantsMixin:
    """Defines the parameters required by the instruction for UTF-16 strings."""
    encoding = type_utils.WSTRING_ENCODING
    get_memoryview_func = core_utils.get_memoryview_from_wstring
    strcpy_func = core_utils.wstrcpy_from_buffer_object
    realloc_func = core_utils.wstring_realloc
    bytes_per_char = type_utils.WSTRING_BYTES_PER_CHAR


class CopyBytesToStrInstructionMixin(StringInstruction):
    """Create and assign a Python string from a C string or wstring"""

    def execute_on_sequence(self, dst: Sequence[str], src: Any, index: int) -> None:
        c_member = src[index]

        # Get the C string as a memoryview (pointer, length) and decode it
        # from UTF-8 or UTF-16 to create the Python string.
        byte_array = self.get_memoryview_func(c_member)

        # Python strings are immutable; we need to create a new one every time.
        dst[index] = str(byte_array, self.encoding)

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        c_member = getattr(src, field_name)

        if c_member != None:
            # Get the C string as a memoryview (pointer, length) and decode it
            # from UTF-8 or UTF-16 to create the Python string.
            byte_array = self.get_memoryview_func(c_member)

            # Python strings are immutable; we need to create a new one every time.
            setattr(dst, field_name, str(byte_array, self.encoding))

class CopyStrToBytesInstructionMixin(StringInstruction):
    """Copy a Python string into a C string or wstring"""

    def __init__(self, field_name: str, field_index: int, is_optional: bool, bound: int):
        super().__init__(field_name, field_index=field_index, is_optional=is_optional)
        self.bound = bound

    def execute_on_sequence(self, dst, src: Sequence[str], index: int) -> None:

        py_member = src[index]

        # Convert the string into a bytes array with the right encoding:
        # UTF-8 for IDL string, and UTF-16 for IDL wstring
        py_bytes = py_member.encode(self.encoding)
        py_bytes_length = len(py_bytes)

        # char_length doesn't include the null terminator
        char_length = py_bytes_length // self.bytes_per_char

        if char_length > self.bound:
            raise ValueError(
                f'String length ({char_length}) exceeds bound ({self.bound}): \'{py_member}\'')

        # We call realloc even if the string is bounded because the XCDR
        # interpreter currently doesn't allocate the strings in a sequence
        c_member_ptr = as_int_ptr(dst[index])
        new_str = self.realloc_func(c_member_ptr, char_length)
        if c_member_ptr != new_str:
            dst[index] = new_str
            c_member_ptr = new_str

        # This C++ function copies the bytes and adds the null terminator into
        # the already-allocated c_member pointer.
        self.strcpy_func(c_member_ptr, py_bytes)

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)

        # Convert the string into a bytes array with the right encoding:
        # UTF-8 for IDL string, and UTF-16 for IDL wstring
        py_bytes = py_member.encode(self.encoding)
        py_bytes_length = len(py_bytes)

        # char_length doesn't include the null terminator
        char_length = py_bytes_length // self.bytes_per_char

        c_member = getattr(dst, field_name)
        if self.bound == annotations.UNBOUNDED:
            # When the string is unbounded we may need to reallocate it because
            # it has not been preallocated.
            c_member_as_int = as_int_ptr(c_member)
            new_str = self.realloc_func(c_member_as_int, char_length)
            if c_member_as_int != new_str:
                setattr(dst, field_name, new_str)
                c_member = new_str
        elif char_length > self.bound:
            raise ValueError(
                f'String length ({char_length}) exceeds bound ({self.bound}): \'{py_member}\'')

        # This C++ function copies the bytes and adds the null terminator into
        # the already-allocated c_member pointer.
        self.strcpy_func(as_int_ptr(c_member), py_bytes)


class CopyBytesToStrInstruction(StringConstantsMixin, CopyBytesToStrInstructionMixin):
    "Python to C string instruction"
    pass  # The two mixins define the execute method with the right parameters


class CopyBytesToWStrInstruction(WideStringConstantsMixin, CopyBytesToStrInstructionMixin):
    "Python to C wstring instruction"
    pass  # The two mixins define the execute method with the right parameters


class CopyStrToBytesInstruction(StringConstantsMixin, CopyStrToBytesInstructionMixin):
    "C to Python string instruction"
    pass


class CopyWStrToBytesInstruction(WideStringConstantsMixin, CopyStrToBytesInstructionMixin):
    "C to Python wstring) instruction"
    pass

# --- Sequence instructions ---------------------------------------------------

# All Python-to-C sequences may need to be resized, which requires access to the
# type plugin's resize_nember method.
class CopyListToCInstruction(Instruction):
    def __init__(
        self,
        field_name: str,
        field_index: int,
        is_optional: bool,
        container_type_plugin: dds._TypePlugin
    ) -> None:
        super().__init__(field_name, field_index=field_index, is_optional=is_optional)
        self.container_type_plugin = container_type_plugin

    def resize_sequence_member(self, c_sample, c_sequence, new_size: int) -> None:
        if c_sequence._absolute_maximum == 0x7FFFFFFF:
            # Unbounded sequences may need to be resized
            if new_size <= c_sequence.capacity:
                c_sequence.set_length(new_size)
            elif new_size != c_sequence._length:
                self.container_type_plugin.resize_member(
                    c_sample, self.field_index, new_size)
        elif c_sequence.capacity == 0 and new_size != 0:
            # This sequence is not initialized (it could be an optional sequence)
            self.container_type_plugin.resize_member(
                c_sample, self.field_index, new_size)
        else:
            # Bounded sequences must have been allocated already (this will
            # fail with an exception if new_size < maximum).
            c_sequence.set_length(new_size)

class CopyPrimitiveSequenceToListInstruction(Instruction):
    """Primitive sequence: C to Python

    Instruction to copy a sequence of primitive types from a C DDS_Sequence
    to a list-like python object (one that supports expand()).
    """

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = self.get_c_attr(src, field_name)
        c_elements = c_member.get_elements_ptr()
        length = len(c_member)

        # extend() first is faster than N append() calls
        py_member.extend(itertools.repeat(0, length))
        for i in range(length):
            py_member[i] = c_elements[i]


class CopyPrimitiveSequenceToExtendableBufferInstruction(Instruction):
    """Primitive sequence: C to Python

    Instruction to copy a sequence of primitive types from a C DDS_Sequence
    to a python object supporting the buffer protocol, using a memcpy.

    The instruction assumes that the object holding the buffer (e.g. an
    array.array) can be resized with .extend(range).
    """

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = self.get_c_attr(src, field_name)
        length = len(c_member)
        if length > 0:
            elements_ptr = c_member.get_elements_raw_ptr()
            py_member.extend(itertools.repeat(0, length))
            core_utils.memcpy_to_buffer_object(
                py_member, elements_ptr, c_member._element_size * length)


class CopyPrimitiveSequenceToResizableBufferInstruction(Instruction):
    """Primitive sequence: C to Python

    Instruction to copy a sequence of primitive types from a C DDS_Sequence
    to a python object supporting the buffer protocol, using a memcpy.

    The instruction assumes that the object holding the buffer (e.g. an
    array.array) can be resized with .resize(length).
    """

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = self.get_c_attr(src, field_name)
        length = len(c_member)
        if length > 0:
            elements_ptr = c_member.get_elements_raw_ptr()
            py_member.resize(length)
            core_utils.memcpy_to_buffer_object(
                py_member, elements_ptr, c_member._element_size * length)

class CopyPrimitiveSequenceToFixedSizeBufferInstruction(Instruction):
    """Primitive sequence: C to Python

    Instruction to copy a sequence of primitive types from a C DDS_Sequence
    to a python object supporting the buffer protocol, using a memcpy

    The instruction assumes that the object holding the buffer cannot be resized
    and therfore it creates a new one with the right size.
    """

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        c_member = self.get_c_attr(src, field_name)
        length = len(c_member)
        if length > 0:
            elements_ptr = c_member.get_elements_raw_ptr()
            py_member = self.field_factory(length)
            core_utils.memcpy_to_buffer_object(
                py_member, elements_ptr, c_member._element_size * length)
            setattr(dst, field_name, py_member)

class CopyPrimitiveListToSequenceInstruction(CopyListToCInstruction):
    """Primitive sequence: Python to C

    Instruction to copy a bounded sequence of primitive types
    """

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = self.get_c_attr(dst, field_name)
        length = len(py_member)
        self.resize_sequence_member(dst, c_member, length)
        c_elements = c_member.get_elements_ptr()
        for i in range(length):
            c_elements[i] = py_member[i]


class CopyBufferToSequenceInstruction(CopyListToCInstruction):
    """Primitive sequence: Python to C

    Instruction to copy a bounded sequence of primitive types using buffer
    protocol
    """

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = self.get_c_attr(dst, field_name)
        length = len(py_member)
        self.resize_sequence_member(dst, c_member, length)

        if length > 0:
            elements_ptr = c_member.get_elements_raw_ptr()
            core_utils.memcpy_from_buffer_object(
                elements_ptr, py_member, c_member._element_size * length)


class CopyBufferOrListToSequenceInstruction(CopyListToCInstruction):
    """Primitive sequence: Python to C

    Instruction to copy a bounded sequence of primitive types using buffer
    protocol or a list, depending on what the user provides
    """

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        length = len(py_member)
        c_member = self.get_c_attr(dst, field_name)
        self.resize_sequence_member(dst, c_member, length)

        if length > 0:
            if isinstance(py_member, list):
                c_elements = c_member.get_elements_ptr()
                for i in range(length):
                    c_elements[i] = py_member[i]
            else:
                elements_ptr = c_member.get_elements_raw_ptr()
                core_utils.memcpy_from_buffer_object(
                    elements_ptr, py_member, c_member._element_size * length)


class CopyConstructedSequenceToListInstruction(Instruction):
    """Complex sequence: C to Python

    Instruction to copy an bounded or unbounded sequence of constructed types
    into a Python list.
    """

    def __init__(
        self,
        field_name: str,
        field_index: int,
        field_factory: Any,
        element_factory: Any,
        is_optional: bool,
        element_program: 'SampleProgram'
    ) -> None:
        super().__init__(field_name, field_index=field_index,
                         field_factory=field_factory, is_optional=is_optional)
        self.element_factory = element_factory
        self.element_program = element_program

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = self.get_c_attr(src, field_name)
        c_elements = c_member.get_elements_ptr()
        for i in range(len(c_member)):
            element = self.element_factory()
            self.element_program.execute(dst=element, src=c_elements[i])
            py_member.append(element)


class CopyClassListToSequenceInstruction(CopyListToCInstruction):
    """Complex sequence: Python to C

    Instruction to copy a Python list into a bounded sequence of constructed
    types.
    """

    def __init__(
        self,
        field_name: str,
        field_index: int,
        is_optional: bool,
        container_type_plugin: dds._TypePlugin,
        element_program: 'SampleProgram'
    ) -> None:
        super().__init__(field_name, field_index=field_index,
                         is_optional=is_optional, container_type_plugin=container_type_plugin)
        self.element_program = element_program

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = self.get_c_attr(dst, field_name)
        length = len(py_member)
        self.resize_sequence_member(dst, c_member, length)

        c_elements = c_member.get_elements_ptr()
        for i in range(length):
            self.element_program.execute(dst=c_elements[i], src=py_member[i])


class CopyStrSequenceToListInstruction(Instruction):
    """String sequence: C to Python

    Instruction to copy an bounded or unbounded sequence of strings into a
    Python list.
    """

    def __init__(
        self, field_name: str,
        field_index: int,
        field_factory: Any,
        is_optional: bool,
        element_instruction: StringInstruction
    ) -> None:
        super().__init__(field_name, field_index=field_index,
                         field_factory=field_factory, is_optional=is_optional)
        self.element_instruction = element_instruction

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = self.get_c_attr(src, field_name)
        length = len(c_member)
        py_member.extend(itertools.repeat(None, length))
        for i in range(length):
            self.element_instruction.execute_on_sequence(
                dst=py_member, src=c_member, index=i)


class CopyStrListToSequenceInstruction(CopyListToCInstruction):
    """String sequence: Python to C

    Instruction to copy a Python list into a sequence of strings.
    """

    def __init__(
        self,
        field_name: str,
        field_index: int,
        is_optional: bool,
        container_type_plugin: dds._TypePlugin,
        element_instruction: StringInstruction
    ) -> None:
        super().__init__(field_name, field_index=field_index,
                         is_optional=is_optional, container_type_plugin=container_type_plugin)
        self.element_instruction = element_instruction

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = self.get_c_attr(dst, field_name)
        length = len(py_member)
        self.resize_sequence_member(dst, c_member, length)

        i = 0
        while i < length:
            self.element_instruction.execute_on_sequence(
                dst=c_member, src=py_member, index=i)
            i += 1


# --- Arrays ------------------------------------------------------------------

# Note that the following instructions duplicate significant ammounts of code
# from the sequence instructions. However, every function call that can be
# avoided goes a long way in improving performance.


class CopyPrimitiveArrayToListInstruction(Instruction):
    """Primitive array: C to Python

    Instruction to copy a sequence of primitive types from a C Array
    to a list-like python object (one that supports expand()).
    """

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = getattr(src, field_name)
        length = len(c_member)

        # py_member is expected to have the required number of elements
        for i in range(length):
            py_member[i] = c_member[i]


class CopyPrimitiveArrayToBufferInstruction(Instruction):
    """Primitive array: C to Python

    Instruction to copy an array of primitive types from a C array
    to a python object supporting the buffer protocol, using a memcpy
    """

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = getattr(src, field_name)

        # The py_member should have been created with the right element
        # count. If not, the memcpy call will fail with an exception.
        core_utils.memcpy_buffer_objects(py_member, c_member)


class CopyPrimitiveListToArrayInstruction(Instruction):
    """Primitive array: Python to C

    Instruction to copy an array primitive types
    """

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = getattr(dst, field_name)
        length = len(c_member)
        if len(py_member) != length:
            raise ValueError(
                f"Expected array length of {length} but got {len(py_member)}")

        for i in range(length):
            c_member[i] = py_member[i]


class CopyBufferToArrayInstruction(Instruction):
    """Primitive array: Python to C

    Instruction to copy an array of primitive types using buffer protocol
    """

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = getattr(dst, field_name)
        length = len(c_member)
        if len(py_member) != length:
            raise ValueError(
                f"Expected array length of {length} but got {len(py_member)}")

        core_utils.memcpy_buffer_objects(c_member, py_member)


class CopyConstructedArrayToListInstruction(Instruction):
    """Complex array: C to Python

    Instruction to an array of constructed types into a Python list.
    """

    def __init__(self, field_name: str, py_element_factory, element_program: 'SampleProgram'):
        super().__init__(field_name)
        self.py_element_factory = py_element_factory
        self.element_program = element_program

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = getattr(src, field_name)

        # py_member is expected to have the required number of elements
        for i in range(len(c_member)):
            self.element_program.execute(dst=py_member[i], src=c_member[i])


class CopyClassListToArrayInstruction(Instruction):
    """Complex array: Python to C

    Instruction to copy a Python list into an array of constructed types.
    """

    def __init__(self, field_name: str, element_program: 'SampleProgram'):
        super().__init__(field_name)
        self.element_program = element_program

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = getattr(dst, field_name)
        length = len(c_member)

        if len(py_member) != length:
            raise ValueError(
                f"Expected array length of {length} but got {len(py_member)}")

        for i in range(length):
            self.element_program.execute(dst=c_member[i], src=py_member[i])

class CopyStrArrayToListInstruction(Instruction):
    """String array: C to Python

    Instruction to copy an array of strings into a Python list.
    """

    def __init__(self, field_name: str, element_instruction: StringInstruction):
        super().__init__(field_name)
        self.element_instruction = element_instruction

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = getattr(src, field_name)
        length = len(c_member)

        # py_member is expected to have the required number of elements
        for i in range(length):
            self.element_instruction.execute_on_sequence(
                dst=py_member, src=c_member, index=i)


class CopyStrListToArrayInstruction(Instruction):
    """String array: Python to C

    Instruction to copy a Python list into an array of strings.
    """

    def __init__(self, field_name: str, element_instruction: StringInstruction):
        super().__init__(field_name)
        self.element_instruction = element_instruction

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = getattr(dst, field_name)
        length = len(c_member)

        i = 0
        while i < length:
            self.element_instruction.execute_on_sequence(
                dst=c_member, src=py_member, index=i)
            i += 1


# --- Program -----------------------------------------------------------------


class SampleProgram:
    """A sample program is a sequence of steps to copy a sample of one type into
    another
    """

    def __init__(self, instructions: List[Instruction], type_plugin: dds._TypePlugin = None) -> None:
        self.instructions = instructions
        self.type_plugin = type_plugin

    def execute(self, dst, src):
        """Runs all the instructions of this program"""

        for instruction in self.instructions:
            try:
                if instruction.is_primitive:
                    # Optimization for primitive types to avoid extra function calls
                    setattr(dst, instruction.field_name,
                            getattr(src, instruction.field_name))
                else:
                    if instruction.is_optional:
                        try:
                            src_member_value = getattr(src, instruction.field_name)
                        except AttributeError:
                            # For optional members we are able to handle not
                            # just None, but even a non-existent member.
                            continue

                        if self.type_plugin is not None:
                            # src is the Python sample and dst is the C sample
                            if src_member_value is not None:
                                # Python optional member is set: allocate the
                                # optional member in the C sample
                                self.type_plugin.initialize_member(
                                    dst, instruction.field_index)

                                instruction.execute(dst=dst, src=src)
                        else:
                            # src is the C sample and dst is the Python sample.
                            if src_member_value: # check for non-NULL C pointer
                                if instruction.field_factory is not None:
                                    # C optional member is set: construct the
                                    # Python member if needed
                                    setattr(dst, instruction.field_name,
                                            instruction.field_factory())

                                instruction.execute(dst=dst, src=src)
                    else:
                        instruction.execute(dst=dst, src=src)
            except Exception as ex:
                raise FieldSerializationError(instruction.field_name) from ex

    def __repr__(self) -> str:
        return f"SampleProgram({len(self.instructions)})"


class UnionSampleProgram:
    """A union sample program consists of an instruction to obtain the
    discriminator and a set of instructions to choose one according to the
    discriminator.
    """

    def __init__(
        self,
        discriminator_instruction: Instruction,
        instructions: Dict[Any, Instruction],
        type_plugin: dds._TypePlugin = None
    ) -> None:
        self.discriminator_instruction = discriminator_instruction
        self.instructions = instructions
        self.default_instruction: Optional[Instruction] = instructions.get(
            DEFAULT_LABEL)
        self.type_plugin = type_plugin

    def execute(self, dst, src):
        """Runs the discriminator instruction followed by the instruction it
        selects.
        """

        try:
            instruction = self.discriminator_instruction

            # The discriminator is always a primitive or enum so we know
            # the instruction is always getattr/setattr
            discriminator = getattr(src, instruction.field_name)
            discriminator_field = instruction.field_name
            setattr(dst, discriminator_field, discriminator)

            instruction = self.instructions.get(
                discriminator, self.default_instruction)

            if instruction is not None:
                if self.type_plugin is None:
                    # src is the C sample and dst is the Python sample.
                    if instruction.field_factory is not None:
                        # Construct the python member
                        setattr(dst, instruction.field_name,
                                instruction.field_factory())

                instruction.execute(dst=dst, src=src)

            # Temporary solution to handle the case where multiple
            # discriminators select the same case. We need to set it again in
            # case instruction.execute called the instruction.field_name setter
            # and selected another discriminator.
            #
            # The solution is to not call the setters, but assign to dst.value
            # (when dst is the Python object).
            setattr(dst, discriminator_field, discriminator)

        except Exception as ex:
            raise FieldSerializationError(instruction.field_name) from ex

    def __repr__(self) -> str:
        return f"UnionSampleProgram({len(self.instructions)})"


@dataclass
class SampleProgramOptions:
    allow_primitive_lists: bool = True

DEFAULT_SAMPLE_PROGRAM_OPTIONS = SampleProgramOptions()

class SamplePrograms:
    """Contains the programs to copy a dataclass object to a ctypes object and
    viceversa
    """

    def __init__(
        self,
        py_type: type,
        c_type: type,
        type_plugin,
        member_annotations,
        is_union: bool = False,
        options: SampleProgramOptions = DEFAULT_SAMPLE_PROGRAM_OPTIONS
    ):
        self.options = options
        if reflection_utils.is_enum(py_type):
            self.c_to_py_program, self.py_to_c_program = self._create_enum_programs(
                py_type, type_plugin)
        else:
            if not hasattr(py_type, '__dataclass_fields__'):
                raise TypeError(f"{py_type} is not a dataclass")
            if not hasattr(c_type, '_fields_'):
                raise TypeError(f"{c_type} is not a ctype")

            if is_union:
                self.c_to_py_program, self.py_to_c_program = self._create_union_programs(
                    py_type, type_plugin, member_annotations)
            else:
                self.c_to_py_program, self.py_to_c_program = self._create_struct_programs(
                    py_type, type_plugin, member_annotations)

    def _create_struct_programs(
        self,
        py_type: type,
        type_plugin,
        member_annotations
    ):
        c_to_py_instructions, py_to_c_instructions = self._create_class_instructions(
            fields(py_type),
            first_index=0,
            type_plugin=type_plugin,
            member_annotations=member_annotations)
        return SampleProgram(c_to_py_instructions), SampleProgram(py_to_c_instructions, type_plugin)

    def _create_union_programs(
        self,
        py_type: type,
        type_plugin,
        member_annotations
    ):
        discr = union_discriminator(py_type)
        c_to_py_discr_instructions, py_to_c_discr_instructions = self._create_class_instructions(
            (discr,),
            first_index=0,
            type_plugin=type_plugin,
            member_annotations=member_annotations)

        cases = union_cases(py_type)
        c_to_py_instructions, py_to_c_instructions = self._create_class_instructions(
            cases,
            first_index=1,
            type_plugin=type_plugin,
            member_annotations=member_annotations)

        assert len(cases) == len(py_to_c_instructions)
        assert len(cases) == len(c_to_py_instructions)

        c_to_py_instruction_map = {}
        py_to_c_instruction_map = {}
        for i, case in enumerate(cases):
            # The actual Case object is in case.default (case is a dataclass field)
            for label in case.default.labels:
                c_to_py_instruction_map[label] = c_to_py_instructions[i]
                py_to_c_instruction_map[label] = py_to_c_instructions[i]

        return UnionSampleProgram(
                c_to_py_discr_instructions[0], c_to_py_instruction_map), \
            UnionSampleProgram(
                py_to_c_discr_instructions[0], py_to_c_instruction_map, type_plugin)

    def _create_class_instructions(
        self,
        fields,
        first_index,
        type_plugin,
        member_annotations
    ):
        c_to_py_instructions = []
        py_to_c_instructions = []
        i = first_index
        for field in fields:
            field_name = field.name
            field_type = reflection_utils.remove_classvar(field.type)
            current_annotations = member_annotations.get(field_name, {})
            field_factory = self._get_field_factory(field, current_annotations)
            is_optional = False
            if reflection_utils.is_optional_type(field_type):
                field_type = reflection_utils.get_underlying_type(field_type)
                is_optional = True

            c_to_py_instr = None
            py_to_c_instr = None

            # Translate from the member index to the index of the instruction
            # in the XCDR initialization program. This is the argument required
            # to manipulate individual member (e.g. in resize_member) in the
            # C type using the XCDR interpreter.
            plugin_instruction_index = type_plugin.get_instruction_index_from_member_index(i)

            if reflection_utils.is_primitive(field_type):
                c_to_py_instr = CopyPrimitiveToPyInstruction(
                    field_name, is_optional=is_optional, field_index=plugin_instruction_index)
                py_to_c_instr = CopyPrimitiveToCInstruction(
                    field_name, is_optional=is_optional, field_index=plugin_instruction_index)
            elif reflection_utils.is_enum(field_type):
                c_to_py_instr = CopyInt32ToIntEnumInstruction(
                    field_name, enum_type=field_type, is_optional=is_optional)
                py_to_c_instr = CopyIntEnumToInt32Instruction(
                    field_name, field_index=plugin_instruction_index, is_optional=is_optional)
            elif field_type is str:
                c_to_py_instr, py_to_c_instr = self._create_string_instructions(
                    field_name,
                    field_index=plugin_instruction_index,
                    is_optional=is_optional,
                    field_factory=field_factory,
                    string_annotations=current_annotations)
            elif reflection_utils.is_sequence_type(field_type):
                # In the Python type there is no distinction between an array
                # and a sequence; the ArrayAnnotation makes it an array.
                array_info = annotations.find_annotation(
                    current_annotations, annotations.ArrayAnnotation)
                if array_info.is_array:
                    c_to_py_instr, py_to_c_instr = self._create_array_instructions(
                        field,
                        is_optional=is_optional,
                        field_factory=field_factory,
                        sequence_annotations=current_annotations)
                else:
                    c_to_py_instr, py_to_c_instr = self._create_sequence_instructions(
                        type_plugin,
                        field,
                        field_index=plugin_instruction_index,
                        is_optional=is_optional,
                        field_factory=field_factory,
                        sequence_annotations=current_annotations)
            elif reflection_utils.is_constructed_type(field_type):
                c_to_py_instr = CopyAggregationToPyInstruction(
                    field_name,
                    is_optional=is_optional,
                    field_factory=field_factory,
                    sample_program=field_type.type_support._sample_programs.c_to_py_program)
                py_to_c_instr = CopyAggregationToCInstruction(
                    field_name,
                    field_index=plugin_instruction_index,
                    is_optional=is_optional,
                    sample_program=field_type.type_support._sample_programs.py_to_c_program)

            if c_to_py_instr is not None:
                c_to_py_instructions.append(c_to_py_instr)
            if py_to_c_instr is not None:
                py_to_c_instructions.append(py_to_c_instr)

            i += 1

        return c_to_py_instructions, py_to_c_instructions

    def __repr__(self) -> str:
        return f"SamplePrograms:({len(self.c_to_py_program)})"

    def _create_enum_programs(
        self,
        py_type: type,
        type_plugin,
    ):
        return SampleProgram([]), SampleProgram([])

    def _create_string_instructions(
        self,
        field_name: str,
        field_index: int,
        is_optional: bool,
        field_factory,
        string_annotations,
    ) -> Tuple[StringInstruction, StringInstruction]:

        bound = annotations.find_annotation(
            string_annotations, annotations.BoundAnnotation)
        encoding = annotations.find_annotation(
            string_annotations, annotations.CharEncodingAnnotation)

        if encoding.value == annotations.CharEncoding.UTF16:
            c_to_py_instr = CopyBytesToWStrInstruction(
                field_name, field_factory=field_factory)
            py_to_c_instr = CopyWStrToBytesInstruction(
                field_name, field_index=field_index, is_optional=is_optional, bound=bound.value)
        else:
            c_to_py_instr = CopyBytesToStrInstruction(
                field_name, field_factory=field_factory)
            py_to_c_instr = CopyStrToBytesInstruction(
                field_name, field_index=field_index, is_optional=is_optional, bound=bound.value)

        return c_to_py_instr, py_to_c_instr


    def _create_sequence_instructions(
        self,
        container_type_plugin,
        field_info,
        field_index: int,
        is_optional: bool,
        field_factory,
        sequence_annotations
    ):
        field_name = field_info.name
        element_type = reflection_utils.remove_classvar(field_info.type)
        if reflection_utils.is_optional_type(element_type):
            # E.g. from Optional[Sequence[int]] get Sequence[int]
            element_type = reflection_utils.get_underlying_type(element_type)
        # E.g. from Sequence[int] get int
        element_type = reflection_utils.get_underlying_type(element_type)
        bound = annotations.find_annotation(
            sequence_annotations, annotations.BoundAnnotation)

        if reflection_utils.is_primitive_or_enum(element_type):
            if reflection_utils.supports_buffer_protocol(field_factory):
                # C to Py: select the optimal way to resize the python collection
                if reflection_utils.sequence_is_resizable(field_factory):
                    c_to_py_instr = CopyPrimitiveSequenceToResizableBufferInstruction(
                        field_name,
                        field_index=field_index,
                        is_optional=is_optional,
                        field_factory=field_factory)
                elif reflection_utils.supports_size_argument(field_factory):
                    c_to_py_instr = CopyPrimitiveSequenceToFixedSizeBufferInstruction(
                        field_name,
                        field_index=field_index,
                        is_optional=is_optional,
                        field_factory=field_factory)
                else:
                    c_to_py_instr = CopyPrimitiveSequenceToExtendableBufferInstruction(
                        field_name,
                        field_index=field_index,
                        is_optional=is_optional,
                        field_factory=field_factory)
                # Py to C
                if self.options.allow_primitive_lists:
                    py_to_c_instr = CopyBufferOrListToSequenceInstruction(
                        field_name,
                        field_index=field_index,
                        is_optional=is_optional,
                        container_type_plugin=container_type_plugin)
                else:
                    py_to_c_instr = CopyBufferToSequenceInstruction(
                        field_name,
                        field_index=field_index,
                        is_optional=is_optional,
                        container_type_plugin=container_type_plugin)
            else:
                c_to_py_instr = CopyPrimitiveSequenceToListInstruction(
                    field_name,
                    field_index=field_index,
                    is_optional=is_optional,
                    field_factory=field_factory)
                py_to_c_instr = CopyPrimitiveListToSequenceInstruction(
                    field_name,
                    field_index=field_index,
                    is_optional=is_optional,
                    container_type_plugin=container_type_plugin)
        elif element_type is str:
            # Get the annotations applied to the element type
            string_annotations = annotations.find_annotation(
                sequence_annotations, annotations.ElementAnnotations)
            element_instructions =  self._create_string_instructions(
                field_name=None,
                field_index=-1,
                is_optional=False,
                field_factory=None,
                string_annotations=string_annotations.value)

            c_to_py_instr = CopyStrSequenceToListInstruction(
                field_name,
                field_index=field_index,
                is_optional=is_optional,
                field_factory=field_factory,
                element_instruction=element_instructions[0])
            py_to_c_instr = CopyStrListToSequenceInstruction(
                field_name,
                field_index=field_index,
                is_optional=is_optional,
                container_type_plugin=container_type_plugin,
                element_instruction=element_instructions[1])
        else:
            element_ts = element_type.type_support
            c_to_py_instr = CopyConstructedSequenceToListInstruction(
                field_name,
                field_index=field_index,
                is_optional=is_optional,
                field_factory=field_factory,
                element_factory=element_type,
                element_program=element_ts._sample_programs.c_to_py_program)
            py_to_c_instr = CopyClassListToSequenceInstruction(
                field_name,
                field_index=field_index,
                is_optional=is_optional,
                container_type_plugin=container_type_plugin,
                element_program=element_ts._sample_programs.py_to_c_program)

        return c_to_py_instr, py_to_c_instr


    def _create_array_instructions(
        self,
        field,
        is_optional: bool,
        field_factory,
        sequence_annotations
    ):
        if is_optional:
            raise TypeError('Optional arrays are not supported')

        field_type = reflection_utils.remove_classvar(field.type)
        element_type = reflection_utils.get_underlying_type(field_type)

        if reflection_utils.is_primitive_or_enum(element_type):
            factory_type = field.default_factory
            if reflection_utils.supports_buffer_protocol(factory_type):
                c_to_py_instr = CopyPrimitiveArrayToBufferInstruction(
                    field.name)
                py_to_c_instr = CopyBufferToArrayInstruction(
                    field.name)
            else:
                c_to_py_instr = CopyPrimitiveArrayToListInstruction(
                    field.name)
                py_to_c_instr = CopyPrimitiveListToArrayInstruction(
                    field.name)

        elif element_type is str:
            # Get the annotations applied to the element type
            string_annotations = annotations.find_annotation(
                sequence_annotations, annotations.ElementAnnotations)
            element_instructions = self._create_string_instructions(
                field_name=None,
                field_index=-1,
                is_optional=False,
                field_factory=None,
                string_annotations=string_annotations.value)

            c_to_py_instr = CopyStrArrayToListInstruction(
                field.name,
                element_instructions[0])

            py_to_c_instr = CopyStrListToArrayInstruction(
                field.name,
                element_instructions[1])
        else:
            element_ts = element_type.type_support
            c_to_py_instr = CopyConstructedArrayToListInstruction(
                field.name, element_type, element_ts._sample_programs.c_to_py_program)

            py_to_c_instr = CopyClassListToArrayInstruction(
                field.name,
                element_ts._sample_programs.py_to_c_program)

        c_to_py_instr.field_factory = field_factory
        return c_to_py_instr, py_to_c_instr

    def _get_field_factory(self, field, member_annotations) -> Callable[[], Any]:
        if field.default_factory is not MISSING:
            return field.default_factory

        field_type = reflection_utils.remove_classvar(field.type)
        if reflection_utils.is_optional_type(field_type):
            field_type = reflection_utils.get_underlying_type(field_type)

        if reflection_utils.is_primitive_or_enum(field_type):
            return None

        if field_type is str:
            return None

        if reflection_utils.is_sequence_type(field_type):
            # In the Python type there is no distinction between an array
            # and a sequence; the ArrayAnnotation makes it an array.
            element_type = reflection_utils.get_underlying_type(field_type)
            array_info = annotations.find_annotation(
                member_annotations, annotations.ArrayAnnotation)

            # For arrays, the factory returns fixed-size collections; for
            # sequences, the factory returns empty collections.
            dimensions = array_info.dimensions if array_info.is_array else 0
            return type_utils.get_optimal_collection_factory(
                element_type,
                dimensions)

        if reflection_utils.is_constructed_type(field_type):
            return field_type
