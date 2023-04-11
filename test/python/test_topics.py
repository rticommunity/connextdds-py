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
from rti.types.builtin import String


@idl.struct
class Point:
    x: int = 0
    y: int = 0


class PointListener(dds.TopicListener):
	listener_called = False

	def on_inconsistent_topic(self, topic: dds.AnyTopic, status: dds.InconsistentTopicStatus) -> None:
		assert topic.type_name == "Point"
		assert topic.name == "shared_topic_name"
		assert status.total_count == 1
		assert status.total_count_change == 1
		PointListener.listener_called = True


@idl.struct(member_annotations={'name': [idl.bound(10)]})
class Person:
	name: str = ""


def test_topic_listener_can_be_set(shared_participant):
    class PointListener(dds.TopicListener):
        def on_inconsistent_topic(self, topic: dds.AnyTopic, status: dds.InconsistentTopicStatus) -> None:
            pass

    class OtherPointListener(dds.TopicListener):
        def on_inconsistent_topic(self, topic: dds.AnyTopic, status: dds.InconsistentTopicStatus) -> None:
            pass

    listener = PointListener()
    other_listener = OtherPointListener()
    topic = dds.Topic(shared_participant, "point_topic",
                      Point, dds.TopicQos(), listener)
    assert topic.listener == listener

    topic.listener = other_listener
    assert topic.listener == other_listener

    # Test set_listener fn
    topic.set_listener(listener, dds.StatusMask.ALL)
    assert topic.listener == listener

    # Test it can be set to none
    topic.listener = None
    assert topic.listener is None


def test_inconsistent_topic(shared_participant):

	person_participant = create_participant()
	person_topic = dds.Topic(person_participant, "shared_topic_name", Person)

	point_listener = PointListener()
	point_topic = dds.Topic(
	    shared_participant, "shared_topic_name", Point, dds.TopicQos(), point_listener)

	w = dds.DataWriter(person_participant.implicit_publisher, person_topic)
	r = dds.DataReader(shared_participant.implicit_subscriber, point_topic)

	wait.until(lambda: PointListener.listener_called)


def test_close_topic(shared_participant):
    topic = dds.Topic(shared_participant, "StringTopic", String)
    assert "StringTopic" == shared_participant.find_topics()[0].name
    topic.close()
    assert len(shared_participant.find_topics()) == 0


def test_topic_qos_getter_setter(shared_participant):
    topic = dds.Topic(shared_participant, "StringTopic", String)
    qos = topic.qos
    assert qos == dds.TopicQos()
    qos.deadline.period = dds.Duration(1, 2)
    topic.qos = qos
    assert topic.qos == qos
    assert topic.qos.deadline.period == dds.Duration(1, 2)


def test_topic_type_name(shared_participant):
    topic = dds.Topic(shared_participant, "StringTopic", String)
    assert topic.type_name == "String"


def test_topic_comparison(shared_participant):
    topic1 = dds.Topic(shared_participant, "StringTopic", String)
    topic2 = topic1
    topic3 = dds.Topic(shared_participant, "OtherTopic", String)
    sc = dds.StatusCondition(topic1)
    topic4 = dds.Topic(sc.entity)
    assert topic1 == topic2
    assert topic1 == topic4
    assert topic1 != topic3


def test_topic_inconsistent_topic_status(shared_participant):
    topic = dds.Topic(shared_participant, "StringTopic", String)
    status = topic.inconsistent_topic_status
    assert status.total_count == 0
    assert status.total_count_change == 0


def test_dd_topic_functions(shared_participant):
    dd_type = dds.StructType("DD_Type")
    t1 = dds.DynamicData.Topic(shared_participant, "DD_Topic1", dd_type)
    t2 = dds.DynamicData.Topic(
        shared_participant, "DD_Topic2", "DD_Type", dds.TopicQos())
    t3 = dds.DynamicData.Topic(t1)
    t4 = dds.DynamicData.Topic(
        shared_participant, "DD_Topic4", dd_type, dds.TopicQos())
    sc = dds.StatusCondition(t1)
    assert dds.DynamicData.Topic(sc.entity) == t1
    status = t1.inconsistent_topic_status
    assert status.total_count == 0
    assert status.total_count_change == 0
    assert t1 == t3
    assert t1 != t2
    assert t1.type_name == "DD_Type"
    qos = t1.qos
    assert qos == dds.TopicQos()
    qos.deadline.period.sec = 1
    t1.qos = qos
    assert t1.qos == qos
    assert t1 == t3
