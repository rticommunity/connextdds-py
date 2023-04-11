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

from typing import Optional

# A simple dictionary used by dds.DomainParticipant.register_idl_type to keep
# track of registered IDL-based Python types (and their TypeSupport)

_registered_types = {}

def register_type(type_name: str, type_class: type):
    _registered_types[type_name] = type_class

def get_type(type_name: str) -> Optional[type]:
    return _registered_types.get(type_name)
