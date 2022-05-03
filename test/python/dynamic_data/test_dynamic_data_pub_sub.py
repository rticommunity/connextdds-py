#
# (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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
import pathlib


# ----------------- Types ------------------------------------------------------

@idl.struct
class Point_IDL:
    x: int = 0
    y: int = 0


# ---------- Helpers and fixtures ----------------------------------------------


def get_sample_point(type: type):

    point = dds.DynamicData(type)
    point["x"] = 1
    point["y"] = 2
    return point


def same_elements(a, b):
    return len(a) == len(b) and all(x in b for x in a)


def check_expected_data(reader: dds.DataReader, expected_samples: list):
    wait.for_data(reader, count=len(expected_samples))
    samples = reader.take_valid()
    assert same_elements([s.data for s in samples], expected_samples)


def get_point_from_idl_type():
    return idl.get_type_support(Point_IDL).dynamic_type


def get_point_from_struct_type():
    point_from_struct_type = dds.StructType("PointFromStructType")
    point_from_struct_type.add_member(dds.Member("x", dds.Int32Type()))
    point_from_struct_type.add_member(dds.Member("y", dds.Int32Type()))
    return point_from_struct_type


def get_point_from_xml_type():
    FILE = str(pathlib.Path(__file__).parent.absolute()) + \
        "/../../xml/Point.xml"
    return dds.QosProvider(FILE).type("Point")

# The reason these types are wrapped in a fncall is so that they will be garbage
# collected before we do heap monitoring. Since these types would normally be
# created at test collection time, they would outlive the tests and be detected
# as memory leaks.


def get_test_types_generator():
    return [
        get_point_from_idl_type,
        get_point_from_struct_type,
        get_point_from_xml_type,
    ]


@pytest.fixture(scope="function", params=get_test_types_generator())
def pubsub(request, shared_participant):
    """This fixture provides a test with a writer and a reader that have already
       been discovered. The participant is shared within the module to speed up
       execution, but the contained entities are deleted after each test
       function."""
    data_type = request.param()
    shared_participant.register_type(data_type.name, data_type)
    fixture = PubSubFixture(shared_participant, data_type)
    yield fixture
    fixture.participant.close_contained_entities()


@pytest.fixture(scope="function", params=get_test_types_generator())
def type_fixture(request):
    return request.param()

# --------- Tests --------------------------------------------------------------


def test_dynamic_data_pub_sub_create_entities(type_fixture):
    participant = dds.DomainParticipant(test_utils.get_test_domain(), test_utils.get_test_participant_qos())
    topic = dds.DynamicData.Topic(participant, "test_topic", type_fixture)
    sub = dds.Subscriber(participant)
    pub = dds.Publisher(participant)
    writer = dds.DynamicData.DataWriter(pub, topic)
    reader = dds.DynamicData.DataReader(sub, topic)
    wait.for_discovery(reader, writer)
    sample = get_sample_point(type_fixture)
    writer.write(sample)
    wait.for_data(reader, count=1)
    samples = reader.take()
    assert len(samples) == 1
    assert samples[0].data == sample


def test_dynamic_data_pub_sub(pubsub):
    sample = get_sample_point(pubsub.data_type)
    pubsub.writer.write(sample)
    check_expected_data(pubsub.reader, [sample])
