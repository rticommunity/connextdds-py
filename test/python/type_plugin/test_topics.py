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

PointListener_called = False
PersonListener_called = False

@idl.struct
class Point:
    x: int = 0
    y: int = 0


class PointListener(dds.NoOpAnyTopicListener):
	def on_inconsistent_topic(self, arg0: dds.AnyTopic, arg1: dds.InconsistentTopicStatus) -> None:
		global PointListener_called 
		PointListener_called = True

@idl.struct
class Person:
	name: str = ""

def test_two_topics_with_same_name(shared_participant):
	point_listener = PointListener()
	assert isinstance(point_listener, dds.AnyTopicListener)
	point_topic = dds.Topic(shared_participant, "Point_Topic", Point, dds.TopicQos(), point_listener)
	person_topic = dds.Topic(shared_participant, "Person_Topic")

	assert PointListener_called
	
