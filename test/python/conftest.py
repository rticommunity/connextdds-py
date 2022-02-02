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

import pytest
import leak_detector
import rti.connextdds.heap_monitoring as heap_monitoring

# Enable heap monitoring for all tests. Heap monitoring is used to detect
# memory leaks by the fixture below

heap_monitoring.enable()

def pytest_sessionfinish(session, exitstatus):
    # Disable heap monitoring after all tests run
    #
    # Note: running this as a pytest hook instead of a fixture ensures that
    # the leak detector runs after all other fixtures have been deleted.
    if exitstatus == 1: # Test failures
        try:
            leak_detector.check_leaks()
        except Exception as e:
            # If a test has already failed and there is also a leak, print the
            # leak error message, but don't throw an exception that makes the
            # output confusing
            print(f"\n{e}\n")
    else:
        # If tests have been succesful, let check_leaks() throw an exception to
        # make the process fail
        leak_detector.check_leaks()
