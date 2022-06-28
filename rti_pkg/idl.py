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

import dataclasses
from types import SimpleNamespace
from typing import List, Union, Any
import rti.idl_impl.type_plugin as idl_impl
import rti.idl_impl.annotations as annotations
import rti.idl_impl.type_hints as type_hints
import rti.idl_impl.sample_interpreter as sample_interpreter
import rti.idl_impl.reflection_utils as reflection_utils
import rti.idl_impl.unions as unions
import rti.idl_impl.type_utils as type_utils
import rti.connextdds

#
# This module contains the public interface for decorating Python classes
# as DDS/IDL types
#


# --- Custom type hints -------------------------------------------------------

# The following publicly exposes the type hints defined in the type_hints module
# They're not defined directly here because they're also used internally in
# idl_impl.

int8 = type_hints.int8

int16 = type_hints.int16
uint16 = type_hints.uint16

int32 = type_hints.int32
uint32 = type_hints.uint32

# int64 is provided for consistency, but int has the same meaning
int64 = type_hints.int64
uint64 = type_hints.uint64

# float64 is provided for consistency, but float has the same meaning
float32 = type_hints.float32
float64 = type_hints.float64

char = type_hints.char
wchar = type_hints.wchar

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
    rti.connextdds.ExtensibilityKind.EXTENSIBLE)
mutable = annotations.ExtensibilityAnnotation(
    rti.connextdds.ExtensibilityKind.MUTABLE)
final = annotations.ExtensibilityAnnotation(
    rti.connextdds.ExtensibilityKind.FINAL)


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
        field_descriptor = unions.DefaultCase(list(args))
    elif len(args) == 0:
        raise TypeError("At least one case label is required")
    elif len(args) > 1:
        field_descriptor = unions.MultiCase(list(args))
    else:
        field_descriptor = unions.Case(args[0])

    return field_descriptor

# --- Dataclass factories -----------------------------------------------------

array_factory = type_utils.array_factory
list_factory = type_utils.list_factory

# --- Exceptions --------------------------------------------------------------

FieldSerializationError = sample_interpreter.FieldSerializationError

# --- Utility functions -------------------------------------------------------

to_array = type_utils.to_array
to_char = type_utils.to_char
to_wchar = type_utils.to_wchar
from_char = type_utils.from_char
from_wchar = type_utils.from_wchar

# --- Decorators --------------------------------------------------------------


def struct(cls=None, *, type_annotations=[], member_annotations={}):
    """This decorator makes a Python class usable as DDS topic-type defined as
    an IDL struct.
    """

    def wrapper(cls):
        actual_cls = dataclasses.dataclass(cls)
        actual_cls.type_support = idl_impl.TypeSupport(
            actual_cls,
            idl_impl.TypeSupportKind.STRUCT,
            type_annotations,
            member_annotations)
        return actual_cls
    if cls is None:
        # Decorator used with arguments:
        #  @idl.struct(type_annotations={...}, ...)
        #  class Foo:
        return wrapper
    else:
        # Decorator used without arguments:
        #  @idl.struct
        #  class Foo:
        return wrapper(cls)


def union(cls=None, *, type_annotations=[], member_annotations={}):
    """This decorator makes a Python class usable as DDS topic-type defined as
    an IDL union.

    The class requires two instance members: discriminator and value, plus
    an idl.case field for each union case.
    """

    def wrapper(cls):
        unions.configure_union_class(cls)
        cls.type_support = idl_impl.TypeSupport(
            cls,
            idl_impl.TypeSupportKind.UNION,
            type_annotations,
            member_annotations)

        return cls
    if cls is None:
        return wrapper
    else:
        return wrapper(cls)


def alias(cls=None, *, annotations=[]):
    """This decorator maps a Python class with a single field to an IDL alias;
    the type of the field refers to the aliased type.
    """

    def wrapper(cls):
        actual_cls = dataclasses.dataclass(cls)
        if isinstance(annotations, list):
            member_annotations = {'value': annotations}
        else:
            member_annotations = annotations

        actual_cls.type_support = idl_impl.TypeSupport(
            actual_cls,
            idl_impl.TypeSupportKind.ALIAS,
            type_annotations=None,
            member_annotations=member_annotations)
        return actual_cls
    if cls is None:
        return wrapper
    else:
        return wrapper(cls)

def enum(cls=None, *, type_annotations=[]):
    """This decorator makes a Python IntEnum usable as an IDL enum"""

    def wrapper(cls):
        if not reflection_utils.is_enum(cls):
            raise TypeError(f"{cls} is not an IntEnum")

        cls.type_support = idl_impl.TypeSupport(
            cls,
            idl_impl.TypeSupportKind.ENUM,
            type_annotations)
        return cls
    if cls is None:
        # Decorator used with arguments:
        #  @idl.enum(type_annotations={...}, ...)
        #  class Foo(IntEnum):
        return wrapper
    else:
        # Decorator used without arguments:
        #  @idl.enum
        #  class Foo(IntEnum):
        return wrapper(cls)


_idl_modules = {}


def get_module(name: str):
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

    idl_impl.finalize_type_plugin_factory()
