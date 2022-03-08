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


import rti.connextdds as dds
import time


class WaitError(Exception):
    def __init__(self, message: str = None):
        if message is None:
            super().__init__("Wait timeout")
        else:
            super().__init__("Wait timeout: " + str(message))


def until(predicate) -> None:
    max_wait = 10
    sleep_time = 0.1
    while not predicate() and max_wait > 0:
        time.sleep(sleep_time)
        max_wait -= sleep_time

    if max_wait <= 0:
        raise WaitError()


def until_equal(expected_value, function) -> None:
    try:
        until(lambda: expected_value == function())
    except WaitError:
        raise WaitError(f"Expected {expected_value}, got {function()}")


def for_discovery(reader: dds.DataReader, writer: dds.DataWriter) -> None:
    until(lambda: writer.instance_handle in reader.matched_publications)
    until(lambda: reader.instance_handle in writer.matched_subscriptions)


def for_data(reader: dds.DataReader, count: int = -1) -> None:
    if count < 0:
        # Wait for one or more samples
        until(lambda: reader.datareader_cache_status.sample_count > 0)
    else:
        # Wait for a specific number
        until_equal(count, lambda: reader.datareader_cache_status.sample_count)

