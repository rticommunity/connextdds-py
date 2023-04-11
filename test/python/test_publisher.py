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
from rti.types.builtin import String


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


def test_publisher_qos_getter_setter(shared_participant):
	pub = dds.Publisher(shared_participant)
	assert pub.qos == dds.PublisherQos()

	new_qos = dds.PublisherQos()
	new_qos.partition.name = ['test']
	pub.qos = new_qos
	assert pub.qos == new_qos
	assert pub.qos.partition.name == ['test']
	other_qos = dds.PublisherQos()
	other_qos.partition.name = ['other']
	pub << other_qos
	assert pub.qos == other_qos
	assert pub.qos.partition.name == ['other']
	same_as_other_qos = dds.PublisherQos()
	pub >> same_as_other_qos
	assert same_as_other_qos == other_qos
	assert same_as_other_qos.partition.name == ['other']


def test_publisher_default_dw_qos(shared_participant):
	pub = dds.Publisher(shared_participant)
	assert pub.default_datawriter_qos == dds.DataWriterQos()

	dw_qos = dds.DataWriterQos()
	dw_qos.entity_name = dds.EntityName('test')
	pub.default_datawriter_qos = dw_qos
	dw = dds.DataWriter(pub, dds.Topic(
		shared_participant, 'String_topic', String))
	assert dw.qos.entity_name == dw_qos.entity_name


def test_publisher_participant(shared_participant):
	pub = dds.Publisher(shared_participant)
	assert pub.participant == shared_participant


def test_publisher_wait_for_acknowledgement_no_acknowledgement(shared_participant):
	pub = dds.Publisher(shared_participant)
	topic = dds.Topic(shared_participant, 'String_topic', String)

	writer_qos = pub.default_datawriter_qos
	writer_qos.reliability.acknowledgment_kind = dds.AcknowledgmentKind.APPLICATION_EXPLICIT
	writer_qos.data_writer_protocol.rtps_reliable_writer.heartbeat_period = dds.Duration(
		0, 10000000)
	writer_qos.data_writer_protocol.rtps_reliable_writer.fast_heartbeat_period = dds.Duration(
		0, 1000000)
	writer_qos.data_writer_protocol.rtps_reliable_writer.late_joiner_heartbeat_period = dds.Duration(
		0, 10000000)
	writer = dds.DataWriter(pub, topic, writer_qos)

	sub = dds.Subscriber(shared_participant)

	reader_qos = sub.default_datareader_qos
	reader_qos.reliability.acknowledgment_kind = dds.AcknowledgmentKind.APPLICATION_EXPLICIT
	reader_qos.reliability.kind = dds.ReliabilityKind.RELIABLE
	reader_qos.data_reader_protocol.rtps_reliable_reader.max_heartbeat_response_delay = dds.Duration(
		0, 0)

	reader = dds.DataReader(sub, topic, reader_qos)
	wait.for_discovery(reader, writer)
	writer.write(String('test'))

	with pytest.raises(dds.TimeoutError):
		pub.wait_for_acknowledgments(dds.Duration(0, 20000000))


def test_publisher_wait_for_acknowledgement_with_acknowledgement(shared_participant):
	pub = dds.Publisher(shared_participant)
	topic = dds.Topic(shared_participant, 'String_topic', String)

	writer_qos = pub.default_datawriter_qos
	writer_qos.reliability.acknowledgment_kind = dds.AcknowledgmentKind.APPLICATION_EXPLICIT
	writer_qos.data_writer_protocol.rtps_reliable_writer.heartbeat_period = dds.Duration(
		0, 10000000)
	writer_qos.data_writer_protocol.rtps_reliable_writer.fast_heartbeat_period = dds.Duration(
		0, 1000000)
	writer_qos.data_writer_protocol.rtps_reliable_writer.late_joiner_heartbeat_period = dds.Duration(
		0, 10000000)

	writer = dds.DataWriter(pub, topic, writer_qos)
	sub = dds.Subscriber(shared_participant)

	reader_qos = sub.default_datareader_qos
	reader_qos.reliability.kind = dds.ReliabilityKind.RELIABLE
	reader_qos.data_reader_protocol.rtps_reliable_reader.app_ack_period = dds.Duration(
		1, 0)
	reader_qos.data_reader_protocol.rtps_reliable_reader.samples_per_app_ack = 1

	reader = dds.DataReader(sub, topic, reader_qos)
	wait.for_discovery(reader, writer)
	writer.write(String('test'))

	wait.for_data(reader, 1)
	reader.take()
	reader.acknowledge_all()

	pub.wait_for_acknowledgments(dds.Duration(5, 0))


def test_publisher_find_writers(shared_participant):

	pub = dds.Publisher(shared_participant)
	assert len(pub.find_datawriters()) == 0

	topic = dds.Topic(shared_participant, 'Foo', String)
	qos = dds.DataWriterQos()
	qos.entity_name = dds.EntityName('foo')
	writer = dds.DataWriter(pub, topic, qos)
	assert len(pub.find_datawriters()) == 1
	writer2 = dds.DataWriter(pub, topic)
	assert len(pub.find_datawriters()) == 2
	assert writer, writer2 in pub.find_datawriters()
	assert pub.find_datawriter('foo') == writer
	# find_datawriter_by_topic_names returns the first writer found
	# so it could be writer or writer2
	assert pub.find_datawriter_by_topic_name('Foo') in (writer, writer2)