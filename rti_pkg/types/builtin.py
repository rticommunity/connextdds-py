
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

from dataclasses import field
from typing import Sequence
from rti.idl_impl.decorators import struct as _struct
from rti.idl_impl.annotations import KeyAnnotation as _KeyAnnotation
from rti.idl_impl.type_hints import uint8 as _octet
from rti.idl_impl.type_utils import array_factory as _array_factory


@_struct
class String:
    """Built-in type consisting of a string

    This type can be used to create a dds.Topic, e.g.
    ```
    topic = dds.Topic(participant, "My Topic", String)
    ```
    """
    value: str = ""
    """The string payload"""


@_struct(member_annotations={'key': [_KeyAnnotation(True)]})
class KeyedString:
    """Built-in type consisting of a string payload and a second string that is
    the key.

    This type can be used to create a dds.Topic, e.g.
    ``topic = dds.Topic(participant, "My Topic", KeyedString)``.
    """
    key: str = ""
    """The key"""
    value: str = ""
    """The string payload"""


@_struct
class Bytes:
    """Built-in type consisting of a sequence of bytes

    This type can be used to create a dds.Topic, e.g.
    ``topic = dds.Topic(participant, "My Topic", Bytes)``.
    """
    value: Sequence[_octet] = field(default_factory=_array_factory(_octet))
    """The byte payload"""


@_struct(member_annotations={'key': [_KeyAnnotation(True)]})
class KeyedBytes:
    """Built-in type consisting of a sequence of bytes and a string that is the
    key.

    This type can be used to create a dds.Topic, e.g.
    ``topic = dds.Topic(participant, "My Topic", KeyedBytes)``.
    """
    key: str = ""
    """The key"""
    value: Sequence[_octet] = field(default_factory=_array_factory(_octet))
    """The byte payload"""
