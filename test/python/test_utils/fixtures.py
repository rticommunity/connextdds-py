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
import rti.idl_impl.test_utils as test_utils


create_participant = test_utils.create_participant
PubSubFixture = test_utils.PubSubFixture
wait = test_utils.wait
get_test_domain = test_utils.get_test_domain
get_test_participant_qos = test_utils.get_test_participant_qos

# Provides a participant exclusively for the current test
@pytest.fixture
def participant():
    participant = create_participant()
    yield participant
    participant.close()

# Provides a participant that is shared among all the tests in a module that
# request this fixture
@pytest.fixture(scope="module")
def shared_participant_no_cleanup():
    participant = create_participant()
    yield participant
    participant.close()

# Provides the above participant but it delete its contained entities after each
# test
@pytest.fixture
def shared_participant(shared_participant_no_cleanup):
    yield shared_participant_no_cleanup
    shared_participant_no_cleanup.close_contained_entities()
