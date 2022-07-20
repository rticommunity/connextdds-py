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

import dataclasses
import rti.idl_impl.unions as unions
import rti.idl_impl.type_plugin as idl_impl
import rti.idl_impl.reflection_utils as reflection_utils
import rti.idl_impl.sample_interpreter as sample_interpreter

# --- Serialization options ---------------------------------------------------

@dataclasses.dataclass
class SerializationOptions:
    """Configures certain global options that control how types are
    serialized.

    The singleton variable that can be modified is called serialization_options.
    It must be modified before the definition of the @struct- or
    @union-decorated types for which the options are to be applied.
    """

    allow_primitive_lists: bool = True


serialization_options = SerializationOptions()


def _get_sample_program_options(
    serialization_options: SerializationOptions
) -> sample_interpreter.SampleProgramOptions:
    return sample_interpreter.SampleProgramOptions(
        allow_primitive_lists=serialization_options.allow_primitive_lists)


def _get_current_sample_program_options():
    return _get_sample_program_options(serialization_options)


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
            member_annotations,
            sample_program_options=_get_current_sample_program_options())
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
            member_annotations,
            sample_program_options=_get_current_sample_program_options())

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
            member_annotations=member_annotations,
            sample_program_options=_get_current_sample_program_options())
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
