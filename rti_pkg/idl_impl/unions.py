
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

from rti.idl_impl.reflection_utils import get_underlying_type
from dataclasses import fields

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


class DefaultCase(Case):
    """A field descriptor that enforces that the valid discriminator for the
    "default" union case
    """
    def __init__(self, default_label, excluded_labels):
        super().__init__(default_label)
        self.excluded_labels = excluded_labels

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
            obj.discriminator = self.label
            obj.value = value


def union_discriminator(union_type):
    flds = fields(union_type)
    if len(flds) < 2:
        raise TypeError(
            "Invalid @idl.union type: a discriminator field and a value field are required")

    return flds[0]

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
