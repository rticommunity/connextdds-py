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
from time import sleep

PersonListener_called = False


@idl.struct
class Point:
    x: int = 0
    y: int = 0


class PointListener(dds.TopicListener):
	PointListener_called = False

	def on_inconsistent_topic(self, arg0: dds.AnyTopic, arg1: dds.InconsistentTopicStatus) -> None:
		self.PointListener_called = True


@idl.struct(member_annotations={'name': [idl.bound(10)]})
class Person:
	name: str = ""


def test_two_topics_with_same_name(shared_participant):

	person_participant = create_participant()
	person_topic = dds.Topic(person_participant, "shared_topic_name", Person)

	point_listener = PointListener()
	point_topic = dds.Topic(
	    shared_participant, "shared_topic_name", Point, dds.TopicQos(), point_listener)

	w = dds.DataWriter(person_participant.implicit_publisher, person_topic)
	r = dds.DataReader(shared_participant.implicit_subscriber, point_topic)

	listener_called = False
	for i in range(0, 30):
		if point_listener.PointListener_called:
			listener_called = True
			break
		sleep(.1)

	assert listener_called

	person_participant.close()
