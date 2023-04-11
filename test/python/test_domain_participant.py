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
from test_utils.fixtures import (
    create_participant,
    get_test_domain,
    get_test_participant_qos,
    wait,
    participant,
    shared_participant,
    shared_participant_no_cleanup,
    PubSubFixture)
from test_utils import log_capture
from rti.types.builtin import String
from time import sleep


def test_participant_default_creation(shared_participant):
    assert shared_participant.domain_id == get_test_domain()


def test_participant_alternative_constructors():
    # init with just domain_id
    p1 = dds.DomainParticipant(get_test_domain())
    # init using entity
    p2 = dds.DomainParticipant(p1)
    assert p1.domain_id == p2.domain_id
    assert p1.qos == p2.qos
    assert p1.domain_id == get_test_domain()


def test_participant_creation_w_default_qos(shared_participant):
    assert shared_participant.domain_id == get_test_domain()
    event_count = shared_participant.qos.event.max_count
    assert event_count == dds.Event().max_count


def test_participant_creation_w_qos():
    user_data_values = [10, 11, 12, 13, 14, 15]
    qos = get_test_participant_qos()
    qos.user_data.value = user_data_values
    qos.database.shutdown_cleanup_period = dds.Duration.from_milliseconds(100)
    # Use the with statement so the GC will always close the participant
    # immediately after the test
    with dds.DomainParticipant(get_test_domain(), qos) as p:
        retrieved_qos = p.qos
        assert retrieved_qos.user_data.value == user_data_values
        assert (
            retrieved_qos.database.shutdown_cleanup_period
            == dds.Duration.from_milliseconds(100)
        )


def test_participant_creation_w_listener():
    l = dds.NoOpDomainParticipantListener()
    with dds.DomainParticipant(get_test_domain(), get_test_participant_qos(), l) as p:
        assert p.listener == l


def test_participant_creation_failure():
    qos = get_test_participant_qos()
    qos.resource_limits.type_object_max_serialized_length = -2
    with log_capture.expected_exception(dds.Error):
        dds.DomainParticipant(0, qos)


def test_set_get_qos(shared_participant):
    qos = shared_participant.qos
    assert qos.entity_factory.autoenable_created_entities
    qos << dds.EntityFactory.manually_enable
    assert not (qos.entity_factory.autoenable_created_entities)
    shared_participant.qos = qos
    # assert qos == p.qos
    assert not (qos.entity_factory.autoenable_created_entities)
    qos << dds.EntityFactory.auto_enable
    shared_participant << qos
    retrieved_qos = get_test_participant_qos()
    shared_participant >> retrieved_qos
    assert retrieved_qos.entity_factory.autoenable_created_entities


def test_set_qos_exception(shared_participant):
    qos = shared_participant.qos
    d = dds.Duration(77)
    db = dds.Database()
    db.shutdown_cleanup_period = d
    qos << db
    with log_capture.expected_exception(dds.ImmutablePolicyError):
        shared_participant.qos = qos


def test_set_get_default_qos():
    default_qos = dds.DomainParticipant.default_participant_qos
    try:
        set_qos = dds.DomainParticipant.default_participant_qos
        set_qos << dds.UserData([33, 33])
        assert set_qos != dds.DomainParticipant.default_topic_qos
        dds.DomainParticipant.default_participant_qos = set_qos
        get_qos = dds.DomainParticipant.default_participant_qos
        assert get_qos == set_qos
        assert get_qos.user_data == dds.UserData([33, 33])
    finally:
        # Reset the default qos
        dds.DomainParticipant.default_participant_qos = default_qos


def test_set_get_factory_qos():
    default_qos = dds.DomainParticipantFactoryQos()
    try:
        set_qos = dds.DomainParticipantFactoryQos()
        set_qos << dds.EntityFactory(False)
        assert set_qos != default_qos
        dds.DomainParticipant.participant_factory_qos = set_qos
        get_qos = dds.DomainParticipant.participant_factory_qos
        assert set_qos == get_qos
        assert get_qos.entity_factory == dds.EntityFactory(False)
        set_qos << dds.EntityFactory(True)
        dds.DomainParticipant.participant_factory_qos = set_qos
        get_qos = dds.DomainParticipant.participant_factory_qos
        assert set_qos == get_qos
    finally:
        # Reset the default qos
        dds.DomainParticipant.participant_factory_qos = default_qos


def test_set_get_listener(shared_participant):
    assert shared_participant.listener is None
    l = dds.NoOpDomainParticipantListener()
    shared_participant.listener = l
    assert shared_participant.listener == l
    shared_participant.set_listener(None, dds.StatusMask.NONE)
    assert shared_participant.listener is None


def test_participant_register_type(shared_participant):
    @idl.struct
    class TestPoint:
        x: int = 0
        y: int = 0
    try:
        dt = idl.get_type_support(TestPoint).dynamic_type
        assert not shared_participant.is_type_registered("TestPoint")
        shared_participant.register_type("TestPoint", dt)
        assert shared_participant.is_type_registered("TestPoint")
        shared_participant.unregister_type("TestPoint")
        assert not shared_participant.is_type_registered("TestPoint")
    finally:
        # Clean up in case of failure
        shared_participant.unregister_type("TestPoint")


def test_participant_contentfilter(shared_participant):
    class CustomFilterType(dds.DynamicData.ContentFilter):
        def __init__(self):
            super(CustomFilterType, self).__init__()

        def compile(
            self, expression, parameters, type_code, type_class_name, old_compile_data
        ):
            pass

        def evaluate(self, compile_data, sample, meta_data):
            pass

        def finalize(self, compile_data):
            pass

    filter_name = "CustomFilter"
    assert shared_participant.find_contentfilter(filter_name) is None
    shared_participant.register_contentfilter(CustomFilterType(), filter_name)
    assert shared_participant.find_contentfilter(filter_name) is not None
    assert type(shared_participant.find_contentfilter(
        filter_name)) is CustomFilterType
    assert filter_name in shared_participant.find_registered_content_filters()
    shared_participant.unregister_contentfilter(filter_name)
    assert shared_participant.find_contentfilter(filter_name) is None


def test_already_closed_exception():
    p = create_participant()
    p.close()
    with pytest.raises(dds.AlreadyClosedError):
        p.contains_entity(dds.InstanceHandle())
    with pytest.raises(dds.AlreadyClosedError):
        p.listener
    with pytest.raises(dds.AlreadyClosedError):
        p.set_listener(None, dds.StatusMask.NONE)
    with pytest.raises(dds.AlreadyClosedError):
        p.qos
    with pytest.raises(dds.AlreadyClosedError):
        p.domain_id
    with pytest.raises(dds.AlreadyClosedError):
        p.assert_liveliness()
    with pytest.raises(dds.AlreadyClosedError):
        p.current_time
    with pytest.raises(dds.AlreadyClosedError):
        p.default_subscriber_qos
    with pytest.raises(dds.AlreadyClosedError):
        p.default_publisher_qos
    with pytest.raises(dds.AlreadyClosedError):
        p.default_topic_qos
    with pytest.raises(dds.AlreadyClosedError):
        p.default_datareader_qos
    with pytest.raises(dds.AlreadyClosedError):
        p.default_datawriter_qos
    with pytest.raises(dds.AlreadyClosedError):
        p.default_publisher_qos = dds.PublisherQos()
    with pytest.raises(dds.AlreadyClosedError):
        p.default_subscriber_qos = dds.SubscriberQos()
    with pytest.raises(dds.AlreadyClosedError):
        p.default_topic_qos = dds.TopicQos()
    with pytest.raises(dds.AlreadyClosedError):
        p.default_datareader_qos = dds.DataReaderQos()
    with pytest.raises(dds.AlreadyClosedError):
        p.default_datawriter_qos = dds.DataWriterQos()


def test_current_time(shared_participant):
    t = shared_participant.current_time
    assert t != dds.Time.invalid
    assert t > dds.Time(1, 0)


def test_assert_liveliness(shared_participant):
    shared_participant.assert_liveliness()


def test_add_remove_peer(shared_participant):
    shared_participant.add_peer("udpv4://")
    shared_participant.add_peers(["udpv4://"])
    with log_capture.expected_exception(dds.InvalidArgumentError):
        shared_participant.add_peer("")
    shared_participant.remove_peer("udpv4://")
    shared_participant.remove_peers(["udpv4://"])
    with log_capture.expected_exception(dds.InvalidArgumentError):
        shared_participant.remove_peer("")


def test_durable_subscription(shared_participant):
    t = dds.Topic(shared_participant, "TestTopic", String)
    shared_participant.register_durable_subscription(
        dds.EndpointGroup("role_name", 5), "TestTopic")
    shared_participant.delete_durable_subscription(
        dds.EndpointGroup("role_name", 5))


def test_get_readers_and_subscribers(shared_participant: dds.DomainParticipant):
    assert type(shared_participant.builtin_subscriber) is dds.Subscriber
    assert type(
        shared_participant.participant_reader) is dds.ParticipantBuiltinTopicData.DataReader
    assert type(
        shared_participant.publication_reader) is dds.PublicationBuiltinTopicData.DataReader
    assert type(
        shared_participant.subscription_reader) is dds.SubscriptionBuiltinTopicData.DataReader
    # TODO: This causes an error
    #assert type(shared_participant.service_request_reader) is dds.ServiceRequest.DataReader


def test_domain_participant_config_params():
    default_params = dds.DomainParticipantConfigParams()
    assert (
        default_params.domain_id
        == dds.DomainParticipantConfigParams.DOMAIN_ID_USE_XML_CONFIG
    )
    assert (
        default_params.participant_name
        == dds.DomainParticipantConfigParams.ENTITY_NAME_USE_XML_CONFIG
    )
    assert (
        default_params.participant_qos_library_name
        == dds.DomainParticipantConfigParams.QOS_ELEMENT_NAME_USE_XML_CONFIG
    )
    assert (
        default_params.participant_qos_profile_name
        == dds.DomainParticipantConfigParams.QOS_ELEMENT_NAME_USE_XML_CONFIG
    )
    assert (
        default_params.domain_entity_qos_library_name
        == dds.DomainParticipantConfigParams.QOS_ELEMENT_NAME_USE_XML_CONFIG
    )
    assert (
        default_params.domain_entity_qos_profile_name
        == dds.DomainParticipantConfigParams.QOS_ELEMENT_NAME_USE_XML_CONFIG
    )


def test_find_extensions():
    p_qos = get_test_participant_qos()
    p_qos << dds.EntityName("MyParticipant1")
    assert p_qos.participant_name.name == "MyParticipant1"

    with dds.DomainParticipant(get_test_domain(), p_qos) as p1:
        assert p1.qos.participant_name.name == "MyParticipant1"

        p_qos << dds.EntityName("MyParticipant2")
        with dds.DomainParticipant(get_test_domain(), p_qos) as p2:
            assert dds.DomainParticipant.find("MyParticipant1") == p1
            assert dds.DomainParticipant.find("MyParticipant2") == p2
            assert dds.DomainParticipant.find("MyParticipant3") is None


def test_find_flow_controller(shared_participant: dds.DomainParticipant):
    fc = dds.FlowController(shared_participant, "MyFlowController")
    assert fc == shared_participant.find_flow_controller("MyFlowController")


def test_get_participant_protocol_status(shared_participant: dds.DomainParticipant):
    assert type(
        shared_participant.participant_protocol_status) is dds.DomainParticipantProtocolStatus
    assert shared_participant.participant_protocol_status.corrupted_rtps_message_count == 0


def test_resume_endpoint_discovery():
    qos = get_test_participant_qos()
    qos.discovery.enable_endpoint_discovery = False

    p1 = dds.DomainParticipant(get_test_domain(), qos)
    p2 = dds.DomainParticipant(get_test_domain(), qos)
    assert p2.qos.discovery.enable_endpoint_discovery is False
    t1 = dds.Topic(p1, "Topic Endpoint Discovery", String)
    t2 = dds.Topic(p2, "Topic Endpoint Discovery", String)

    reader_qos = dds.DataReaderQos()
    reader_qos.durability.kind = dds.DurabilityKind.TRANSIENT_LOCAL
    reader_qos.history.kind = dds.HistoryKind.KEEP_ALL
    reader_qos.reliability.kind = dds.ReliabilityKind.RELIABLE

    writer_qos = dds.DataWriterQos()
    writer_qos.durability.kind = dds.DurabilityKind.TRANSIENT_LOCAL
    writer_qos.history.kind = dds.HistoryKind.KEEP_ALL
    writer_qos.reliability.kind = dds.ReliabilityKind.RELIABLE

    writer = dds.DataWriter(dds.Publisher(p1), t1, writer_qos)
    reader = dds.DataReader(dds.Subscriber(p2), t2, reader_qos)

    sleep(.5)
    assert reader.subscription_matched_status.total_count == 0
    assert writer.publication_matched_status.total_count == 0

    p1.resume_endpoint_discovery(p2.instance_handle)

    sleep(.5)
    assert reader.subscription_matched_status.total_count == 0
    assert writer.publication_matched_status.total_count == 0

    p2.resume_endpoint_discovery(p1.instance_handle)
    wait.until_equal(1, lambda: reader.subscription_matched_status.total_count)
    wait.until_equal(1, lambda: writer.publication_matched_status.total_count)


def test_participant_factory_qos_to_string():
    the_qos = dds.DomainParticipantFactoryQos()
    # No qos print format
    assert str(the_qos) != ""
    assert "<participant_factory_qos>" in str(the_qos)
    assert "</participant_factory_qos>" in str(the_qos)
    min_length = len(str(the_qos))
    # Do we support qos format options?
    the_qos << dds.EntityFactory(False)
    assert min_length != len(str(the_qos))
    assert "<entity_factory>" in str(the_qos)


def test_domain_participant_qos_to_string():
    the_qos = get_test_participant_qos()
    assert str(the_qos) != ""
    assert "<domain_participant_qos>" in str(the_qos)
    assert "</domain_participant_qos>" in str(the_qos)
    min_length = len(str(the_qos))
    the_qos << dds.EntityFactory(False)
    assert min_length != len(str(the_qos))
    assert "<entity_factory>" in str(the_qos)


def test_close_after_close():
    p1 = create_participant()
    p1.close()
    with pytest.raises(dds.AlreadyClosedError):
        p1.close()

    # Shouldn't error out here
    with create_participant() as p2:
        p2.close()

    with pytest.raises(dds.AlreadyClosedError):
        with create_participant() as p3:
            p3.close()
            p3.qos.participant_name.name


def test_banish_ignored_participants(participant):
    with log_capture.expected_exception(dds.PreconditionNotMetError) as errinfo:
        participant.banish_ignored_participants()
    assert "participant has not enabled security" in errinfo.logs
