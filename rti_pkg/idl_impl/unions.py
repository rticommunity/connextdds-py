
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


from dataclasses import fields, dataclass, Field, MISSING
from typing import ClassVar

DEFAULT_LABEL = 0x40000001

class Case:
    """A field descriptor that enforces that the valid discriminator for a union
    case is preserved
    """
    def __init__(self, label):
        self.label = label

    def __get__(self, obj, objtype=None):
        if obj is None:
            return self
        if obj.discriminator != self.label:
            raise ValueError(
                f"Union field not selected by current discriminator ({obj.discriminator})")
        return obj.value

    def __set__(self, obj, value):
        obj.discriminator = self.label
        obj.value = value

    @property
    def labels(self):
        return [self.label]

    @property
    def single_label(self):
        return self.label


class MultiCase(Case):
    """A field descriptor that enforces that the valid discriminator for a union
    case is preserved when the case has more than one label
    """
    def __get__(self, obj, objtype=None):
        if obj is None:
            return self
        if obj.discriminator not in self.label:
            raise ValueError(
                f"Union field not selected by current discriminator ({obj.discriminator})")
        return obj.value

    def __set__(self, obj, value):
        if isinstance(value, tuple):
            if value[1] not in self.label:
                raise ValueError(
                    f"Invalid discriminator ({value[1]}) for this union field, expected one of {self.label}")
            obj.discriminator = value[1]
            obj.value = value[0]
        else:
            obj.discriminator = self.label[0]
            obj.value = value

    @property
    def labels(self):
        return self.label

    @property
    def single_label(self):
        return self.label[0]

class DefaultCase(MultiCase):
    """A field descriptor that enforces that the valid discriminator for the
    "default" union case
    """
    def __init__(self, label):
        super().__init__(label)
        self.excluded_labels = [] # to be set after parsing the whole union

    def __get__(self, obj, objtype=None):
        if obj is None:
            return self
        if obj.discriminator in self.excluded_labels:
            raise ValueError(
                f"Union field not selected by current discriminator ({obj.discriminator})")
        return obj.value

    def __set__(self, obj, value):
        if isinstance(value, tuple):
            if value[1] in self.excluded_labels:
                raise ValueError(
                    f"Invalid discriminator ({value[1]}) for default union field, discriminator must be different from {self.excluded_labels}")
            obj.discriminator = value[1]
            obj.value = value[0]
        else:
            obj.discriminator = obj.default_discriminator
            obj.value = value

    @property
    def labels(self):
        return self.label + [DEFAULT_LABEL]


def _raise_missing_discr_or_value_fields():
    raise TypeError(
        "Invalid @idl.union type: a discriminator field and a value field are required")

def union_discriminator(union_type) -> Field:
    flds = fields(union_type)
    if len(flds) < 2:
        _raise_missing_discr_or_value_fields()

    if flds[0].name != 'discriminator':
        _raise_missing_discr_or_value_fields()

    return flds[0]

def union_value_field(union_type) -> Field:
    flds = fields(union_type)
    if len(flds) < 2:
        _raise_missing_discr_or_value_fields()

    if flds[1].name != 'value':
        _raise_missing_discr_or_value_fields()

    return flds[1]

def union_cases(union_type):
    # Filter all dataclass fields whose assigned value is a union Case field
    # descriptor (note that the dataclasses.fields function not return the union
    # field descriptors, so we have to look at __dataclass_fields__ directly)
    result = [field for field
        in union_type.__dataclass_fields__.values()
        if isinstance(field.default, Case)]

    if len(result) == 0:
        raise TypeError(
            "Invalid @idl.union type: at least one case is required")

    return result


def _add_init_method(union_type):
    """Adds a __init__ method that sets the right discriminator for the value
    that has been used to create the union.
    """
    def init_union(union_instance, *args, **kwargs):
        if len(args) == 0 and len(kwargs) == 0:
            t = type(union_instance)
            union_instance.discriminator = t.default_discriminator
            union_instance.value = t.default_value()
            return

        if len(args) == 1 and args[0] is None:
            union_instance.discriminator = 0
            union_instance.value = None
            return

        if len(kwargs) == 1:
            arg = list(kwargs.items())[0]
            if not hasattr(union_type, arg[0]):
                raise TypeError(f"__init__() got an unexpected keyword argument '{arg[0]}'")
            setattr(union_instance, arg[0], arg[1])
            return

        raise TypeError(
            "Union constructor expects either None or a single named argument identifying the union case")

    union_type.__init__ = init_union

def _configure_union_cases(union_type):

    if (len(union_type.__annotations__) < 2):
        _raise_missing_discr_or_value_fields()

    # skip the first two items (discriminator and value)
    items = iter(union_type.__annotations__.items())
    next(items)
    next(items)
    for name, type in items:
        union_type.__annotations__[name] = ClassVar[type]

def _configure_default_case(union_type):
    cases = union_cases(union_type)

    # Find the default case, if any
    default_case = None
    for case in cases:
        if isinstance(case.default, DefaultCase):
            if default_case is not None:
                raise TypeError("Only one default case is allowed per union")
            default_case = case

    # Gather all labels of the non-default cases in the default case's
    # excluded_labels
    if default_case is not None:
        for case in cases:
            if case is not default_case:
                default_case.default.excluded_labels += case.default.labels

    return default_case

def _configure_default_factory(union_type):
    discr_field = union_discriminator(union_type)
    if discr_field.default is MISSING:
        raise TypeError("A default value for the discriminator is required")
    union_type.default_discriminator = discr_field.default

    value_field = union_value_field(union_type)
    if value_field.default is not MISSING:
        union_type.default_value = lambda: value_field.default
    elif value_field.default_factory is not MISSING:
        union_type.default_value = value_field.default_factory
    else:
        raise TypeError("No default value or factory specified for union value")

def configure_union_class(union_type):
    """Processes an @idl.union-decorated class to create the fields and methods
    that allow using the class as a union, where only one value can be set at
    a time, and the discriminator identifies the selected case.
    """

    # Add a init method that can receive the named cases as arguments. By doing
    # this at the beginning the dataclass() conversion will not add a init
    # method, which would receive the discriminator and value as arguments.
    _add_init_method(union_type)

    # Turn all fields after the discriminator and value into ClassVars so that
    # the dataclass() conversion ignores them
    _configure_union_cases(union_type)

    # Create the dataclass, which will only take into account the discr and
    # value fields, but won't add a init method
    dataclass(union_type)

    # Configure the default case setter/getter, if any, taking into account the
    # rest of cases as invalid discriminators for the default case
    _configure_default_case(union_type)

    # Set two class-level fields: a default_discriminator and a default_value
    # factory
    _configure_default_factory(union_type)

    return union_type