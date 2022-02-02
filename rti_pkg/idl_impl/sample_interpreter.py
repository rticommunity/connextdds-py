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

from typing import Any, List, Tuple, Sequence
import itertools
import abc

import rti.connextdds as dds
import rti.idl_impl.reflection_utils as reflection_utils
import rti.idl_impl.annotations as annotations
import rti.connextdds.core_utils as core_utils


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

    def __init__(self, field_name: str, is_primitive: bool = False):
        self.field_name = field_name
        self.is_primitive = is_primitive

    @abc.abstractmethod
    def execute(self, dst: Any, src: Any) -> None:
        """Executes the instruction, copying src int dst"""


class CopyPrimitiveInstruction(Instruction):
    """Instruction to copy a primitive type from one object to another."""

    def __init__(self, field_name: str):
        super().__init__(field_name, is_primitive=True)

    def execute(self, dst: Any, src: Any) -> None:
        setattr(dst, self.field_name, getattr(src, self.field_name))

# --- Structs  ----------------------------------------------------------------


class CopyAggregationInstruction(Instruction):
    """An instruction to recursively copy an aggregation type from one object
    to another.
    """

    def __init__(self, field_name: str, sample_program: 'SampleProgram'):
        super().__init__(field_name)
        self.sample_program = sample_program

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        dst_member = getattr(dst, field_name)
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
    encoding = 'utf-8'
    get_memoryview_func = core_utils.get_memoryview_from_string
    strcpy_func = core_utils.strcpy_from_buffer_object
    realloc_func = core_utils.string_realloc
    bytes_per_char = 1


class WideStringConstantsMixin:
    """Defines the parameters required by the instruction for UTF-16 strings."""
    encoding = 'utf-16-le'
    get_memoryview_func = core_utils.get_memoryview_from_wstring
    strcpy_func = core_utils.wstrcpy_from_buffer_object
    realloc_func = core_utils.wstring_realloc
    bytes_per_char = 2


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

        # Get the C string as a memoryview (pointer, length) and decode it
        # from UTF-8 or UTF-16 to create the Python string.
        byte_array = self.get_memoryview_func(c_member)

        # Python strings are immutable; we need to create a new one every time.
        setattr(dst, field_name, str(byte_array, self.encoding))


class CopyStrToBytesInstructionMixin(StringInstruction):
    """Copy a Python string into a C string or wstring"""

    def __init__(self, field_name: str, bound: int):
        super().__init__(field_name)
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

        # TODO PY-17: Review if this should be implemented in C:
        #
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

class CopyPrimitiveSequenceToListInstruction(Instruction):
    """Primitive sequence: C to Python

    Instruction to copy a sequence of primitive types from a C DDS_Sequence
    to a list-like python object (one that supports expand()).
    """

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = getattr(src, field_name)
        c_elements = c_member.get_elements_ptr()
        length = len(c_member)

        # extend() first is faster than N append() calls
        py_member.extend([0] * length)
        for i in range(length):
            py_member[i] = c_elements[i]

class CopyPrimitiveSequenceToBufferInstruction(Instruction):
    """Primitive sequence: C to Python

    Instruction to copy a sequence of primitive types from a C DDS_Sequence
    to a python object supporting the buffer protocol, using a memcpy
    """

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = getattr(src, field_name)
        length = len(c_member)
        if length > 0:
            elements_ptr = c_member.get_elements_raw_ptr()
            py_member.extend(itertools.repeat(0, length))
            core_utils.memcpy_to_buffer_object(
                py_member, elements_ptr, c_member._element_size * length)


# TODO PY-17: Add Py-to-C optimized implementation when Py supports buffer
# protocol (we already have that impl in the other direction)
class CopyPrimitiveListToBoundedSequenceInstruction(Instruction):
    """Primitive sequence: Python to C

    Instruction to copy a bounded sequence of primitive types
    """

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = getattr(dst, field_name)
        length = len(py_member)
        c_member.set_length(length)

        c_elements = c_member.get_elements_ptr()
        for i in range(length):
            c_elements[i] = py_member[i]


class CopyPrimitiveListToUnboundedSequenceInstruction(Instruction):
    """Primitive sequence: Python to C

    Instruction to copy an ubounded sequence of primitive types
    """

    def __init__(
        self,
        field_name: str,
        member_offset: int,
        member_dynamic_type: dds.DynamicType
    ):
        super().__init__(field_name)
        self.member_offset = member_offset
        self.member_dynamic_type = member_dynamic_type

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = getattr(dst, field_name)
        length = len(py_member)

        dds._TypePlugin.resize_sequence(
            dst, self.member_dynamic_type, self.member_offset, length)

        c_elements = c_member.get_elements_ptr()
        for i in range(length):
            c_elements[i] = py_member[i]


class CopyConstructedSequenceToListInstruction(Instruction):
    """Complex sequence: C to Python

    Instruction to copy an bounded or unbounded sequence of constructed types
    into a Python list.
    """

    def __init__(self, field_name: str, py_element_factory, element_program: 'SampleProgram'):
        super().__init__(field_name)
        self.py_element_factory = py_element_factory
        self.element_program = element_program

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = getattr(src, field_name)
        c_elements = c_member.get_elements_ptr()
        for i in range(len(c_member)):
            element = self.py_element_factory()
            self.element_program.execute(dst=element, src=c_elements[i])
            py_member.append(element)


class CopyBoundedClassListToSequenceInstruction(Instruction):
    """Complex sequence: Python to C

    Instruction to copy a Python list into a bounded sequence of constructed
    types.
    """

    def __init__(self, field_name: str, element_program: 'SampleProgram'):
        super().__init__(field_name)
        self.element_program = element_program

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = getattr(dst, field_name)
        length = len(py_member)

        # Bounded sequences are pre-initialized to the maximum, so we can
        # just set the length
        c_member.set_length(length)

        c_elements = c_member.get_elements_ptr()
        for i in range(length):
            self.element_program.execute(dst=c_elements[i], src=py_member[i])


class CopyUnboundedClassListToSequenceInstruction(Instruction):
    """Complex sequence: Python to C

    Instruction to copy a Python list into a unbounded sequence of constructed
    types.
    """

    def __init__(
        self,
        field_name: str,
        member_offset: int,
        member_dynamic_type: dds.DynamicType,
        element_program: 'SampleProgram'
    ):
        super().__init__(field_name)
        self.element_program = element_program
        self.member_offset = member_offset
        self.member_dynamic_type = member_dynamic_type

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = getattr(dst, field_name)
        length = len(py_member)

        # Unbounded sequences are not pre-initialized, so we need to use the
        # C interpreter to allocate and initialize the elements
        dds._TypePlugin.resize_sequence(
            dst, self.member_dynamic_type, self.member_offset, length)
        c_elements = c_member.get_elements_ptr()
        for i in range(length):
            self.element_program.execute(dst=c_elements[i], src=py_member[i])


class CopyStrSequenceToListInstruction(Instruction):
    """String sequence: C to Python

    Instruction to copy an bounded or unbounded sequence of strings into a
    Python list.
    """

    def __init__(self, field_name: str, element_instruction: StringInstruction):
        super().__init__(field_name)
        self.element_instruction = element_instruction

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = getattr(src, field_name)
        length = len(c_member)
        py_member.extend(itertools.repeat(None, length))
        for i in range(length):
            self.element_instruction.execute_on_sequence(
                dst=py_member, src=c_member, index=i)


class CopyBoundedStrListToSequenceInstruction(Instruction):
    """String sequence: Python to C

    Instruction to copy a Python list into a bounded sequence of strings.
    """

    def __init__(self, field_name: str, element_instruction: StringInstruction):
        super().__init__(field_name)
        self.element_instruction = element_instruction

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = getattr(dst, field_name)
        length = len(py_member)

        # Bounded sequences are pre-initialized to the maximum, so we can
        # just set the length
        c_member.set_length(length)

        i = 0
        while i < length:
            self.element_instruction.execute_on_sequence(
                dst=c_member, src=py_member, index=i)
            i += 1


class CopyUnboundedStrListToSequenceInstruction(Instruction):
    """String sequence: Python to C

    Instruction to copy a Python list into a unbounded sequence of strings.
    """

    def __init__(
        self,
        field_name: str,
        member_offset: int,
        member_dynamic_type: dds.DynamicType,
        element_instruction: StringInstruction
    ):
        super().__init__(field_name)
        self.element_instruction = element_instruction
        self.member_offset = member_offset
        self.member_dynamic_type = member_dynamic_type

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = getattr(dst, field_name)
        length = len(py_member)

        # Unbounded sequences are not pre-initialized, so we need to use the
        # C interpreter to allocate and initialize the elements
        dds._TypePlugin.resize_sequence(
            dst, self.member_dynamic_type, self.member_offset, length)

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

    def __init__(self, instructions: List[Instruction]) -> None:
        self.instructions: List[Instruction] = instructions

    def execute(self, dst, src):
        """Runs all the instructions of this program"""

        for instruction in self.instructions:
            try:
                if instruction.is_primitive:
                    # Optimization for primitive types to avoid extra function calls
                    setattr(dst, instruction.field_name,
                            getattr(src, instruction.field_name))
                else:
                    instruction.execute(dst=dst, src=src)
            except Exception as ex:
                raise FieldSerializationError(instruction.field_name) from ex


class SamplePrograms:
    """Contains the programs to copy a dataclass object to a ctypes object and
    viceversa
    """

    def __init__(
        self,
        py_type:
        type,
        c_type: type,
        type_plugin,
        member_annotations
    ):
        if not hasattr(py_type, '__dataclass_fields__'):
            raise TypeError(f"{py_type} is not a dataclass")
        if not hasattr(c_type, '_fields_'):
            raise TypeError(f"{c_type} is not a ctype")

        self.c_to_py_program, self.py_to_c_program = self._create_programs(
            py_type, c_type, type_plugin, member_annotations)

    def _create_programs(
        self,
        py_type: type,
        c_type: type,
        type_plugin,
        member_annotations
    ):
        c_to_py_instructions = []
        py_to_c_instructions = []
        fields = py_type.__dataclass_fields__
        for field_name, field_info in fields.items():
            field_type = field_info.type
            c_to_py_instr = None
            py_to_c_instr = None
            if reflection_utils.is_primitive(field_type):
                c_to_py_instr = CopyPrimitiveInstruction(field_name)
                py_to_c_instr = c_to_py_instr  # Same instruction both ways
            elif field_type is str:
                current_annotations = member_annotations.get(field_name, {})
                c_to_py_instr, py_to_c_instr = self._create_string_instructions(
                    field_name, current_annotations)
            elif reflection_utils.is_sequence_type(field_type):
                current_annotations = member_annotations.get(field_name, {})
                c_to_py_instr, py_to_c_instr = self._create_sequence_instructions(
                    c_type,
                    type_plugin,
                    field_info,
                    field_name,
                    current_annotations)
            elif reflection_utils.is_constructed_type(field_type):
                c_to_py_instr = CopyAggregationInstruction(
                    field_name,
                    field_type.type_support._sample_programs.c_to_py_program)
                py_to_c_instr = CopyAggregationInstruction(
                    field_name,
                    field_type.type_support._sample_programs.py_to_c_program)

            if c_to_py_instr is not None:
                c_to_py_instructions.append(c_to_py_instr)
            if py_to_c_instr is not None:
                py_to_c_instructions.append(py_to_c_instr)

        return SampleProgram(c_to_py_instructions), SampleProgram(py_to_c_instructions)

    def _create_string_instructions(
        self,
        field_name: str,
        string_annotations,
    ) -> Tuple[StringInstruction, StringInstruction]:

        bound = annotations.find_annotation(
            string_annotations, annotations.BoundAnnotation)
        encoding = annotations.find_annotation(
            string_annotations, annotations.CharEncodingAnnotation)

        if encoding.value == annotations.CharEncoding.UTF16:
            c_to_py_instr = CopyBytesToWStrInstruction(field_name)
            py_to_c_instr = CopyWStrToBytesInstruction(field_name, bound.value)
        else:
            c_to_py_instr = CopyBytesToStrInstruction(field_name)
            py_to_c_instr = CopyStrToBytesInstruction(field_name, bound.value)

        return c_to_py_instr, py_to_c_instr


    def _create_sequence_instructions(
        self,
        container_c_type,
        container_type_plugin,
        field_info,
        field_name,
        sequence_annotations
    ):
        element_type = reflection_utils.get_underlying_type(field_info.type)
        bound = annotations.find_annotation(
            sequence_annotations, annotations.BoundAnnotation)

        if reflection_utils.is_primitive(element_type):
            factory_type = field_info.default_factory
            if factory_type is not list:
                c_to_py_instr = CopyPrimitiveSequenceToBufferInstruction(
                    field_name)
            else:
                c_to_py_instr = CopyPrimitiveSequenceToListInstruction(
                    field_name)
            if bound.value == annotations.UNBOUNDED:
                member_dynamic_type = container_type_plugin.get_member_dynamic_type_ref(
                    field_name)
                py_to_c_instr = CopyPrimitiveListToUnboundedSequenceInstruction(
                    field_name,
                    member_offset=getattr(container_c_type, field_name).offset,
                    member_dynamic_type=member_dynamic_type)
            else:
                py_to_c_instr = CopyPrimitiveListToBoundedSequenceInstruction(
                    field_name)
        elif element_type is str:
            # Get the annotations applied to the element type
            string_annotations = annotations.find_annotation(
                sequence_annotations, annotations.ElementAnnotations)
            element_instructions =  self._create_string_instructions(
                None,
                string_annotations.value)

            c_to_py_instr = CopyStrSequenceToListInstruction(
                field_name,
                element_instructions[0])

            if bound.value == annotations.UNBOUNDED:
                member_dynamic_type = container_type_plugin.get_member_dynamic_type_ref(
                    field_name)
                py_to_c_instr = CopyUnboundedStrListToSequenceInstruction(
                    field_name,
                    member_offset=getattr(container_c_type, field_name).offset,
                    member_dynamic_type=member_dynamic_type,
                    element_instruction=element_instructions[1])
            else:
                py_to_c_instr = CopyBoundedStrListToSequenceInstruction(
                    field_name,
                    element_instructions[1])
        else:
            element_ts = element_type.type_support
            c_to_py_instr = CopyConstructedSequenceToListInstruction(
                field_name, element_type, element_ts._sample_programs.c_to_py_program)
            if bound.value == annotations.UNBOUNDED:
                member_dynamic_type = container_type_plugin.get_member_dynamic_type_ref(
                    field_name)
                py_to_c_instr = CopyUnboundedClassListToSequenceInstruction(
                    field_name,
                    member_offset=getattr(container_c_type, field_name).offset,
                    member_dynamic_type=member_dynamic_type,
                    element_program=element_ts._sample_programs.py_to_c_program)
            else:
                py_to_c_instr = CopyBoundedClassListToSequenceInstruction(
                    field_name,
                    element_ts._sample_programs.py_to_c_program)

        return c_to_py_instr, py_to_c_instr
