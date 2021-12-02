 #
 # (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 #
 # RTI grants Licensee a license to use, modify, compile, and create derivative
 # works of the Software solely for use with RTI products.  The Software is
 # provided "as is", with no warranty of any type, including any warranty for
 # fitness for any purpose. RTI is under no obligation to maintain or support
 # the Software.  RTI shall not be liable for any incidental or consequential
 # damages arising out of the use or inability to use the software.
 #

import rti.connextdds as dds
from test_utils import wait, fixtures

def test_data_reader_writer():
    fx = fixtures.PubSubFixture(0, dds.StringTopicType)

    # TODO PY-17: writer.write("Hello") doesn't work at the moment because
    # of the ordering of write functions in init_dds_datawriter_untyped_methods.
    for i in range(1, 6):
        fx.writer.write(dds.StringTopicType("Hello World " + str(i)))

    wait.for_data(fx.reader, 5)

    count = 0
    with fx.reader.read() as samples:
        assert len(samples) == 5
        for x, _ in samples:
            count += 1
            assert x == f"Hello World {count}"
