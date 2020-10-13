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
import pytest
import pathlib

LOCATION = str(pathlib.Path(__file__).parent.absolute()) + "/"


def test_string_uri_creation():
    assert (
        len(
            dds.QosProvider(
                LOCATION + "../xml/XmlApplication.xml"
            ).qos_profile_libraries
        )
        == 4
    )


def test_file_uri_creation():
    pass


def test_invalid_creation():
    with pytest.raises(dds.Error):
        dds.QosProvider("invalid_file.xml")
    with pytest.raises(dds.Error):
        dds.QosProvider(LOCATION + "../xml/InvalidXml.xml")


def test_default_provider():
    provider1 = dds.QosProvider.default
    provider2 = dds.QosProvider.default
    provider3 = provider2
    provider4 = provider3
    assert 3 == len(provider1.qos_profile_libraries)
    assert 3 == len(provider2.qos_profile_libraries)
    assert 3 == len(provider3.qos_profile_libraries)
    assert 3 == len(provider4.qos_profile_libraries)

    params = dds.QosProviderParams()
    params.url_profile = dds.StringSeq([LOCATION + "../xml/XmlApplication.xml"])
    provider4.reload_profiles()
    provider4.provider_params = params

    assert 4 == len(provider1.qos_profile_libraries)
    assert 4 == len(provider2.qos_profile_libraries)
    assert 4 == len(provider3.qos_profile_libraries)
    assert 4 == len(provider4.qos_profile_libraries)


def test_getters():
    DEFAULT_PROFILE = "my_default_profile1"
    TEST_LIB = "my_test_library1"
    domain_participant_qos = dds.DomainParticipantQos()
    pub_qos = dds.PublisherQos()
    dw_qos = dds.DataWriterQos()
    sub_qos = dds.SubscriberQos()
    dr_qos = dds.DataWriterQos()
    topic_qos = dds.TopicQos()
    topic_a = "topic_A"
    topic_b = "topic_B"

    my_uri = LOCATION + "../xml/QosProviderTest_qos1.xml"
    qos_provider = dds.QosProvider(my_uri)

    my_other_profile_name = "my_other_profile1"

    assert qos_provider.default_library == TEST_LIB
    assert qos_provider.default_profile == DEFAULT_PROFILE
    assert qos_provider.default_library_profile == TEST_LIB

    domain_participant_qos = qos_provider.participant_qos
    assert domain_participant_qos.participant_name.name == "defaultParticipantName"

    pub_qos = qos_provider.publisher_qos
    assert pub_qos.entity_name.name == "defaultPublisherName"

    dw_qos = qos_provider.datawriter_qos
    assert dw_qos.entity_name.name == "defaultPublicationName"

    dw_qos = qos_provider.get_topic_datawriter_qos(topic_a)
    assert dw_qos.entity_name.name == "defaultPublicationNameA"

    dw_qos = qos_provider.get_topic_datawriter_qos(topic_b)
    assert dw_qos.entity_name.name == "defaultPublicationNameB"

    sub_qos = qos_provider.subscriber_qos
    assert sub_qos.entity_name.name == "defaultSubscriberName"

    dr_qos = qos_provider.datareader_qos
    assert dr_qos.entity_name.name == "defaultSubscriptionName"

    dr_qos = qos_provider.get_topic_datareader_qos(topic_a)
    assert dr_qos.entity_name.name == "defaultSubscriptionNameA"

    dr_qos = qos_provider.get_topic_datareader_qos(topic_b)
    assert dr_qos.entity_name.name == "defaultSubscriptionNameB"

    topic_qos = qos_provider.topic_qos
    assert topic_qos.resource_limits.max_samples == 100

    topic_qos = qos_provider.get_topic_name_qos(topic_a)
    assert topic_qos.resource_limits.max_samples == 101

    topic_qos = qos_provider.get_topic_name_qos(topic_b)
    assert topic_qos.resource_limits.max_samples == 102

    # Start using my_other_profile_name

    domain_participant_qos = qos_provider.participant_qos_profile(my_other_profile_name)
    assert domain_participant_qos.participant_name.name == "otherParticipantName"

    pub_qos = qos_provider.get_publisher_qos_profile(my_other_profile_name)
    assert pub_qos.entity_name.name == "otherPublisherName"

    dw_qos = qos_provider.get_datawriter_qos_profile(my_other_profile_name)
    assert dw_qos.entity_name.name == "otherPublicationName"

    dw_qos = qos_provider.set_topic_datawriter_qos(my_other_profile_name, topic_a)
    assert dw_qos.entity_name.name == "otherPublicationNameA"

    dw_qos = qos_provider.set_topic_datawriter_qos(my_other_profile_name, topic_b)
    assert dw_qos.entity_name.name == "otherPublicationNameB"

    sub_qos = qos_provider.get_subscriber_qos_profile(my_other_profile_name)
    assert sub_qos.entity_name.name == "otherSubscriberName"

    dr_qos = qos_provider.get_datareader_qos_profile(my_other_profile_name)
    assert dr_qos.entity_name.name == "otherSubscriptionName"

    dr_qos = qos_provider.set_topic_datareader_qos(my_other_profile_name, topic_a)
    assert dr_qos.entity_name.name == "otherSubscriptionNameA"

    dr_qos = qos_provider.set_topic_datareader_qos(my_other_profile_name, topic_b)
    assert dr_qos.entity_name.name == "otherSubscriptionNameB"

    topic_qos = qos_provider.get_topic_qos_profile(my_other_profile_name)
    assert topic_qos.resource_limits.max_samples == 200

    topic_qos = qos_provider.set_topic_name_qos(my_other_profile_name, topic_a)
    assert topic_qos.resource_limits.max_samples == 201

    topic_qos = qos_provider.set_topic_name_qos(my_other_profile_name, topic_b)
    assert topic_qos.resource_limits.max_samples == 202

    # USE_DDS_DEFAULT_QOS_PROFILE does not exist and cannot be tested
