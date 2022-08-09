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

from test_utils.fixtures import *
import rti.connextdds as dds
import rti.types as idl


def test_subscriber_listener_can_be_set(shared_participant):
	listener = dds.NoOpSubscriberListener()
	other_listener = dds.NoOpSubscriberListener()
	sub = dds.Subscriber(shared_participant, dds.SubscriberQos(), listener)
	assert sub.listener == listener

	sub.listener = other_listener
	assert sub.listener == other_listener

	sub.set_listener(None, dds.StatusMask.NONE)
	assert sub.listener is None

	sub.listener = dds.NoOpSubscriberListener()
	assert type(sub.listener) == dds.NoOpSubscriberListener


def test_subscriber_find_readers(shared_participant):
	@idl.struct
	class Foo:
		x: int = 0

	sub = dds.Subscriber(shared_participant)
	assert len(sub.find_datareaders()) == 0

	topic = dds.Topic(shared_participant, 'Foo', Foo)
	reader = dds.DataReader(sub, topic)
	assert len(sub.find_datareaders()) == 1
	reader2 = dds.DataReader(sub, topic)
	assert len(sub.find_datareaders()) == 2
	assert reader, reader2 in sub.find_datareaders()

