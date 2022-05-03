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

import rti.connextdds as dds
import rti.idl as idl

import pytest
from rti.idl_impl.test_utils import wait
from test_utils.fixtures import *


# --- Helpers and fixtures ----------------------------------------------------
@idl.struct(member_annotations={'x': [idl.key]})
class PointIDL:
    x: int = 0
    y: int = 0


@idl.struct(member_annotations={'x': [idl.key]})
class PointIDLForDD:
    x: int = 0
    y: int = 0

# The reason these types are wrapped in a lambda is so that they are lazily
# created and they will be garbage collected before we do heap monitoring.
# Since these types would normally be created at test collection time, they
# would outlive the tests and be detected as memory leaks.


def get_test_types_generator():
    return [
        lambda: PointIDL,
        lambda: idl.get_type_support(PointIDLForDD).dynamic_type,
        lambda: dds.StringTopicType
    ]


def get_keyed_types_generator():
    return [lambda: PointIDL, lambda: idl.get_type_support(PointIDLForDD).dynamic_type]


@pytest.fixture(scope="function", params=get_test_types_generator())
def pubsub(request, shared_participant):
    """This fixture provides a test with a writer and a reader that have already
       been discovered. The participant is shared within the module to speed up
       execution, but the contained entities are deleted after each test
       function."""

    data_type = request.param()
    fixture = PubSubFixture(shared_participant, data_type)
    yield fixture
    fixture.participant.close_contained_entities()


class PointListener(dds.NoOpDataReaderListener):
    def on_data_available(self, reader):
        pass


class OtherPointListener(dds.NoOpDataReaderListener):
    def on_data_available(self, reader):
        pass


def get_reader_listeners(type: type):
    if type is PointIDL:
        return (PointListener(), OtherPointListener())
    elif type == idl.get_type_support(PointIDLForDD).dynamic_type:
        return (dds.DynamicData.NoOpDataReaderListener(), dds.DynamicData.NoOpDataReaderListener())
    elif type is dds.StringTopicType:
        return (dds.StringTopicType.NoOpDataReaderListener(), dds.StringTopicType.NoOpDataReaderListener())
    else:
        raise TypeError("Unsupported type: {}".format(type))


def get_reader_w_listeners(participant, topic, type, listener):
    ns = test_utils._get_topic_namespace(topic, type)
    return ns.DataReader(participant, topic, dds.DataReaderQos(), listener)


# --- Datareader tests ---------------------------------------------------------


def test_datareader_listener_can_be_set(pubsub):

    listener, other_listener = get_reader_listeners(pubsub.data_type)

    assert pubsub.reader.listener is None

    pubsub.reader.listener = listener
    assert pubsub.reader.listener == listener

    # Test set_listener fn
    pubsub.reader.set_listener(other_listener, dds.StatusMask.ALL)
    assert pubsub.reader.listener == other_listener

    # Test it can be set to none
    pubsub.reader.listener = None
    assert pubsub.reader.listener is None

    pubsub.reader.listener = get_reader_listeners(pubsub.data_type)[0]
    assert isinstance(pubsub.reader.listener,
                      (dds.DataReaderListener, dds.DynamicData.DataReaderListener, dds.StringTopicType.DataReaderListener))

    # Test constructor
    new_reader = get_reader_w_listeners(
        pubsub.participant, pubsub.topic, pubsub.data_type, listener)

    assert new_reader.listener == listener
    new_reader.close()
