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


from contextlib import contextmanager
import rti.connextdds as dds

import pytest

"""Utilities to check for error messages in DDS logs and exceptions"""

class ErrorInfo:
    """Provides information about an error, whether it is a log message or an
    exception
    """

    def __init__(self):
        self.messages = []
        self.excinfo = None

    def append(self, message: str):
        self.messages.append(message)

    @property
    def logs(self) -> str:
        """Returns the log messages that were captured"""
        return "\n".join(self.messages)

    @property
    def exception_msg(self) -> str:
        """Returns the exception message, if any"""
        if self.excinfo is None:
            return None

        return str(self.excinfo.value)

    def __str__(self):
        return "\n".join(self.messages)


@contextmanager
def expected_errors():
    """Context manager that captures log messages when an exception is not
    expected
    """

    errors = ErrorInfo()
    def append_msg(msg):
        nonlocal errors
        errors.append(msg)

    dds.Logger.instance.output_handler(append_msg)
    try:
        yield errors
    finally:
        assert len(errors.messages) > 0, "No error messages were captured"
        dds.Logger.instance.reset_output_handler()


@contextmanager
def expected_exception(expected_exception_type):
    """Context manager that captures log messages and an exception when both
    an exception and a separate log message are expected
    """

    with expected_errors() as errors:
        with pytest.raises(expected_exception_type) as excinfo:
            yield errors
        errors.excinfo = excinfo

