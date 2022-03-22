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
from test_utils import wait, fixtures
from test_utils.fixtures import *


# --- Helpers and fixtures ----------------------------------------------------

def same_elements(a, b):
    return len(a) == len(b) and all(x in b for x in a)


def check_expected_data(reader: dds.DataReader, expected_samples: list):
    wait.for_data(reader, count=len(expected_samples))
    samples = reader.take_data()
    assert same_elements(samples, expected_samples)

@idl.struct # TODO PY-17: (member_annotations={'x': [idl.key]})
class Point:
    x: int = 0
    y: int = 0


@pytest.fixture(scope="function", params=[Point])  # TODO add built-in types
def pubsub(request, shared_participant):
    """This fixture provides a test with a writer and a reader that have already
       been discovered. The participant is shared within the module to speed up
       execution, but the contained entities are deleted after each test
       function."""

    fixture = fixtures.PubSubFixture(shared_participant, request.param)
    yield fixture
    fixture.participant.close_contained_entities()


@pytest.fixture(scope="function", params=[Point])  # TODO add built-in types
def pub(request, shared_participant):
    fixture = fixtures.PubSubFixture(
        shared_participant, request.param, create_reader=False)
    yield fixture
    fixture.participant.close_contained_entities()


# --- Write tests -------------------------------------------------------------

def test_write(pubsub):
    sample = Point(x=11, y=22)
    pubsub.writer.write(sample)
    check_expected_data(pubsub.reader, [sample])


def test_write_with_shift_operator(pubsub):
    sample = Point(x=11, y=22)
    pubsub.writer << sample
    check_expected_data(pubsub.reader, [sample])


def test_write_with_timestamp(pubsub):
    sample = Point(x=11, y=22)
    pubsub.writer.write(sample, dds.Time(123))
    check_expected_data(pubsub.reader, [sample])

    # TODO PY-17 check timestamp
    # assert samples[0].timestamp == dds.Time(123)


def test_write_with_instance_handle(pubsub):
    sample = Point(x=11, y=22)
    pubsub.writer.write(sample, handle=dds.InstanceHandle.nil())
    check_expected_data(pubsub.reader, [sample])


def test_write_with_instance_handle_and_timestamp(pubsub):
    sample = Point(x=11, y=22)
    pubsub.writer.write(sample, handle=dds.InstanceHandle.nil(),
                        timestamp=dds.Time(123))
    check_expected_data(pubsub.reader, [sample])


def test_write_w_params(pubsub):
    sample = Point(x=11, y=22)
    params = dds.WriteParams()
    params.source_timestamp = dds.Time(123)
    pubsub.writer.write(sample, params)
    check_expected_data(pubsub.reader, [sample])


def test_write_with_timestamp_with_shift_operator(pubsub):
    sample = Point(x=11, y=22)
    pubsub.writer << (sample, dds.Time(123))
    check_expected_data(pubsub.reader, [sample])


def test_write_list(pubsub):
    samples = [Point(x=11, y=22), Point(x=33, y=44)]
    pubsub.writer.write(samples)
    check_expected_data(pubsub.reader, samples)


def test_write_list_with_shift_operator(pubsub):
    samples = [Point(x=11, y=22), Point(x=33, y=44)]
    pubsub.writer << samples
    check_expected_data(pubsub.reader, samples)


def test_write_list_with_timestamp(pubsub):
    samples = [Point(x=11, y=22), Point(x=33, y=44)]
    pubsub.writer.write(samples, dds.Time(123))
    check_expected_data(pubsub.reader, samples)


def test_write_list_of_pairs_with_shift_operator(pubsub):
    samples = [Point(x=11, y=22), Point(x=33, y=44)]
    pubsub.writer << [(sample, dds.Time(sample.x)) for sample in samples]
    check_expected_data(pubsub.reader, samples)


def test_datareader_listener_can_be_set(pubsub):
    class PointListener(dds.NoOpDataReaderListener):
        def on_data_available(self, reader):
            pass

    class OtherPointListener(dds.NoOpDataReaderListener):
        def on_data_available(self, reader):
            pass


    listener = PointListener()
    other_listener = OtherPointListener()

    assert pubsub.reader.listener is None

    pubsub.reader.listener = listener
    assert pubsub.reader.listener == listener

    # Test set_listener fn
    pubsub.reader.set_listener(other_listener, dds.StatusMask.ALL)
    assert pubsub.reader.listener == other_listener

    # Test it can be set to none
    pubsub.reader.listener = None
    assert pubsub.reader.listener is None
    
    pubsub.reader.listener = PointListener()
    assert type(pubsub.reader.listener) is PointListener 

    # Test constructor
    new_reader = dds.DataReader(
        pubsub.participant, pubsub.topic, dds.DataReaderQos(), listener)
    assert new_reader.listener == listener
    new_reader.close()


# --- Instance tests ----------------------------------------------------------

@pytest.mark.skip(reason="keys not supported yet")
def test_register_dispose_unregister_instance(pub):
    sample = Point(x=11, y=22)
    instance_handle = pub.writer.register_instance(sample)
    assert instance_handle is not None
    assert instance_handle != dds.InstanceHandle.nil()

    pub.writer.dispose_instance(instance_handle)
    pub.writer.unregister_instance(instance_handle)


@pytest.mark.skip(reason="keys not supported yet")
def test_register_dispose_unregister_instance_with_timestamp(pub):
    sample = Point(x=11, y=22)
    instance_handle = pub.writer.register_instance(sample, dds.Time(123))
    assert instance_handle is not None
    assert instance_handle != dds.InstanceHandle.nil()

    pub.writer.dispose_instance(instance_handle, dds.Time(124))
    pub.writer.unregister_instance(instance_handle, dds.Time(125))


@pytest.mark.skip(reason="keys not supported yet")
def test_register_dispose_unregister_instance_with_params(pub):
    sample = Point(x=11, y=22)
    params = dds.WriteParams()
    params.source_timestamp = dds.Time(123)
    instance_handle = pub.writer.register_instance(sample, params)
    assert instance_handle is not None
    assert instance_handle != dds.InstanceHandle.nil()

    params.source_timestamp = dds.Time(124)
    params.handle = instance_handle
    pub.writer.dispose_instance(params)

    params.source_timestamp = dds.Time(125)
    pub.writer.unregister_instance(params)


# --- Manual tests ------------------------------------------------------------

@pytest.mark.skip(reason="This is a manual test")
def test_stress_write():
    # TODO PY-17: this is not automated. It requires manually monitoring the
    # memory of the process to ensure that it doesn't grow unlimited.
    t = Point
    participant = fixtures.create_participant()
    topic = fixtures.create_topic(participant, t)
    writer = fixtures.create_writer(topic, t, dds.DataWriterQos())

    while True:
        samples = Point(x=11, y=22)
        writer.write(samples, dds.Time(123))
