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
import rti.connextdds.heap_monitoring as heap_monitoring
import rti.connextdds as dds
import rti.idl as idl
import os
import gc

#
# This module is ran by conftest.py to detect leaks after all tests run
#

HEAP_USAGE_FILENAME = "test_heap_snapshot.txt"

def get_heap_usage_from_heap_snapshot(file: str) -> int:
    """Get the current heap usage from a heap snapshot file."""

    with open(file, "r") as f:
        for line in f:
            if line.startswith("Current application heap usage:"):
                return int(line.split(":")[1].strip())

def check_leaks() -> None:
    """Finalize all global memory and check for leaks
    
    Make the tests fail if any are found.
    """

    # Here we must manually invoke the gc so that we ensure everything is tidy
    # before taking the heap snapshot. While in most cases Python reclaims 
    # objects immediately after they become unreachable (similarly to a C++ 
    # shared_ptr), the GC still has to periodically check for objects that could
    # be part of a reference cycle. If one of those outstanding objects is a 
    # native C DDS object, the leak checker would report a leak.
    gc.collect()
    # Finalize global state
    idl.finalize_globals()
    dds.DomainParticipant.finalize_participant_factory()

    # Take snapshot when all native heap memory should've been freed
    heap_monitoring.take_snapshot(HEAP_USAGE_FILENAME, print_details=True)
    heap_usage = get_heap_usage_from_heap_snapshot(HEAP_USAGE_FILENAME)
    if heap_usage > 0:
        raise Exception(
            f"*** MEMORY LEAK detected *** heap usage after last test is {heap_usage} - see {HEAP_USAGE_FILENAME}")

    # Delete the snapshot file if there're no leaks
    os.remove(HEAP_USAGE_FILENAME)

