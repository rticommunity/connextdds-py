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

from typing import Any, List

import rti.connextdds as dds
import rti.idl_impl.reflection_utils as reflection_utils
import rti.connextdds.core_utils as core_utils
import itertools

#
# This module provides generic functions to convert IDL-based Python types to
# C types (ctypes) and vice-versa.
#
# There are two phases in this process:
#   - Compilation phase: the types are inspected to generate a "program" that
#     provides the steps to perform these convertions
#   - Execution phase: the compiled program is executed to convert each object.
#

class Instruction:
    """Base class for instructions that can be executed by the interpreter"""

    def __init__(self, field_name: str, is_primivite: bool = False):
        self.field_name = field_name
        self.is_primitive = is_primivite


class CopyPrimitiveInstruction(Instruction):
    """Instruction to copy a primitive type from one object to another."""

    def __init__(self, field_name: str):
        super().__init__(field_name, is_primivite=True)

    def execute(self, dst: Any, src: Any) -> None:
        setattr(dst, self.field_name, getattr(src, self.field_name))

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

class CopyPrimitiveSequenceToListInstruction(Instruction):
    """Instruction to copy a sequence of primitive types from a C DDS_Sequence
    to a list-like python object (one that supports expand()).
    """
    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = getattr(src, field_name)
        c_elements = c_member.get_elements_ptr()
        for i in range(len(c_member)):
            py_member.append(c_elements[i])


class CopyPrimitiveSequenceToListInstruction(Instruction):
    """Instruction to copy a sequence of primitive types from a C DDS_Sequence
    to a list-like python object (one that supports expand()).
    """

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = getattr(src, field_name)
        c_elements = c_member.get_elements_ptr()
        length = len(c_member)
        py_member.extend([0] * length)
        for i in range(length):
            py_member[i] = c_elements[i]


class CopyPrimitiveSequenceToBufferInstruction(Instruction):
    """Instruction to copy a sequence of primitive types from a C DDS_Sequence
    to a python object supporting the buffer protocol, using a memcpy.
    """
    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(dst, field_name)
        c_member = getattr(src, field_name)
        elements_ptr = c_member.get_elements_raw_ptr()
        length = len(c_member)
        if length > 0:
            py_member.extend(itertools.repeat(0, length))
            core_utils.memcpy_to_buffer_object(
                py_member, elements_ptr, c_member._element_size * length)


class CopyPrimitiveListToSequenceInstruction(Instruction):
    """Instruction to copy a sequence of primitive types from one object to
    another.
    """
    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = getattr(dst, field_name)
        lenght = len(py_member)
        c_member.set_length(lenght)

        c_elements = c_member.get_elements_ptr()
        i = 0
        while i < lenght:
            c_elements[i] = py_member[i]
            i += 1


class CopyConstructedSequenceToListInstruction(Instruction):
    """Instruction to copy a sequence of constructed types from one object to
    another.
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


class CopyClassListToSequenceInstruction(Instruction):
    """Instruction to copy a sequence of constructed types from one object to
    another.
    """

    def __init__(self, field_name: str, element_program: 'SampleProgram'):
        super().__init__(field_name)
        self.element_program = element_program

    def execute(self, dst: Any, src: Any) -> None:
        field_name = self.field_name
        py_member = getattr(src, field_name)
        c_member = getattr(dst, field_name)
        lenght = len(py_member)
        c_member.set_length(lenght)

        c_elements = c_member.get_elements_ptr()
        i = 0
        while i < lenght:
            self.element_program.execute(dst=c_elements[i], src=py_member[i])
            i += 1


class SampleProgram:
    """A sample program is a sequence of steps to copy a sample of one type into
    another
    """

    def __init__(self, instructions: List[Instruction]) -> None:
        self.instructions: List[Instruction] = instructions

    def execute(self, dst, src):
        for instruction in self.instructions:
            try:
                if instruction.is_primitive:
                    # Optimization for primitive types to avoid extra function calls
                    setattr(dst, instruction.field_name,
                            getattr(src, instruction.field_name))
                else:
                    instruction.execute(dst=dst, src=src)
            except Exception as e:
                raise dds.Exception(
                    f"Error processing field '{instruction.field_name}'", e)

class SamplePrograms:
    """
    Contains the programs to copy a dataclass object to a ctypes object and
    viceversa"""

    def __init__(self, py_type: type, c_type: type):
        if not hasattr(py_type, '__dataclass_fields__'):
            raise TypeError(f"{py_type} is not a dataclass")
        if not hasattr(c_type, '_fields_'):
            raise TypeError(f"{c_type} is not a ctype")

        self.c_to_py_program, self.py_to_c_program = self._create_programs(
            py_type, c_type)

    def _create_programs(self, py_type: type, c_type: type):
        c_to_py_instructions = []
        py_to_c_instructions = []
        fields = py_type.__dataclass_fields__
        for field_name, field_info in fields.items():
            field_type = field_info.type
            c_to_py_instr = None
            py_to_c_instr = None
            if reflection_utils.is_primitive(field_type):
                c_to_py_instr = CopyPrimitiveInstruction(field_name)
                py_to_c_instr = c_to_py_instr # Same instruction both ways
            elif reflection_utils.is_sequence_type(field_type):
                c_to_py_instr, py_to_c_instr = self._create_sequence_instructions(
                    field_info, field_name)
            elif reflection_utils.is_constructed_type(field_type):
                c_to_py_instr = CopyAggregationInstruction(
                    field_name, field_type.type_support.sample_programs.c_to_py_program)
                py_to_c_instr = CopyAggregationInstruction(
                    field_name, field_type.type_support.sample_programs.py_to_c_program)

            if c_to_py_instr is not None:
                c_to_py_instructions.append(c_to_py_instr)
            if py_to_c_instr is not None:
                py_to_c_instructions.append(py_to_c_instr)

        return SampleProgram(c_to_py_instructions), SampleProgram(py_to_c_instructions)

    def _create_sequence_instructions(self, field_info, field_name):
        element_type = reflection_utils.get_underlying_type(field_info.type)
        if reflection_utils.is_primitive(element_type):
            factory_type = field_info.default_factory
            if factory_type is not list:
                c_to_py_instr = CopyPrimitiveSequenceToBufferInstruction(
                    field_name)
            else:
                c_to_py_instr = CopyPrimitiveSequenceToListInstruction(
                    field_name)
            py_to_c_instr = CopyPrimitiveListToSequenceInstruction(field_name)
        else:
            c_to_py_instr = CopyConstructedSequenceToListInstruction(
                field_name, element_type, element_type.type_support.sample_programs.c_to_py_program)
            py_to_c_instr = CopyClassListToSequenceInstruction(
                field_name, element_type.type_support.sample_programs.py_to_c_program)

        return c_to_py_instr, py_to_c_instr
