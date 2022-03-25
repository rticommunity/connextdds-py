
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

from dataclasses import dataclass, field
from enum import IntEnum
from typing import List, Union, Any
from functools import reduce
import rti.connextdds as dds

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


UNBOUNDED = 0x7FFFFFFF


@dataclass
class BoundAnnotation:
    value: int = UNBOUNDED


@dataclass
class ArrayAnnotation:
    dimensions: Union[None, int, List[int]] = None

    @property
    def is_array(self) -> bool:
        return self.dimensions is not None

    @property
    def is_single_dimension(self) -> bool:
        return isinstance(self.dimensions, int)

    @property
    def total_size(self) -> int:
        if self.is_single_dimension:
            return self.dimensions
        return reduce(lambda x, y: x * y, self.dimensions)

class CharEncoding(IntEnum):
    UTF8 = 0
    UTF16 = 1


@dataclass
class CharEncodingAnnotation:
    value: CharEncoding = CharEncoding.UTF8

@dataclass
class ElementAnnotations:
    value: List[Any] = field(default_factory=list)

def find_annotation(annotations, cls, default=None):
    if default is None:
        default = cls()

    for annotation in annotations:
        if isinstance(annotation, cls):
            return annotation
    return default


def inherit_type_annotations(base_annotations: list, derived_annotations: list) -> list:
    """Combines the annotations of a base class and a derived class, throwing
    an exception if they are inconsistent
    """

    result = base_annotations.copy()
    for annotation in derived_annotations:
        base_annotation = find_annotation(base_annotations, type(annotation), default="")
        if base_annotation != "":
            if base_annotation != annotation:
                raise ValueError(
                    f"Inconsistent annotation in base class (base class: {base_annotation}, derived class: {annotation})")
        else:
            result.append(annotation)

    return result

def inherit_member_annotations(base_annotations: dict, derived_annotations: dict) -> dict:
    """Combines the two dictionaries of member annotations"""

    return {**base_annotations, **derived_annotations}
