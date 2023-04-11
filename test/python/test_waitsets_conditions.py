#
# (c) 2023 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#


import rti.connextdds as dds
from test_utils.fixtures import *
from rti.types.builtin import String


def test_waitset_property():
	wsp = dds.WaitSetProperty()
	assert wsp.event_count == 1
	assert wsp.event_delay == dds.Duration.infinite
	wsp = dds.WaitSetProperty(42, dds.Duration(42, 0))
	assert wsp.event_count == 42
	assert wsp.event_delay == dds.Duration(42, 0)
	wsp.event_count = 43
	assert wsp.event_count == 43
	wsp.event_delay = dds.Duration(43, 0)
	assert wsp.event_delay == dds.Duration(43, 0)
	wsp2 = dds.WaitSetProperty(43, dds.Duration(43, 0))
	assert wsp == wsp2
	wsp2.event_count = 44
	assert wsp != wsp2


def test_waitset_init():
	waitset = dds.WaitSet()
	assert waitset is not None
	assert waitset.conditions == []
	assert waitset.property.event_count == 1
	assert waitset.property.event_delay == dds.Duration.infinite
	waitset = dds.WaitSet(dds.WaitSetProperty(1, dds.Duration(1, 0)))
	assert waitset.property == dds.WaitSetProperty(1, dds.Duration(1, 0))


def test_waitset_attach_detach():
	waitset = dds.WaitSet()
	assert waitset is not None
	p1 = create_participant()
	p2 = create_participant()
	c1 = dds.StatusCondition(p1)
	c2 = dds.StatusCondition(p2)
	waitset.attach_condition(c1)
	assert c1 == waitset.conditions[0]
	# Since waitset.conditions returns a list of TriggeredConditions objects
	# c1 is not in the list unless we convert the list to StatusCondition
	# objects but we still want to test that the "in" operator works
	c1_triggered_condition = waitset.conditions[0]
	assert c1_triggered_condition in waitset.conditions
	assert len(waitset.conditions) == 1
	assert c1 in map(lambda x: dds.StatusCondition(x), waitset.conditions)
	waitset.attach_condition(c2)
	for c in waitset.conditions:
		assert c in (c1, c2)
	assert len(waitset.conditions) == 2
	waitset.detach_condition(c1)
	assert len(waitset.conditions) == 1
	assert c2 == waitset.conditions[0]
	waitset.detach_condition(c2)
	assert waitset.conditions == []
	waitset += c1
	assert c1 == waitset.conditions[0]
	assert len(waitset.conditions) == 1
	assert waitset.conditions[0].trigger_value == False
	waitset += c2
	for c in waitset.conditions:
		assert c in (c1, c2)
	assert len(waitset.conditions) == 2
	assert waitset.conditions[0] != waitset.conditions[1]
	assert not waitset.conditions[0] == waitset.conditions[1]
	waitset -= c1
	assert len(waitset.conditions) == 1
	assert c2 == waitset.conditions[0]
	waitset -= c2
	assert waitset.conditions == []
	waitset.conditions = [c1, c2]
	assert len(waitset.conditions) == 2
	for c in reversed(waitset.conditions):
		assert c in (c1, c2)
	waitset.detach_all()
	assert waitset.conditions == []


def test_waitset_wait(shared_participant):
	waitset = dds.WaitSet()
	pubsub = PubSubFixture(shared_participant, String)
	sc = dds.StatusCondition(pubsub.reader)
	sc.enabled_statuses = dds.StatusMask.DATA_AVAILABLE
	rc = dds.ReadCondition(pubsub.reader, dds.DataState.any_data)
	waitset += sc
	waitset += rc

	# Wait with set duration
	samples_read = 0
	data_available = 0
	while samples_read < 5:
		pubsub.writer.write(String("Hello World"))
		active = waitset.wait(dds.Duration(5, 0))
		if sc in active:
			data_available += 1
		if rc in active:
			samples_read += len(pubsub.reader.take())
	# It is possible (but unlikely) to have data_available > 5 due to it being 
	# possible for sc to be in active while rc has not been updated yet
	assert data_available >= 5

	# Wait with infinite duration
	samples_read = 0
	data_available = 0
	while samples_read < 5:
		pubsub.writer.write(String("Hello World"))
		active = waitset.wait()
		if sc in active:
			data_available += 1
		if rc in active:
			samples_read += len(pubsub.reader.take())
	# It is possible (but unlikely) to have data_available > 5 due to it being 
	# possible for sc to be in active while rc has not been updated yet
	assert data_available >= 5


def test_waitset_dispatch(shared_participant):
	waitset = dds.WaitSet()
	pubsub = PubSubFixture(shared_participant, String)
	sc = dds.StatusCondition(pubsub.reader)
	sc.enabled_statuses = dds.StatusMask.DATA_AVAILABLE
	assert sc.enabled_statuses == dds.StatusMask.DATA_AVAILABLE
	samples_read = 0

	def handler(c):
		nonlocal samples_read
		if c == sc:
			samples_read += 1

	sc.set_handler(handler)
	waitset += sc

	# Dispatch with set duration
	for _ in range(5):
		pubsub.writer.write(String("Hello World"))
		waitset.dispatch(dds.Duration(5, 0))
	assert samples_read == 5

	# Dispatch with infinite duration
	samples_read = 0
	for _ in range(5):
		pubsub.writer.write(String("Hello World"))
		waitset.dispatch()

	wait.until(lambda: samples_read == 5)


def test_status_condition(shared_participant):
	sc = dds.StatusCondition(shared_participant)
	sc.enabled_statuses = dds.StatusMask.DATA_ON_READERS
	assert sc.enabled_statuses == dds.StatusMask.DATA_ON_READERS
	sc.enabled_statuses = dds.StatusMask.DATA_AVAILABLE
	assert sc.enabled_statuses == dds.StatusMask.DATA_AVAILABLE
	assert sc.entity == shared_participant
	sc.set_handler(lambda c: None)
	sc.reset_handler()

	assert sc == dds.StatusCondition(shared_participant)
	assert sc != dds.StatusCondition(shared_participant.implicit_subscriber)
	assert sc.trigger_value == False
	
	ws = dds.WaitSet()
	ws += sc
	assert sc in map(lambda x: dds.StatusCondition(x), ws.conditions)


def test_guard_condition():
	gc = dds.GuardCondition()
	assert gc.trigger_value == False
	gc.trigger_value = True
	assert gc.trigger_value == True
	gc.trigger_value = False
	assert gc.trigger_value == False
	gc.set_handler(lambda c: None)
	gc.reset_handler()
	gc.set_handler_no_args(lambda: None)
	gc.reset_handler()

	assert gc == gc
	assert gc != dds.GuardCondition()

	ws = dds.WaitSet()
	ws += gc
	assert gc in map(lambda x: dds.GuardCondition(x), ws.conditions)


def test_query_condition(shared_participant):
	pubsub = PubSubFixture(shared_participant, String)
	e = "value = 'Hello World'"
	q = dds.Query(pubsub.reader, e)
	qc1 = dds.QueryCondition(q, dds.DataState())
	qc2 = dds.QueryCondition(q, dds.DataState(), lambda x: None)
	qc3 = dds.QueryCondition(q, dds.DataStateEx())
	qc4 = dds.QueryCondition(q, dds.DataStateEx(), lambda x: None)
	
	assert qc1.expression == e
	assert qc1.parameters == []
	assert len(qc1) == 0
	qc1.parameters = dds.StringSeq(["1"])
	assert qc1.parameters == dds.StringSeq(["1"])
	assert len(qc1) == 1
	assert qc1.parameters_length == 1
	
	assert qc1 == qc1
	assert qc1 != qc2
	assert not qc1 == qc2
	
	qc1.set_handler_no_args(lambda: None)
	qc1.reset_handler()
	
	qc1.set_handler(lambda c: None)
	qc1.reset_handler()
	
	ws = dds.WaitSet()
	ws += qc1
	ws += qc2
	ws += qc3
	ws += qc4
	assert qc1 in map(lambda x: dds.QueryCondition(x), ws.conditions)
	assert qc2 in map(lambda x: dds.QueryCondition(x), ws.conditions)
	assert qc3 in map(lambda x: dds.QueryCondition(x), ws.conditions)
	assert qc4 in map(lambda x: dds.QueryCondition(x), ws.conditions)
	
	