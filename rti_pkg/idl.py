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
import rti.idl_impl.type_plugin as idl_impl
import rti.idl_impl.type_hints as type_hints
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

# --- Member annotations ------------------------------------------------------

key = idl_impl.KeyAnnotation(True)


def id(value: int) -> idl_impl.IdAnnotation:
    return idl_impl.IdAnnotation(value)


def bound(value: int):
    return idl_impl.BoundAnnotation(value)


unbouned = bound(idl_impl.UNBOUNDED)

# --- Type annotations --------------------------------------------------------

extensible = idl_impl.ExtensibilityAnnotation(
    rti.connextdds.ExtensibilityKind.EXTENSIBLE)
mutable = idl_impl.ExtensibilityAnnotation(
    rti.connextdds.ExtensibilityKind.MUTABLE)
final = idl_impl.ExtensibilityAnnotation(
    rti.connextdds.ExtensibilityKind.FINAL)

# --- Dataclass extensions ----------------------------------------------------


def array_factory(element_type: type, size: int = 0):
    return idl_impl.PrimitiveArrayFactory(element_type, size)

# --- Decorators --------------------------------------------------------------


def struct(cls=None, *, type_annotations={}, member_annotations={}):
    """This decorator makes a Python class usable as DDS topic-type defined as
    an IDL struct.
    """

    def wrapper(cls):
        actual_cls = dataclasses.dataclass(cls)
        actual_cls.type_support = idl_impl.TypeSupport(
            actual_cls, type_annotations, member_annotations)
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
