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

import pytest
from test_utils.fixtures import *
import rti.idl as idl
import rti.connextdds as dds


def test_publisher_listener_can_be_set(shared_participant):
	listener = dds.NoOpPublisherListener()
	other_listener = dds.NoOpPublisherListener()
	sub = dds.Publisher(shared_participant, dds.PublisherQos(), listener)
	assert sub.listener == listener

	sub.listener = other_listener
	assert sub.listener == other_listener

	sub.set_listener(None, dds.StatusMask.NONE)
	assert sub.listener is None

	sub.listener = dds.NoOpPublisherListener()
	assert type(sub.listener) == dds.NoOpPublisherListener
