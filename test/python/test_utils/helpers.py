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

import rti.idl as idl
from contextlib import contextmanager


@contextmanager
def type_support_with_duck_typing(type):
    """Context manager that returns the type support for a given type after
    setting allow_duck_typing to True, and restores the value when exiting the
    context.
    """

    ts = idl.get_type_support(type)
    previous_value = ts.allow_duck_typing
    ts.allow_duck_typing = True
    yield ts
    ts.allow_duck_typing = previous_value
