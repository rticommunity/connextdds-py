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

from dataclasses import InitVar, dataclass, fields
from typing import Union, List, ClassVar, get_origin, get_args
import itertools
import array
import atexit
import ctypes
import rti.connextdds as dds
import rti.idl_impl.sample_interpreter as sample_interpreter
import rti.idl_impl.csequence as csequence
import rti.idl_impl.reflection_utils as reflection_utils
from rti.idl_impl.type_hints import *

#
# This module contains the implementation details of the IDL Type Support
#

_type_factory = dds._GenericTypePluginFactory.instance

def finalize_type_plugin_factory() -> None:
    """Release the native memory used by _type_factory"""
    global _type_factory
    _type_factory = None
    dds._GenericTypePluginFactory.delete_instance()

# --- Annotation classes ------------------------------------------------------

@dataclass
class KeyAnnotation:
    value: bool = False


AUTO_ID = 0x7FFFFFFF

@dataclass
class IdAnnotation:
    value: int = AUTO_ID

@dataclass
class ExtensibilityAnnotation:
    value: dds.ExtensibilityKind = dds.ExtensibilityKind.EXTENSIBLE

UNBOUNDED = 0xFFFFFFFF

@dataclass
class BoundAnnotation:
    value: int = 100 # TODO PY-18: make default unbounded

def find_annotation(annotations, cls, default=None):
    if default is None:
        default = cls()

    for annotation in annotations:
        if isinstance(annotation, cls):
            return annotation
    return default


@dataclass
class PrimitiveArrayFactory:
    element_type: type
    size: int

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

# --- C/Python type conversions -----------------------------------------------

TYPE_TO_CTYPES_MAP = {
    int8: ctypes.c_int8,
    int16: ctypes.c_int16,
    uint16: ctypes.c_uint16,
    int32: ctypes.c_int32,
    uint32: ctypes.c_uint32,
    int64: ctypes.c_int64,
    uint64: ctypes.c_uint64,
    float32: ctypes.c_float,
    float64: ctypes.c_double,
}

def get_offsets(type: ctypes.Structure) -> List[int]:
    """
    Get the in-memory offsets of the fields of a C structure
    """

    return [getattr(type, field_name).offset for field_name, _ in type._fields_]


def get_size(type: ctypes.Structure) -> int:
    """
    Get the size of a C structure
    """

    return ctypes.sizeof(type)


def _get_member_ctype(py_type, member_annotations):
    if reflection_utils.is_optional_type(py_type):
        underlying_type = reflection_utils.get_underlying_type(py_type)
        underlying_ctype = _get_member_ctype(underlying_type, member_annotations)
        if reflection_utils.is_constructed_type(underlying_ctype):
            # Optimization: constructed types cache their pointer type, saving
            # the creation of a new ctypes.POINTER type.
            return py_type.type_support.c_type_ptr
        else:
            return ctypes.POINTER(underlying_ctype)

    basic_type = TYPE_TO_CTYPES_MAP.get(py_type, None)
    if basic_type is not None:
        return basic_type

    if reflection_utils.is_sequence_type(py_type):
        element_ctype = _get_member_ctype(reflection_utils.get_underlying_type(py_type),
                                          member_annotations)
        if reflection_utils.is_constructed_type(element_ctype):
            # Optimization: constructed types cache their sequence type
            return py_type.type_support.c_type_seq
        else:
            return csequence.create_sequence_type(element_ctype)

    if reflection_utils.is_constructed_type(py_type):
        return py_type.type_support.c_type

    raise TypeError(f'Unsupported member type {py_type}')

def create_ctype_from_dataclass(py_type):
    """Given a user-level dataclass, return an equivalent ctypes type that """
    """can be used by the C interpreter"""

    c_fields = []
    for field in fields(py_type):
        member_type = _get_member_ctype(field.type, None)
        c_fields.append((field.name, member_type))

    return type(
        py_type.__name__ + 'Native',
        (ctypes.Structure,),
        {
            '__doc__': f'Equivalent C type for {py_type.__name__}',
            '_fields_': c_fields
        }
    )

# --- Dynamic type creation ---------------------------------------------------

def bounded_string():
    return dds.StringType(128)

py_to_dynamic_type_map = {
    int: dds.Int32Type(),
    float: dds.Float64Type(),
    bool: dds.BoolType()
}

PY_TYPE_TO_DYNAMIC_TYPE_MAP = {
    int8: dds.Int8Type(),
    int16: dds.Int16Type(),
    uint16: dds.Uint16Type(),
    int32: dds.Int32Type(),
    uint32: dds.Uint32Type(),
    int64: dds.Int64Type(),
    uint64: dds.Uint64Type(),
    float32: dds.Float32Type(),
    float64: dds.Float64Type(),
}

def _get_member_dynamic_type(py_type, member_annotations):
    if reflection_utils.is_optional_type(py_type):
        underlying_type = reflection_utils.get_underlying_type(py_type)
        return _get_member_dynamic_type(underlying_type, member_annotations)

    basic_type = PY_TYPE_TO_DYNAMIC_TYPE_MAP.get(py_type, None)
    if basic_type is not None:
        return basic_type

    if reflection_utils.is_sequence_type(py_type):
        member_type = reflection_utils.get_underlying_type(py_type)
        member_dynamic_type = _get_member_dynamic_type(member_type, member_annotations)
        bound = find_annotation(member_annotations, cls=BoundAnnotation)
        return _type_factory.create_sequence(member_dynamic_type, bound.value)

    if reflection_utils.is_constructed_type(py_type):
        return py_type.type_support._dynamic_type_ref

    raise TypeError(f'Unsupported member type {py_type}')

def create_dynamic_type_from_dataclass(py_type, c_type=None, type_annotations=None, member_annotations=None):
    """Given an IDL-derived dataclass, return the DynamicType that describes """
    """the IDL type"""

    extensibility = find_annotation(type_annotations, ExtensibilityAnnotation)

    if member_annotations is None:
        member_annotations = {}

    member_args = []
    for field in fields(py_type):
        annotations = member_annotations.get(field.name, {})
        is_optional = reflection_utils.is_optional_type(field.type)
        member_type = _get_member_dynamic_type(field.type, annotations)

        is_key = find_annotation(annotations, cls=KeyAnnotation)
        member_id = find_annotation(annotations, cls=IdAnnotation)

        member_args.append(
            (field.name, member_type, member_id.value, is_key.value, is_optional, False))

    # Create the struct type only after all the members have been parsed. The
    # type_factory requires that dependent types are created first
    dynamic_type = _type_factory.create_struct(
        py_type.__name__, extensibility.value, get_size(c_type), get_offsets(c_type))

    for member_arg in member_args:
        _type_factory.add_member(dynamic_type, *member_arg)

    # Once finalized the type creation, this creates the plugin and assigns it
    # to dynamic_type
    _type_factory.create_type_plugin(dynamic_type)

    return dynamic_type


# --- C/Python sample conversion ----------------------------------------------

def copy_python_str_to_ctype(py_str):
    """Convert a python string to a ctypes string."""
    if py_str is None:
        return None
    return ctypes.c_char_p(py_str.encode('utf-8'))


def create_python_str_from_ctype(ctype_str):
    """Convert a ctypes string to a python string."""
    if ctype_str is None:
        return None
    return ctype_str.decode('utf-8')


def copy_to_c_sample(sample, c_sample):
    """Copy a python sample into a C sample."""

    for field, value in sample.__dict__.items():
        if isinstance(value, str):
            setattr(c_sample, field, copy_python_str_to_ctype(value))
        else:
            member_type = type(value)
            if hasattr(member_type, 'type_support'):
                copy_to_c_sample(value, getattr(c_sample, field))
            else:
                setattr(c_sample, field, value)


def create_c_sample(sample):
    c_sample = type(sample).type_support.c_type()
    copy_to_c_sample(sample, c_sample)
    return c_sample


def copy_from_c_sample(sample, c_sample):
    """Copy a C sample into a python sample."""
    for field, value in sample.__dict__.items():
        if isinstance(value, str):
            setattr(sample, field, create_python_str_from_ctype(
                getattr(c_sample, field)))
        else:
            if hasattr(type(value), 'type_support'):
                copy_from_c_sample(value, getattr(c_sample, field))
            else:
                setattr(sample, field, getattr(c_sample, field))


# --- Type support ------------------------------------------------------------

class TypeSupport:
    """Support and utility methods for the usage of an IDL type"""

    def __init__(self, idl_type, type_annotations=None, member_annotations=None):
        self.type = idl_type
        self.c_type = create_ctype_from_dataclass(idl_type)
        self.c_type_ptr = ctypes.POINTER(self.c_type)
        self.c_type_seq = csequence.create_sequence_type(self.c_type)
        self._plugin_dynamic_type = create_dynamic_type_from_dataclass(
            idl_type, self.c_type, type_annotations, member_annotations)

        self.sample_programs = sample_interpreter.SamplePrograms(
            self.type, self.c_type)

    def _create_c_sample(self, sample):
        if (type(sample) is not self.type):
            raise TypeError(
                f'Expected type {self.type}, got {type(sample)}')

        c_sample = self.c_type()
        self._plugin_dynamic_type.initialize_sample(c_sample)
        try:
            self.sample_programs.py_to_c_program.execute(src=sample, dst=c_sample)
        except:
            self._plugin_dynamic_type.finalize_sample(c_sample)
            raise
        return c_sample

    def _cast_c_sample(self, c_sample_ptr):
        return ctypes.cast(c_sample_ptr, self.c_type_ptr)[0]

    def _create_py_sample(self, c_sample_ptr):
        py_sample = self.type()
        c_sample = self._cast_c_sample(c_sample_ptr)
        self.sample_programs.c_to_py_program.execute(
            src=c_sample, dst=py_sample)
        return py_sample

    def _create_py_sample_no_ptr(self, c_sample):
        py_sample = self.type()
        self.sample_programs.c_to_py_program.execute(
            src=c_sample, dst=py_sample)
        return py_sample

    def serialize(self, sample):
        """Serialize a data sample into a cdr byte buffer"""

        c_sample = self._create_c_sample(sample)
        try:
            return self._plugin_dynamic_type.serialize(c_sample)
        finally:
            self._plugin_dynamic_type.finalize_sample(c_sample)

    def deserialize(self, buffer):
        """
        Create a data sample for the data type for this TypeSupport by
        deserializing a cdr byte buffer"""

        c_sample = self.c_type()
        self._plugin_dynamic_type.initialize_sample(c_sample)
        try:
            self._plugin_dynamic_type.deserialize(c_sample, buffer)
            return self._create_py_sample_no_ptr(c_sample)
        finally:
            self._plugin_dynamic_type.finalize_sample(c_sample)

    @property
    def _dynamic_type_ref(self):
        return self._plugin_dynamic_type.get_dynamic_type_ref()

    @property
    def dynamic_type(self):
        return self._plugin_dynamic_type.clone()
