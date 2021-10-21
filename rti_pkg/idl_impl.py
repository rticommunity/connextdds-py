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

from dataclasses import dataclass
from typing import Union, List, get_origin, get_args
import atexit
import ctypes
import rti.connextdds as dds

#
# This module contains the implementation details of the IDL Type Support
#

_type_factory = dds._GenericTypePluginFactory.instance

@atexit.register
def _cleanup():
    """Clean up global variables when this module is unloaded."""

    _type_factory = None
    dds._GenericTypePluginFactory.delete_instance()

# --- Annotation classes ------------------------------------------------------

@dataclass
class KeyAnnotation:
    value: bool = False

@dataclass
class IdAnnotation:
    value: int = -1

@dataclass
class ExtensibilityAnnotation:
    value: dds.ExtensibilityKind = dds.ExtensibilityKind.EXTENSIBLE

def find_annotation(annotations, cls, default=None):
    if default is None:
        default = cls()

    for annotation in annotations:
        if isinstance(annotation, cls):
            return annotation
    return default

# --- C/Python type conversions -----------------------------------------------

py_to_ctypes_map = {
    int: ctypes.c_int,
    float: ctypes.c_double,
    str: ctypes.c_char_p,
    bool: ctypes.c_byte
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


def is_optional_type(t):
    return get_origin(t) is Union


def get_underlying_optional_type(t):
    return get_args(t)[0]


def create_ctype_from_dataclass(py_type):
    """Given a user-level dataclass, return an equivalent ctypes type that """
    """can be used by the C interpreter"""

    c_fields = []

    for field, info in py_type.__dataclass_fields__.items():

        if is_optional_type(info.type):
            underlying_type = get_underlying_optional_type(info.type)
            member_type = ctypes.POINTER(py_to_ctypes_map[underlying_type])
        else:
            member_type = py_to_ctypes_map[info.type]

        c_fields.append((field, member_type))

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
    int: dds.Int32Type,
    float: dds.Float64Type,
    str: bounded_string,
    bool: dds.BoolType
}

def create_dynamic_type_from_dataclass(py_type, c_type=None, type_annotations=None, member_annotations=None):
    """Given an IDL-derived dataclass, return the DynamicType that describes """
    """the IDL type"""

    extensibility = find_annotation(type_annotations, ExtensibilityAnnotation)

    dynamic_type = _type_factory.create_struct(
        py_type.__name__, extensibility.value, get_size(c_type), get_offsets(c_type))

    if member_annotations is None:
        member_annotations = {}

    for field, info in py_type.__dataclass_fields__.items():
        is_optional = False
        if is_optional_type(info.type):
            underlying_type = get_underlying_optional_type(info.type)
            member_dynamic_type = py_to_dynamic_type_map.get(
                underlying_type, None)
            is_optional = True
        else:
            member_dynamic_type = py_to_dynamic_type_map.get(
                info.type, None)

        if member_dynamic_type is None:
            member_dynamic_type = info.type.type_support.dynamic_type

        annotations = member_annotations.get(field, {})
        is_key = find_annotation(annotations, cls=KeyAnnotation)
        member_id = find_annotation(annotations, cls=IdAnnotation)

        _type_factory.add_member(
            dynamic_type, field, member_dynamic_type(), id=member_id.value, is_key=is_key.value, is_optional=is_optional, is_external=False)

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
            setattr(sample, field, getattr(c_sample, field))

def create_py_sample(idl_type, c_type):
    def _create_py_sample(c_sample_ptr):
        py_sample = idl_type()
        c_sample = ctypes.cast(c_sample_ptr, ctypes.POINTER(c_type))[0]
        copy_from_c_sample(py_sample, c_sample)
        return py_sample
    return _create_py_sample

# --- Type support ------------------------------------------------------------

class TypeSupport:
    """
    Supporting properties for an IDL type
    """

    def __init__(self, idl_type, type_annotations=None, member_annotations=None):
        self.type = idl_type
        self.c_type = create_ctype_from_dataclass(idl_type)
        self._plugin_dynamic_type = create_dynamic_type_from_dataclass(
            idl_type, self.c_type, type_annotations, member_annotations)

    def _create_c_sample(self, sample):
        c_sample = self.c_type()
        copy_to_c_sample(sample, c_sample)
        return c_sample

    def _create_py_sample(self, c_sample_ptr):
        py_sample = self.type()
        c_sample = ctypes.cast(c_sample_ptr, ctypes.POINTER(self.c_type))[0]
        copy_from_c_sample(py_sample, c_sample)
        return py_sample

    @property
    def dynamic_type(self):
        return self._plugin_dynamic_type.clone()
