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
import rti.idl as idl
from typing import Final

# We subtract 1024 from the max serialized size because the core does some 
# operations on the returned result that may end up adding some additional byte
MAX_SERIALIZED_SIZE_ADDITIONAL_BYTES = 1024

def get_test_domain():
    return int(os.environ.get('TEST_DOMAIN', 0))


def get_test_participant_qos():
    qos = dds.DomainParticipantQos()
    qos.database.shutdown_cleanup_period = dds.Duration.from_milliseconds(10)
    return qos


def create_participant():
    return dds.DomainParticipant(get_test_domain(), get_test_participant_qos())


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
    if isinstance(topic, dds.Topic) or isinstance(topic, dds.ContentFilteredTopic):
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
    def __init__(self, participant, data_type, create_writer=True, create_reader=True, reader_policies=[], content_filter=None):
        self.data_type = data_type

        # Figure out if the type has an unbounded value in it
        if "dds." in str(data_type):
            has_unbound_member = False
        else:
            has_unbound_member = (idl.unbounded.value-MAX_SERIALIZED_SIZE_ADDITIONAL_BYTES <= idl.get_type_support(
                data_type).max_serialized_sample_size)
        
        if participant is None:
            self.participant = create_participant()
        else:
            self.participant = participant
        self.topic = create_topic(self.participant, data_type)

        if create_writer:
            writer_qos = self.participant.implicit_publisher.default_datawriter_qos
            writer_qos << dds.Reliability.reliable()
            writer_qos << dds.History.keep_all
            if has_unbound_member:
                writer_qos.property['dds.data_writer.history.memory_manager.fast_pool.pool_buffer_max_size'] = str(
                    1000)
            self.writer = _create_writer(self.topic, data_type, writer_qos)

        if create_reader:
            reader_qos = self.participant.implicit_subscriber.default_datareader_qos
            reader_qos << dds.Reliability.reliable()
            reader_qos << dds.History.keep_all
            for policy in reader_policies:
                reader_qos << policy
            if content_filter is not None:
                cft_name = "Filtered " + self.topic.name
                if type(content_filter) == str:
                    cft = dds.ContentFilteredTopic(
                        self.topic, cft_name, dds.Filter(content_filter))
                elif type(content_filter) == tuple:
                    cft = dds.ContentFilteredTopic(
                        self.topic, cft_name, dds.Filter(*content_filter))
                else:
                    raise TypeError(f"Expected str or tuple, given {type(content_filter)}")
                self.reader = _create_reader(cft, data_type, reader_qos)
            else:
                self.reader = _create_reader(self.topic, data_type, reader_qos)


        if create_reader and create_writer:
            wait.for_discovery(self.reader, self.writer)

    def send_and_check(self, data):
        self.writer.write(data)

        if type(data) is not list:
            data = [data]

        wait.for_data(self.reader, count=len(data))
        received_data = self.reader.take_data()
        assert len(received_data) == len(data)
        assert all(d in received_data for d in data)


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
