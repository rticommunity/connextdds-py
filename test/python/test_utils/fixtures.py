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
import os
import rti.connextdds as dds
from . import wait

def get_test_domain():
    return int(os.environ.get('TEST_DOMAIN', 0))


def create_participant():
    qos = dds.DomainParticipantQos()
    qos.database.shutdown_cleanup_period = dds.Duration.from_milliseconds(10)
    return dds.DomainParticipant(get_test_domain(), qos)

def create_topic(participant: dds.DomainParticipant, type: type):
    topic_name = f'Example {type}'

    # TODO PY-21: these helpers wouldn't be necessary if all topics were
    # unified under dds.Topic.

    # For non-IDL types, such as dds.StringTopicType, dds.DynamicData:
    if "dds." in str(type):
        return type.Topic(participant, topic_name)
    # For IDL types:
    else:
        return dds.Topic(participant, topic_name, type)

# Gets the "namespace" that contains this topic. When type is an IDL type,
# it returns the dds package; for the rest it returns the type, e.g. if
# type is a StringTopicType, it returns dds.StringTopicType.
def _get_topic_namespace(topic, type):
    if isinstance(topic, dds.Topic):
        return dds
    else:
        return type

def _create_reader(topic, type, reader_qos):
    ns = _get_topic_namespace(topic, type)
    return ns.DataReader(topic.participant, topic, reader_qos)

def _create_writer(topic, type, writer_qos):
    ns = _get_topic_namespace(topic, type)
    return ns.DataWriter(topic.participant, topic, writer_qos)


class PubSubFixture:
    def __init__(self, participant, data_type, create_writer=True, create_reader=True):
        self.data_type = data_type

        if participant is None:
            self.participant = create_participant()
        else:
            self.participant = participant
        self.topic = create_topic(self.participant, data_type)

        if create_writer:
            writer_qos = self.participant.implicit_publisher.default_datawriter_qos
            writer_qos << dds.Reliability.reliable()
            writer_qos << dds.History.keep_all
            self.writer = _create_writer(self.topic, data_type, writer_qos)

        if create_reader:
            reader_qos = self.participant.implicit_subscriber.default_datareader_qos
            reader_qos << dds.Reliability.reliable()
            reader_qos << dds.History.keep_all
            self.reader = _create_reader(self.topic, data_type, reader_qos)

        if create_reader and create_writer:
            wait.for_discovery(self.reader, self.writer)


@pytest.fixture(scope="module")
def shared_participant():
    participant = create_participant()
    yield participant
    participant.close()

