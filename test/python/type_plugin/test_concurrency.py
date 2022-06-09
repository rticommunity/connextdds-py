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

from threading import Thread

from test_utils.fixtures import *

# We use a very complex type with sequences and optionals to increase the
# chances to detect any unwanted race condition in the type support
from test_sequences_of_sequences import SequenceTest, create_sequence_sample


# A dds.DataWriter uses a scratchpad ctypes sample in each write. This object
# has to be in a exclusive area so that parallel writes don't use it at the
# same time.
def test_concurrent_write(shared_participant):
    fixture = PubSubFixture(shared_participant, SequenceTest)
    def write_thread(index: int):
        samples = [create_sequence_sample((i + index) % 4) for i in range(100)]
        for sample in samples:
            fixture.writer.write(sample)

    threads = [Thread(target=write_thread, args=(i,)) for i in range(15)]
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()

# TODO PY-17: add test where write() blocks.
