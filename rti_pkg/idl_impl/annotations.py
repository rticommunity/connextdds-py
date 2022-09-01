
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


def get_total_size_from_dimensions(dimensions: Union[None, int, List[int]]) -> int:
    if dimensions is None:
        return 1
    elif isinstance(dimensions, list) or isinstance(dimensions, tuple):
        return reduce(lambda x, y: int(x) * int(y), dimensions)
    else:
        return int(dimensions)

@dataclass
class ArrayAnnotation:
    dimensions: Union[None, int, List[int]] = None

    @property
    def is_array(self) -> bool:
        return self.dimensions is not None

    @property
    def total_size(self) -> int:
        return get_total_size_from_dimensions(self.dimensions)

    @property
    def dimension_list(self) -> dds.Uint32Seq:
        if self.dimensions is None:
            return dds.Uint32Seq()
        elif isinstance(self.dimensions, list) or isinstance(self.dimensions, tuple):
            return dds.Uint32Seq([int(x) for x in self.dimensions])
        else:
            return dds.Uint32Seq([int(self.dimensions)])

class CharEncoding(IntEnum):
    UTF8 = 0
    UTF16 = 1


@dataclass
class CharEncodingAnnotation:
    value: CharEncoding = CharEncoding.UTF8


class AllowedDataRepresentationFlags(IntEnum):
    XCDR1 = 0x01
    XCDR2 = 0x04


@dataclass
class AllowedDataRepresentationAnnotation:
    value: AllowedDataRepresentationFlags = AllowedDataRepresentationFlags.XCDR1 \
        | AllowedDataRepresentationFlags.XCDR2

@dataclass
class ElementAnnotations:
    value: List[Any] = field(default_factory=list)

def find_annotation(annotations, cls, default=None):
    if default is None:
        default = cls()

    if annotations is None:
        return default

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
