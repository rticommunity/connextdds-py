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

import pathlib
import pytest
import rti.connextdds as dds


@pytest.fixture(scope="module")
def types():
    class TestTypes:
        def __init__(self):
            types_file = str(pathlib.Path(__file__).parent.absolute()) + \
                "/../../xml/PerformanceTester.xml"
            self.provider = dds.QosProvider(types_file)

        def __getitem__(self, key):
            return self.provider.type(key)

    types = TestTypes()
    yield types

    # This ensures that the native memory is released. Otherwise pytest may
    # hold a reference to the provider after the tests end, leading to a
    # reported leak by the heap monitoring.
    types.provider = None
