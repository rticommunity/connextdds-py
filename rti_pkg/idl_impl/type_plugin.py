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

from dataclasses import fields
from typing import List, Any, Optional, Dict
from enum import Enum
import ctypes
import rti.connextdds as dds
import rti.idl_impl.sample_interpreter as sample_interpreter
import rti.idl_impl.csequence as csequence
import rti.idl_impl.reflection_utils as reflection_utils
from rti.idl_impl.unions import Case, union_cases, union_discriminator

import rti.idl_impl.type_hints as type_hints
import rti.idl_impl.annotations as annotations

#
# This module contains the implementation details of the IDL Type Support
#

_internal_type_factory = dds._GenericTypePluginFactory.instance
_public_type_factory = dds._GenericTypePluginFactory.public_instance

def _get_type_factory(is_public: bool) -> dds._GenericTypePluginFactory:
    return _public_type_factory if is_public else _internal_type_factory


def finalize_type_plugin_factory() -> None:
    """Release the native memory used by _type_factory"""
    global _internal_type_factory
    global _public_type_factory
    _internal_type_factory = None
    _public_type_factory = None
    dds._GenericTypePluginFactory.delete_instance()

# --- C/Python type conversions -----------------------------------------------


PRIMITIVE_TO_CTYPES_MAP = {
    bool: ctypes.c_int8,
    type_hints.int8: ctypes.c_int8,
    type_hints.int16: ctypes.c_int16,
    type_hints.uint16: ctypes.c_uint16,
    type_hints.int32: ctypes.c_int32,
    type_hints.uint32: ctypes.c_uint32,
    type_hints.int64: ctypes.c_int64,
    type_hints.uint64: ctypes.c_uint64,
    type_hints.float32: ctypes.c_float,
    type_hints.float64: ctypes.c_double,
    type_hints.char: ctypes.c_int8,
    type_hints.wchar: ctypes.c_uint16
}

STR_ENCODING_TO_CTYPES_MAP = {
    annotations.CharEncoding.UTF8: ctypes.c_void_p,
    annotations.CharEncoding.UTF16: ctypes.c_void_p
}


def get_offsets(type: ctypes.Structure) -> List[int]:
    """Get the in-memory offsets of the fields of a C structure"""

    return [getattr(type, field_name).offset for field_name, _ in type._fields_]


def get_size(type: ctypes.Structure) -> int:
    """Get the size of a C structure"""

    return ctypes.sizeof(type)


def _create_array_ctype(element_ctype: type, array_info: annotations.ArrayAnnotation):
    if issubclass(element_ctype, csequence.Sequence):
        raise TypeError(
            f'Arrays of sequences are not supported: wrap the sequence in a @idl.alias or @idl.struct')

    return element_ctype * array_info.total_size


def _get_member_ctype(py_type, member_annotations):
    if reflection_utils.is_optional_type(py_type):
        underlying_type = reflection_utils.get_underlying_type(py_type)
        underlying_ctype = _get_member_ctype(
            underlying_type, member_annotations)
        if reflection_utils.is_constructed_type(underlying_ctype):
            # Optimization: constructed types cache their pointer type, saving
            # the creation of a new ctypes.POINTER type.
            return py_type.type_support.c_type_ptr
        elif underlying_type is not str:
            return ctypes.POINTER(underlying_ctype)
        else:
            # strings are already pointers; continue as if it wasn't optional
            py_type = underlying_type

    basic_type = PRIMITIVE_TO_CTYPES_MAP.get(py_type, None)
    if basic_type is not None:
        return basic_type

    if py_type is str:
        encoding = annotations.find_annotation(
            member_annotations, annotations.CharEncodingAnnotation)
        c_str_type = STR_ENCODING_TO_CTYPES_MAP.get(encoding.value, None)
        if c_str_type is None:
            raise TypeError(f'Unsupported char encoding {encoding.value}')
        return c_str_type

    if reflection_utils.is_sequence_type(py_type):
        element_annotations = annotations.find_annotation(
            member_annotations, annotations.ElementAnnotations)
        element_ctype = _get_member_ctype(reflection_utils.get_underlying_type(py_type),
                                          element_annotations.value)

        array_info = annotations.find_annotation(
            member_annotations, annotations.ArrayAnnotation)

        if array_info.is_array:
            return _create_array_ctype(element_ctype, array_info)
        elif reflection_utils.is_constructed_type(element_ctype):
            # Optimization: constructed types cache their sequence type
            return py_type.type_support.c_type_seq
        else:
            return csequence.create_sequence_type(element_ctype)

    if reflection_utils.is_constructed_type(py_type):
        return py_type.type_support.c_type

    raise TypeError(f'Unsupported member type {py_type}')


def create_ctype_from_dataclass(py_type, member_annotations):
    """Given a user-level dataclass, return an equivalent ctypes type that
    can be used by the C interpreter
    """

    if reflection_utils.is_enum(py_type):
        return ctypes.c_int32

    c_fields = []
    for field in fields(py_type):
        current_annotations = member_annotations.get(field.name, {})
        member_type = _get_member_ctype(field.type, current_annotations)
        c_fields.append((field.name, member_type))

    return type(
        py_type.__name__ + 'Native',
        (ctypes.Structure,),
        {
            '__doc__': f'Equivalent C type for {py_type.__name__}',
            '_fields_': c_fields
        }
    )


def create_ctype_from_union_dataclass(py_type, discriminator, cases, member_annotations):
    """Given a user-level dataclass reprenting a union, return an equivalent
    ctypes type that can be used by the C interpreter
    """

    # Discriminator
    current_annotations = member_annotations.get(discriminator.name, {})
    member_type = _get_member_ctype(discriminator.type, current_annotations)
    c_fields = [(discriminator.name, member_type)]

    # Cases
    for field in cases:
        current_annotations = member_annotations.get(field.name, {})
        member_type = _get_member_ctype(
            reflection_utils.remove_classvar(field.type),
            current_annotations)
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


PY_TYPE_TO_DYNAMIC_TYPE_MAP = {
    bool: dds.BoolType(),
    type_hints.int8: dds.Int8Type(),
    type_hints.int16: dds.Int16Type(),
    type_hints.uint16: dds.Uint16Type(),
    type_hints.int32: dds.Int32Type(),
    type_hints.uint32: dds.Uint32Type(),
    type_hints.int64: dds.Int64Type(),
    type_hints.uint64: dds.Uint64Type(),
    type_hints.float32: dds.Float32Type(),
    type_hints.float64: dds.Float64Type(),
    type_hints.char: dds.CharType(),
    type_hints.wchar: dds.WcharType()
}

def get_idl_base_type(idl_type: type) -> Optional[bool]:
    base = idl_type.__bases__[0] # will be object if no explicit base
    return base if hasattr(base, 'type_support') else None

def _get_member_dynamic_type(py_type, member_annotations, type_factory: dds._GenericTypePluginFactory):
    if reflection_utils.is_optional_type(py_type):
        underlying_type = reflection_utils.get_underlying_type(py_type)
        return _get_member_dynamic_type(underlying_type, member_annotations, type_factory)

    basic_type = PY_TYPE_TO_DYNAMIC_TYPE_MAP.get(py_type, None)
    if basic_type is not None:
        return basic_type

    if py_type is str:
        encoding = annotations.find_annotation(
            member_annotations, annotations.CharEncodingAnnotation)
        bound = annotations.find_annotation(
            member_annotations, annotations.BoundAnnotation)
        if encoding.value == annotations.CharEncoding.UTF8:
            return dds.StringType(bound.value)
        elif encoding.value == annotations.CharEncoding.UTF16:
            return dds.WStringType(bound.value)
        else:
            raise TypeError(f'Unsupported char encoding {encoding.value}')

    if reflection_utils.is_sequence_type(py_type):
        member_type = reflection_utils.get_underlying_type(py_type)
        element_annotations = annotations.find_annotation(
            member_annotations, annotations.ElementAnnotations)
        member_dynamic_type = _get_member_dynamic_type(
            member_type, element_annotations.value, type_factory)
        array_info = annotations.find_annotation(
            member_annotations, annotations.ArrayAnnotation)
        if array_info.is_array:
            # TODO PY-17: Use array_info.dimensions
            return type_factory.create_array(
                member_dynamic_type, array_info.total_size)
        else:
            bound = annotations.find_annotation(
                member_annotations, cls=annotations.BoundAnnotation)
            return type_factory.create_sequence(
                member_dynamic_type, bound.value)

    if reflection_utils.is_constructed_type(py_type):
        if type_factory is _internal_type_factory:
            return py_type.type_support._dynamic_type_ref
        else:
            return py_type.type_support.dynamic_type

    raise TypeError(f'Unsupported member type {py_type}')


def create_dynamic_type_from_dataclass(
    py_type: type,
    c_type: ctypes.Structure,
    type_annotations: List[Any],
    member_annotations: Dict[str, Any],
    is_public_type: bool = False
) -> dds._TypePlugin:
    """Given an IDL-derived dataclass, return the DynamicType that describes
    the IDL type
    """

    extensibility = annotations.find_annotation(
        type_annotations, annotations.ExtensibilityAnnotation)

    if member_annotations is None:
        member_annotations = {}

    # The DynamicType does not include the fields of the base type.
    #
    # We only check if the first base is an IDL type; we ignore other base types
    # for flexibility in the type definition. If the type has a base but it's
    # not an IDL type we don't split the DynamicType; we process all the fields.
    actual_fields = fields(py_type)
    c_type_offsets = get_offsets(c_type)
    base_type = get_idl_base_type(py_type)
    if base_type is not None:
        base_field_count = len(fields(base_type))
        actual_fields = actual_fields[base_field_count:]
        c_type_offsets = c_type_offsets[base_field_count:]

    type_factory = _get_type_factory(is_public_type)
    member_args = []
    for field in actual_fields:
        current_annotations = member_annotations.get(field.name, {})
        is_optional = reflection_utils.is_optional_type(field.type)
        member_type = _get_member_dynamic_type(
            field.type, current_annotations, type_factory)

        is_key = annotations.find_annotation(
            current_annotations, cls=annotations.KeyAnnotation)
        member_id = annotations.find_annotation(
            current_annotations, cls=annotations.IdAnnotation)

        member_args.append(
            (field.name, member_type, member_id.value, is_key.value, is_optional, False))

    # Create the struct type only after all the members have been parsed. The
    # type_factory requires that dependent types are created first
    if base_type is None:
        dynamic_type = type_factory.create_struct(
            py_type.__name__, extensibility.value, get_size(c_type), c_type_offsets)
    else:
        base_type = base_type.type_support._dynamic_type_ref if not is_public_type else base_type.type_support.dynamic_type
        dynamic_type = type_factory.create_struct(
            py_type.__name__, base_type, extensibility.value, get_size(c_type), c_type_offsets)

    for member_arg in member_args:
        type_factory.add_member(dynamic_type, *member_arg)

    # Once finalized the type creation, this creates the plugin and assigns it
    # to dynamic_type
    if not is_public_type:
        type_factory.create_type_plugin(dynamic_type)

    return dynamic_type


def create_dynamic_type_from_union_dataclass(
    py_type: type,
    c_type: ctypes.Structure,
    discriminator,
    cases,
    type_annotations: List[Any],
    member_annotations: Dict[str, Any],
    is_public_type: bool = False
) -> dds._TypePlugin:
    """Given an IDL-derived dataclass representing a union, return the
    DynamicType that describes the IDL type
    """

    extensibility = annotations.find_annotation(
        type_annotations, annotations.ExtensibilityAnnotation)

    if member_annotations is None:
        member_annotations = {}

    type_factory = _get_type_factory(is_public_type)
    member_args = []
    for field in cases:
        current_annotations = member_annotations.get(field.name, {})
        member_type = _get_member_dynamic_type(
            reflection_utils.remove_classvar(field.type),
            current_annotations,
            type_factory)

        member_id = annotations.find_annotation(
            current_annotations, cls=annotations.IdAnnotation)
        case: Case = field.default

        # TODO PY-17: default label not supported yet
        member_args.append(
            (field.name, member_type, case.labels, member_id.value, False))

    current_annotations = member_annotations.get(discriminator.name, {})
    discriminator_type = member_type = _get_member_dynamic_type(
        discriminator.type, current_annotations, type_factory)

    # Create the type only after all the members have been parsed. The
    # type_factory requires that dependent types are created first
    dynamic_type = type_factory.create_union(
        py_type.__name__,
        discriminator_type,
        extensibility.value,
        get_size(c_type),
        get_offsets(c_type))

    for member_arg in member_args:
        type_factory.add_union_member(dynamic_type, *member_arg)

    # Once finalized the type creation, this creates the plugin and assigns it
    # to dynamic_type
    if not is_public_type:
        type_factory.create_type_plugin(dynamic_type)

    return dynamic_type

def create_dynamic_type_from_alias_dataclass(
    py_type: type,
    c_type: ctypes.Structure,
    member_annotations: Dict[str, Any],
    is_public_type: bool = False
) -> dds._TypePlugin:
    """Given an @idl.alias-decorated dataclass, return the DynamicType that
    describes the IDL type
    """

    if member_annotations is None:
        member_annotations = {}

    type_fields = fields(py_type)
    if len(type_fields) != 1:
        raise TypeError(
            f'An @idl.alias must have exactly one field, but {py_type} has {len(type_fields)}')

    field = type_fields[0]
    current_annotations = member_annotations.get(field.name, {})
    is_key = annotations.find_annotation(
        current_annotations, cls=annotations.KeyAnnotation)
    if is_key.value:
        raise TypeError(f'An @idl.alias cannot be a key')
    if reflection_utils.is_optional_type(field.type):
        raise TypeError(f'An @idl.alias cannot be optional')

    type_factory = _get_type_factory(is_public_type)
    member_type = _get_member_dynamic_type(
        field.type, current_annotations, type_factory)
    dynamic_type = type_factory.create_alias(
        py_type.__name__, member_type, get_size(c_type))

    # Once finalized the type creation, this creates the plugin and assigns it
    # to dynamic_type
    if not is_public_type:
        type_factory.create_type_plugin(dynamic_type)

    return dynamic_type


def create_dynamic_type_from_enum(py_type, type_annotations=None):
    """Given an IDL-derived enum, return the DynamicType that describes
    the IDL type
    """

    extensibility = annotations.find_annotation(
        type_annotations, annotations.ExtensibilityAnnotation)

    enum_values: List[dds.EnumMember] = []
    for enum_value in py_type:
        enum_values.append(dds.EnumMember(enum_value.name, enum_value.value))

    return _internal_type_factory.create_enum(py_type.__name__, extensibility.value, enum_values)

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

class TypeSupportKind(Enum):
    STRUCT = 1
    UNION = 2
    ENUM = 3
    ALIAS = 4


class TypeSupport:
    """Support and utility methods for the usage of an IDL type"""

    def __init__(
        self,
        idl_type, kind: TypeSupportKind,
        type_annotations = None,
        member_annotations=None,
        sample_program_options: Optional[sample_interpreter.SampleProgramOptions]=None
    ):
        self.type = idl_type

        # For all types except unions, the object factory for deserialization
        # simply invokes the type's constructor with no arguments, which
        # in the case of a struct initializes each field to its default value.
        self.kind = kind
        self.default_factory = idl_type
        self.allow_duck_typing = False
        self._public_dynamic_type = None # created lazily by property
        base_type = get_idl_base_type(idl_type)
        if base_type is not None:
            type_annotations = annotations.inherit_type_annotations(
                base_type.type_support.type_annotations, type_annotations)
            member_annotations = annotations.inherit_member_annotations(
                base_type.type_support.member_annotations, member_annotations)

        self.type_annotations = type_annotations
        self.member_annotations = member_annotations

        # C type and plugin and dynamic type
        is_union = False
        if kind == TypeSupportKind.ENUM:
            self.c_type = ctypes.c_int32
        elif kind == TypeSupportKind.ALIAS:
            self.c_type = create_ctype_from_dataclass(
                idl_type, member_annotations)
        elif kind == TypeSupportKind.STRUCT:
            self.c_type = create_ctype_from_dataclass(
                idl_type, member_annotations)
        elif kind == TypeSupportKind.UNION:
            is_union = True

            # When creating a union for deserialization, don't set assign a
            # default object to the "value" field. The deserialization will
            # do it right after.
            self.default_factory = lambda: idl_type(None)
            discriminator = union_discriminator(idl_type)
            cases = union_cases(idl_type)
            self.c_type = create_ctype_from_union_dataclass(
                idl_type, discriminator, cases, member_annotations)
        else:
            raise ValueError(f'Unknown type kind {kind}')

        self._plugin_dynamic_type = self._create_dynamic_type(is_public=False)

        # Pointer and sequence types
        self.c_type_ptr = ctypes.POINTER(self.c_type)
        self.c_type_seq = csequence.create_sequence_type(self.c_type)

        # Python/C sample conversion programs
        self._sample_programs = sample_interpreter.SamplePrograms(
            py_type=self.type,
            c_type=self.c_type,
            type_plugin=self._plugin_dynamic_type,
            member_annotations=member_annotations,
            is_union=is_union,
            options=sample_program_options)

    def _create_dynamic_type(self, is_public: bool):
        if self.kind == TypeSupportKind.ENUM:
            return create_dynamic_type_from_enum(
                self.type, self.type_annotations)
        elif self.kind == TypeSupportKind.ALIAS:
            return create_dynamic_type_from_alias_dataclass(
                self.type, self.c_type, self.member_annotations, is_public)
        elif self.kind == TypeSupportKind.STRUCT:
            return create_dynamic_type_from_dataclass(
                self.type, self.c_type, self.type_annotations, self.member_annotations, is_public)
        elif self.kind == TypeSupportKind.UNION:
            discriminator = union_discriminator(self.type)
            cases = union_cases(self.type)
            return create_dynamic_type_from_union_dataclass(
                self.type, self.c_type, discriminator, cases, self.type_annotations, self.member_annotations, is_public)
        else:
            raise ValueError(f'Unknown type kind {self.kind}')

    def _create_c_sample(self, sample):
        if not self.allow_duck_typing and type(sample) is not self.type:
            raise TypeError(
                f'Expected type {self.type}, got {type(sample)}')

        c_sample = self.c_type()
        self._plugin_dynamic_type.initialize_sample(c_sample)
        try:
            self._sample_programs.py_to_c_program.execute(
                src=sample, dst=c_sample)
        except:
            self._plugin_dynamic_type.finalize_sample(c_sample)
            raise
        return c_sample

    def _create_empty_c_sample(self):
        c_sample = self.c_type()
        self._plugin_dynamic_type.initialize_sample(c_sample)
        return c_sample

    def _convert_to_c_sample(self, c_sample, py_sample):
        self._plugin_dynamic_type.finalize_optional_members(c_sample)
        self._sample_programs.py_to_c_program.execute(
            src=py_sample, dst=c_sample)

    def _cast_c_sample(self, c_sample_ptr):
        return ctypes.cast(c_sample_ptr, self.c_type_ptr)[0]

    def _create_py_sample(self, c_sample_ptr):
        py_sample = self.default_factory()
        c_sample = self._cast_c_sample(c_sample_ptr)
        self._sample_programs.c_to_py_program.execute(
            src=c_sample, dst=py_sample)
        return py_sample

    def _create_py_sample_no_ptr(self, c_sample):
        py_sample = self.default_factory()
        self._sample_programs.c_to_py_program.execute(
            src=c_sample, dst=py_sample)
        return py_sample

    def serialize(self, sample):
        """Serialize a data sample into a cdr byte buffer"""

        c_sample = self._create_c_sample(sample)
        try:
            return self._plugin_dynamic_type.serialize(c_sample)
        finally:
            self._plugin_dynamic_type.finalize_sample(c_sample)

    def deserialize(self, buffer) -> Any:
        """Create a data sample for the data type for this TypeSupport by
        deserializing a cdr byte buffer
        """

        c_sample = self.c_type()
        self._plugin_dynamic_type.initialize_sample(c_sample)
        try:
            self._plugin_dynamic_type.deserialize(c_sample, buffer)
            return self._create_py_sample_no_ptr(c_sample)
        finally:
            self._plugin_dynamic_type.finalize_sample(c_sample)

    def get_serialized_sample_size(self, sample) -> int:
        """Returns the serialized size of a given sample
        """
        c_sample = self._create_c_sample(sample)
        try:
            return self._plugin_dynamic_type.serialized_sample_size(c_sample)
        finally:
            self._plugin_dynamic_type.finalize_sample(c_sample)

    @property
    def max_serialized_sample_size(self) -> int:
        """Returns the max serialized size of a given type"""
        return self._plugin_dynamic_type.max_serialized_sample_size()

    def to_dynamic_data(self, sample: Any) -> dds.DynamicData:
        """Converts a given idl sample to dynamic data"""
        dynamic_type = self._get_public_dynamic_type().get_dynamic_type_ref()
        return dds.DynamicData(dynamic_type).from_cdr_buffer(self.serialize(sample))

    def from_dynamic_data(self, sample: dds.DynamicData) -> Any:
        """Converts a given dynamic data sample to an idl type"""
        return self.deserialize(sample.to_cdr_buffer())

    @property
    def _dynamic_type_ref(self):
        return self._plugin_dynamic_type.get_dynamic_type_ref()

    def _get_public_dynamic_type(self):
        if self._public_dynamic_type is None:
            self._public_dynamic_type = self._create_dynamic_type(is_public=True)

        return self._public_dynamic_type

    @property
    def dynamic_type(self) -> dds.DynamicType:
        """Returns a copy of the DynamicType that describes the IDL type"""

        return self._get_public_dynamic_type().clone_type()

    @property
    def is_valid_topic_type(self):
        kind = self._dynamic_type_ref.kind
        return  kind == dds.TypeKind.STRUCTURE_TYPE or kind == dds.TypeKind.UNION_TYPE
