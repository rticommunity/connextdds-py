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
import rti.connextdds

#
# This module contains the public interface for decorating Python classes
# as DDS/IDL types
#

# --- Member annotations ------------------------------------------------------

key = idl_impl.KeyAnnotation(True)

def id(value: int) -> idl_impl.IdAnnotation:
    return idl_impl.IdAnnotation(value)

# --- Type annotations --------------------------------------------------------

extensible = idl_impl.ExtensibilityAnnotation(
    rti.connextdds.ExtensibilityKind.EXTENSIBLE)
mutable = idl_impl.ExtensibilityAnnotation(
    rti.connextdds.ExtensibilityKind.MUTABLE)
final = idl_impl.ExtensibilityAnnotation(
    rti.connextdds.ExtensibilityKind.FINAL)

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
