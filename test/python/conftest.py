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

# This fixture ensures that at the end of the last test we run
# leak_detector.check_leaks
@pytest.fixture(scope="session", autouse=True)
def leak_detection_fixture(request):
    request.addfinalizer(leak_detector.check_leaks)
