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

import pathlib

import rti.connextdds as dds

from test_utils.fixtures import *
from test_sequences import SequenceTest, create_sequence_sample


XML_FILE = str(pathlib.Path(__file__).parent.absolute()) + "/" \
    + "../../xml/AppCreationTestConfiguration.xml"

@pytest.fixture(scope="module")
def register_type():
    dds.DomainParticipant.register_idl_type(SequenceTest, "MyIdlType")

def test_xml_app_pub_sub_idl(register_type):

    provider = dds.QosProvider(XML_FILE)
    with provider.create_participant_from_config(
        "TestParticipantLibrary::IdlParticipant") as participant:

        writer = dds.DataWriter.find_by_name(participant, "TestWriter")
        assert writer is not None
        reader = dds.DataReader.find_by_name(participant, "TestReader")
        assert reader is not None
        assert writer.topic == reader.topic_description
        assert writer.topic.type_name == "MyIdlType"

        wait.for_discovery(reader, writer)

        data = create_sequence_sample()
        writer.write(create_sequence_sample())

        wait.for_data(reader, 1)
        assert reader.take_data()[0] == data


def test_xml_app_pub_sub_w_cft_idl(register_type):

    provider = dds.QosProvider(XML_FILE)
    params = dds.DomainParticipantConfigParams(domain_id=get_test_domain())
    with provider.create_participant_from_config(
            "TestParticipantLibrary::IdlParticipant", params) as participant:

        writer = dds.DataWriter.find_by_name(participant, "TestWriter")
        assert writer is not None
        reader = dds.DataReader.find_by_name(participant, "TestCftReader")
        assert reader is not None
        cft = dds.ContentFilteredTopic(reader.topic_description)
        assert cft.filter_expression == "vertices[0].x > 10"
        assert writer.topic == cft.topic
        assert writer.topic.type_name == "MyIdlType"

        wait.for_discovery(reader, writer)

        data = create_sequence_sample()
        data.vertices[0].x = 4
        writer.write(data)
        data.vertices[0].x = 11
        writer.write(data)

        wait.for_data(reader, 1)
        assert reader.take_data()[0] == data


def test_xml_app_entity_deletion(register_type):

    provider = dds.QosProvider(XML_FILE)
    params = dds.DomainParticipantConfigParams(domain_id=get_test_domain())
    with provider.create_participant_from_config(
            "TestParticipantLibrary::IdlParticipant", params) as participant:

        writer = dds.DataWriter.find_by_name(participant, "TestWriter")
        assert writer is not None
        writer.close()
        assert dds.DataWriter.find_by_name(participant, "TestWriter") is None

        reader = dds.DataReader.find_by_name(participant, "TestCftReader")
        cft = dds.ContentFilteredTopic(reader.topic_description)
        assert reader is not None
        reader.close()
        assert dds.DataReader.find_by_name(participant, "TestCftReader") is None

        cft.close()