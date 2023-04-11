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

import ctypes

DDS_Boolean = ctypes.c_byte

class DDS_SeqElementTypeAllocationParams_t(ctypes.Structure):
    _fields_ = [
        ("allocate_pointers", DDS_Boolean),
        ("allocate_optional_members", DDS_Boolean),
        ("allocate_memory", DDS_Boolean)
    ]


class DDS_SeqElementTypeDeallocationParams_t(ctypes.Structure):
    _fields_ = [
        ("delete_pointers", DDS_Boolean),
        ("delete_optional_members", DDS_Boolean),
    ]


class Sequence(ctypes.Structure):
    """
    Equivalent representation for a C DDS Sequence using ctypes. This is a
    base class that relies on create_sequence_type to reflectively create a
    valid sequence type with an element type"""

    _fields_ = [
        ("_contiguous_buffer", ctypes.c_void_p),
        ("_discontiguous_buffer", ctypes.c_void_p),
        ("_read_token1", ctypes.c_void_p),
        ("_read_token2", ctypes.c_void_p),
        ("_maximum", ctypes.c_uint32),
        ("_length", ctypes.c_uint32),
        ("_sequence_init", ctypes.c_int32),
        ("_owned", DDS_Boolean),
        ("_elementAllocParams", DDS_SeqElementTypeAllocationParams_t),
        ("_absolute_maximum", ctypes.c_uint32),
        ("_elementAllocParams", DDS_SeqElementTypeDeallocationParams_t),
    ]

    def __getitem__(self, index: int):
        elements = self.get_elements_ptr()
        return elements[index]

    def __setitem__(self, index: int, object):
        elements = self.get_elements_ptr()
        elements[index] = object

    def __iter__(self):
        for i in range(0, len(self)):
            yield self[i]

    def __len__(self) -> int:
        return self._length

    def __repr__(self) -> str:
        return "[" + ",".join(str(element) for element in self) + "]"

    def set_length(self, length: int):
        """
        Set the length of the sequence. This operation does not resize the
        sequence."""

        if length > self._maximum:
            raise ValueError(
                f"Native sequence capacity ({self._maximum}) is smaller than the requested length ({length})")

        self._length = length

    @property
    def capacity(self) -> int:
        return self._maximum

    @property
    def absolute_maximum(self) -> int:
        return self._absolute_maximum

    def get_elements_ptr(self):
        # Note that _element_type is not defined in this base class; it is
        # defined by the concrete subclasses created by create_sequence_type
        return ctypes.cast(self._contiguous_buffer,
                           ctypes.POINTER(self._element_type))

    def get_elements_raw_ptr(self):
        return int(self._contiguous_buffer)

    def copy_from_buffer(self, buffer, element_count: int):
        """
        Copy the data from the buffer into the sequence. The sequence must have
        a buffer large enough; this operation doesn't resize it."""

        if self._maximum < element_count:
            raise ValueError(
                f"Native sequence capacity ({self._maximum}) is not large enough to copy data: {element_count}")

        ctypes.memmove(self._contiguous_buffer, buffer,
                       element_count * self._element_size)
        self._length = element_count

def create_sequence_type(element_ctype):
    """Creates a concrete sequence type given the element type"""

    if issubclass(element_ctype, Sequence):
        raise TypeError(
            f'Sequences of sequences are not supported: wrap the inner sequence in a @idl.alias or @idl.struct')

    # The element_type is set as a class field (it's not an instance field,
    # since the type needs to maintain its exact C memory layout)
    return type(
        element_ctype.__name__ + 'Seq',
        (Sequence,),
        {
            '_element_type': element_ctype,
            '_element_size': ctypes.sizeof(element_ctype)
        }
    )
