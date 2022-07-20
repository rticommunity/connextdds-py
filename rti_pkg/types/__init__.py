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

from types import SimpleNamespace
from typing import List, Union, Any

# The following imports are intentionally exposed outside this module
import rti.idl_impl.annotations as annotations
import rti.idl_impl.type_utils as type_utils
from rti.idl_impl.type_plugin import finalize_type_plugin_factory

# The following imports are private for use in this file only
import rti.idl_impl.type_hints as _type_hints
import rti.idl_impl.sample_interpreter as _sample_interpreter
import rti.idl_impl.unions as _unions
import rti.idl_impl.decorators as _decorators
from rti.connextdds import ExtensibilityKind as _ExtensibilityKind

#
# This module contains the public interface for decorating Python classes
# as DDS/IDL types
#


# --- Custom type hints -------------------------------------------------------

# The following publicly exposes the type hints defined in the type_hints module
# They're not defined directly here because they're also used internally in
# idl_impl.

int8 = _type_hints.int8
uint8 = _type_hints.uint8

int16 = _type_hints.int16
uint16 = _type_hints.uint16

int32 = _type_hints.int32
uint32 = _type_hints.uint32

# int64 is provided for consistency, but int has the same meaning
int64 = _type_hints.int64
uint64 = _type_hints.uint64

# float64 is provided for consistency, but float has the same meaning
float32 = _type_hints.float32
float64 = _type_hints.float64

char = _type_hints.char
wchar = _type_hints.wchar

# --- Member annotations ------------------------------------------------------

key = annotations.KeyAnnotation(True)


def id(value: int) -> annotations.IdAnnotation:
    """Annotation that sets a explicit member ID. By default they're assigned
    automatically
    """
    return annotations.IdAnnotation(int(value))


def bound(value: int):
    """Annotation that sets the maximum size for a Sequence or a str field"""
    return annotations.BoundAnnotation(int(value))


unbounded = bound(annotations.UNBOUNDED)


def array(dimensions: Union[None, int, List[int]]):
    """Annotation that configures a Sequence field as an array with the given
    dimensions
    """
    return annotations.ArrayAnnotation(dimensions)


utf8 = annotations.CharEncodingAnnotation(annotations.CharEncoding.UTF8)
utf16 = annotations.CharEncodingAnnotation(annotations.CharEncoding.UTF16)


def element_annotations(value: List[Any]):
    """Sets the annotations for the element type of a sequence or array"""
    return annotations.ElementAnnotations(value)

# --- Type annotations --------------------------------------------------------


extensible = annotations.ExtensibilityAnnotation(
    _ExtensibilityKind.EXTENSIBLE)
mutable = annotations.ExtensibilityAnnotation(
    _ExtensibilityKind.MUTABLE)
final = annotations.ExtensibilityAnnotation(
    _ExtensibilityKind.FINAL)


def allowed_data_representation(xcdr1=True, xcdr2=True):
    """Type annotation that specifies which serialized data representation the type supports"""
    value = annotations.AllowedDataRepresentationFlags.XCDR1 if xcdr1 else 0
    value |= annotations.AllowedDataRepresentationFlags.XCDR2 if xcdr2 else 0
    return annotations.AllowedDataRepresentationAnnotation(value)

# --- Union cases -------------------------------------------------------------


def case(*args, **kwargs):
    """Returns a field descriptor that allows getting and setting a union
    case with the right discriminator
    """

    if kwargs.get('is_default', False):
        field_descriptor = _unions.DefaultCase(list(args))
    elif len(args) == 0:
        raise TypeError("At least one case label is required")
    elif len(args) > 1:
        field_descriptor = _unions.MultiCase(list(args))
    else:
        field_descriptor = _unions.Case(args[0])

    return field_descriptor

# --- Dataclass factories -----------------------------------------------------


def array_factory(element_type: type, size: Union[int, List[int]] = 0):
    # Note that type_utils.array_factory can be set with a custom factory
    return type_utils.array_factory(element_type, size)


list_factory = type_utils.list_factory  # list_factory can't be customized

# --- Exceptions --------------------------------------------------------------

FieldSerializationError = _sample_interpreter.FieldSerializationError

# --- Utility functions -------------------------------------------------------

to_array = type_utils.to_array
to_char = type_utils.to_char
to_wchar = type_utils.to_wchar
from_char = type_utils.from_char
from_wchar = type_utils.from_wchar

# --- Decorators --------------------------------------------------------------

struct = _decorators.struct
union = _decorators.union
enum = _decorators.enum
alias = _decorators.alias
serialization_options = _decorators.serialization_options
SerializationOptions = _decorators.SerializationOptions

_idl_modules = {}


def get_module(name: str) -> SimpleNamespace:
    """Returns a SimpleNamespace that contains types defined in an IDL module
    for a given name. The syntax is '::MyModule' or '::MyModule::MySubmodule'.
    """

    # Return the module if it already exists or create it if it doesn't
    return _idl_modules.setdefault(name, SimpleNamespace())


def get_type_support(cls: type):
    if not hasattr(cls, 'type_support'):
        raise TypeError(f"{cls} is not an IDL type")
    return cls.type_support

# --- Other utilties ----------------------------------------------------------


def finalize_globals() -> None:
    """Optionally finalize global state before the application ends.

    This method can only be used after all other DDS entities have been closed.

    Most applications don't need to call this function. It is useful when
    profiling native memory usage to ensure that the application ends with no
    in-use memory.
    """

    finalize_type_plugin_factory()
