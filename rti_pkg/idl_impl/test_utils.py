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

import typing
from dataclasses import fields
import rti.idl as idl
import rti.idl_impl.reflection_utils as reflection_utils
from rti.idl_impl.type_plugin import PrimitiveArrayFactory


# Set the maximum length a string can be from the generator
MAX_BOUNDS = 1000


class IdlValueGenerator:
    """Class for creating dummy values given an idl type"""

    def __init__(self, sample_type: type):
        """Constuctor takes a type and gets ready for the generating fn"""
        self.sample_type = sample_type
        self.dynamic_type = idl.get_type_support(sample_type).dynamic_type

    def create_test_data(self, sample_type: type = None, seed: int = 0) -> typing.Any:
        """Returns a valid value of the type given to the constructor.
        
        create_test_data takes a sample type (in most cases should be left 
        empty) and a seed.  This function is deterministic and will always 
        return the same value given the same type and seed.
        """
        if sample_type is None:
            sample_type = self.sample_type

        def generate_string_from_seed(bounds: int, seed: int) -> str:
            """Private local function used to generate string values"""
            if bounds > MAX_BOUNDS:
                bounds = MAX_BOUNDS
            if seed % 4 == 0:
                # return a single character, could be any printable ascii char
                return chr((seed % 94) + 33)
            elif seed % 4 == 1:
                return ""
            elif seed % 4 == 2:
                return "".join([chr((x % 94) + 33) for x in range(0, bounds)])
            else:
                return "".join([chr((x % 94) + 33) for x in range(0, seed % bounds)])

        def generate_value_from_seed(sample_type: type, member_type: type, field_name: str, field_factory, seed: int):
            """Recursive private function used to generate the values given a type"""
            if member_type is bool:
                return seed % 2 == 0
            elif reflection_utils.is_primitive(member_type):
                # return mod 128 so it fits in all types
                return member_type(seed % 128)
            elif member_type is str:
                bound = self._get_sequence_type_bounds(field_name)
                return generate_string_from_seed(bound, seed)
            elif reflection_utils.is_optional_type(member_type):
                # If the seed is even we will fill the optional type
                if seed % 2 == 0:
                    return generate_value_from_seed(
                        sample_type, reflection_utils.get_underlying_type(member_type), field_name, None, seed)
                else:
                    return None
            elif reflection_utils.is_sequence_type(member_type):
                # is either a list or primitive array
                bound = self._get_sequence_type_bounds(field_name)
                if bound == idl.unbounded.value:
                    length = seed
                else:
                    length = (seed % bound) + 1

                seq = [generate_value_from_seed(sample_type, reflection_utils.get_underlying_type(
                    member_type), "", None, seed + x) for x in range(0, length)]
                if type(field_factory) == PrimitiveArrayFactory:
                    arr = field_factory()
                    arr.fromlist(seq)
                    return arr
                else:
                    return seq
            elif reflection_utils.is_constructed_type(member_type):
                return self.create_test_data(member_type, seed)
            else:
                raise TypeError(
                    f"unknown type {str(member_type)} given to generator")

        value = sample_type()

        for field in fields(sample_type):
            setattr(value, field.name, generate_value_from_seed(
                sample_type, field.type, field.name, field.default_factory, seed))
            seed += 1
        return value

    def _get_sequence_type_bounds(self, seq_name: str):
        """Private member function used to determine a sequences max length"""
        # You must check that the type is a sequence before calling this
        return self.dynamic_type[seq_name].type.bounds
