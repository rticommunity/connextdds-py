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
from test_utils.fixtures import create_participant, get_test_domain, get_test_participant_qos, wait, participant
from test_utils import log_capture


def test_participant_default_creation(participant):
    assert participant.domain_id == get_test_domain()


def test_participant_creation_w_default_qos(participant):
    assert participant.domain_id == get_test_domain()
    event_count = participant.qos.event.max_count
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


def test_set_get_qos(participant):
    qos = participant.qos
    assert qos.entity_factory.autoenable_created_entities
    qos << dds.EntityFactory.manually_enable
    assert not (qos.entity_factory.autoenable_created_entities)
    participant.qos = qos
    # assert qos == p.qos
    assert not (qos.entity_factory.autoenable_created_entities)
    qos << dds.EntityFactory.auto_enable
    participant << qos
    retrieved_qos = get_test_participant_qos()
    participant >> retrieved_qos
    assert retrieved_qos.entity_factory.autoenable_created_entities


def test_set_qos_exception(participant):
    qos = participant.qos
    d = dds.Duration(77)
    db = dds.Database()
    db.shutdown_cleanup_period = d
    qos << db
    with log_capture.expected_exception(dds.ImmutablePolicyError):
        participant.qos = qos


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


def test_set_get_listener(participant):
    assert participant.listener is None
    l = dds.NoOpDomainParticipantListener()
    participant.listener = l
    assert participant.listener == l
    participant.set_listener(None, dds.StatusMask.NONE)
    assert participant.listener is None


def test_find(participant):
    id1 = get_test_domain()
    id2 = get_test_domain() + 1
    found_p = dds.DomainParticipant.find(id1)
    not_found_p = dds.DomainParticipant.find(id2)
    assert found_p == participant
    assert not_found_p is None


def test_close():
    assert dds.DomainParticipant.find(get_test_domain()) is None
    with create_participant() as p:
        assert dds.DomainParticipant.find(get_test_domain()) == p
        p.close()
    assert dds.DomainParticipant.find(get_test_domain()) is None


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


def test_retain():
    id1 = get_test_domain()
    id2 = get_test_domain() + 1

    def scope_1():
        p1 = dds.DomainParticipant(id1, get_test_participant_qos())
        p2 = dds.DomainParticipant(id2, get_test_participant_qos())
        p1.retain()

    def scope_2():
        p3 = dds.DomainParticipant.find(id1)
        p4 = dds.DomainParticipant.find(id2)
        assert p3 != None
        assert p4 is None
        assert p3 == dds.DomainParticipant.find(id1)

    def scope_3():
        p5 = dds.DomainParticipant.find(id1)
        assert p5 != None
        p5.close()

    scope_1()
    scope_2()
    scope_3()
    assert dds.DomainParticipant.find(id1) is None


def test_current_time(participant):
    t = participant.current_time
    assert t != dds.Time.invalid
    assert t > dds.Time(1, 0)


def test_assert_liveliness(participant):
    participant.assert_liveliness()


def test_ignore():
    with create_participant() as p1, create_participant() as p2:
        dds.DomainParticipant.ignore_participant(p1, p2.instance_handle)


def test_add_remove_peer(participant):
    participant.add_peer("udpv4://")
    with log_capture.expected_exception(dds.InvalidArgumentError):
        participant.add_peer("")
    participant.remove_peer("udpv4://")
    with log_capture.expected_exception(dds.InvalidArgumentError):
        participant.remove_peer("")


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


@pytest.mark.parametrize("set_after", [True, False])
def test_retain_for_listener(set_after):
    listener = dds.NoOpDomainParticipantListener()
    if set_after:
        p = create_participant()
        p.set_listener(listener, dds.StatusMask.NONE)
    else:
        p = dds.DomainParticipant(
            get_test_domain(), get_test_participant_qos(), listener)

    def inner():
        with dds.DomainParticipant.find(get_test_domain()) as new_p:
            assert new_p != None
            new_p.set_listener(None, dds.StatusMask.NONE)

    inner()
    assert dds.DomainParticipant.find(get_test_domain()) is None


def test_discovered_participants():
    qos = get_test_participant_qos()
    with dds.DomainParticipant(
            get_test_domain(),
            qos << dds.EntityName("p1")) as p1, dds.DomainParticipant(
                get_test_domain(),
                qos << dds.EntityName("p2")) as p2, dds.DomainParticipant(
                    get_test_domain(),
                    qos << dds.EntityName("p3")) as p3:
        # Wait for them to discover each other
        wait.until_equal(2, lambda: len(p1.discovered_participants()))
        wait.until_equal(2, lambda: len(p2.discovered_participants()))
        wait.until_equal(2, lambda: len(p3.discovered_participants()))

        assert p2.instance_handle in p1.discovered_participants()
        assert p3.instance_handle in p1.discovered_participants()
        assert p1.instance_handle in p2.discovered_participants()
        assert p3.instance_handle in p2.discovered_participants()
        assert p1.instance_handle in p3.discovered_participants()
        assert p2.instance_handle in p3.discovered_participants()


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


def test_find_no_entity(participant):
    # Testing issue from PY-49
    assert participant.find_subscriber("foo") is None
    assert participant.find_subscribers() == []
    assert participant.find_publisher("foo") is None
    assert participant.find_publishers() == []
    assert participant.find_datawriter("foo") is None
    assert participant.find_datareader("foo") is None
    assert dds.Publisher(participant).find_datawriter("foo") is None
    assert dds.Subscriber(participant).find_datareader("foo") is None
    assert dds.Publisher(
        participant).find_datawriter_by_topic_name("foo") is None
    assert dds.Subscriber(
        participant).find_datareader_by_topic_name("foo") is None
