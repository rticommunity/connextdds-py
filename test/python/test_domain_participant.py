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
import utils
from test_utils.fixtures import *
from test_utils import log_capture

# def test_participant_qos_value_type():
#    pass

# def test_participant_factory_qos_value_type():
#    pass


def test_participant_default_creation():
    p = dds.DomainParticipant(get_test_domain())
    assert p.domain_id == get_test_domain()


def test_participant_creation_w_default_qos():
    p = dds.DomainParticipant(get_test_domain(), get_test_participant_qos())
    assert p.domain_id == get_test_domain()
    event_count = p.qos.event.max_count
    assert event_count == dds.Event().max_count


def test_participant_creation_w_qos():
    user_data_values = [10, 11, 12, 13, 14, 15]
    qos = get_test_participant_qos()
    qos.user_data.value = user_data_values
    qos.database.shutdown_cleanup_period = dds.Duration.from_milliseconds(100)
    p = dds.DomainParticipant(get_test_domain(), qos)
    retrieved_qos = p.qos
    assert retrieved_qos.user_data.value == user_data_values
    assert (
        retrieved_qos.database.shutdown_cleanup_period
        == dds.Duration.from_milliseconds(100)
    )


def test_participant_creation_w_listener():
    l = dds.NoOpDomainParticipantListener()
    p = dds.DomainParticipant(get_test_domain(), get_test_participant_qos(), l)
    assert p.listener == l
    p.close()


def test_participant_creation_failure():
    qos = get_test_participant_qos()
    qos.resource_limits.type_object_max_serialized_length = -2
    with log_capture.expected_exception(dds.Error):
        dds.DomainParticipant(0, qos)

def test_set_get_qos():
    # Use PY-26: participant fixture from
    p = utils.create_participant() # PY-26: Use participant fixture from test_utils.fixtures instead
    qos = p.qos
    assert qos.entity_factory.autoenable_created_entities
    qos << dds.EntityFactory.manually_enable
    assert not (qos.entity_factory.autoenable_created_entities)
    p.qos = qos
    # assert qos == p.qos
    assert not (qos.entity_factory.autoenable_created_entities)
    qos << dds.EntityFactory.auto_enable
    p << qos
    retrieved_qos = get_test_participant_qos()
    p >> retrieved_qos
    assert retrieved_qos.entity_factory.autoenable_created_entities


@pytest.mark.skip("PY-26: use test qos")
def test_set_qos_exception():
    p = dds.DomainParticipant(get_test_domain(), get_test_participant_qos())
    qos = p.qos
    d = dds.Duration(77)
    db = dds.Database()
    db.shutdown_cleanup_period = d
    qos << db
    with pytest.raises(dds.ImmutablePolicyError):
        p.qos = qos


@pytest.mark.skip("PY-26: reset default qos")
def test_set_get_default_qos():
    set_qos = dds.DomainParticipant.default_participant_qos
    set_qos << dds.UserData([33, 33])
    assert set_qos != dds.DomainParticipant.default_topic_qos
    dds.DomainParticipant.default_participant_qos = set_qos
    get_qos = dds.DomainParticipant.default_participant_qos
    assert get_qos == set_qos
    assert get_qos.user_data == dds.UserData([33, 33])


@pytest.mark.skip("PY-26: reset default qos")
def test_set_get_factory_qos():
    default_qos = dds.DomainParticipantFactoryQos()
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


def test_set_get_listener():
    p = utils.create_participant() # PY-26: Use participant fixture from test_utils.fixtures instead
    assert p.listener is None
    l = dds.NoOpDomainParticipantListener()
    p.listener = l
    assert p.listener == l
    p.set_listener(None, dds.StatusMask.NONE)
    assert p.listener is None


def test_find():
    id1 = get_test_domain()
    id2 = get_test_domain() + 1
    p = utils.create_participant() # PY-26: Use participant fixture from test_utils.fixtures instead
    found_p = dds.DomainParticipant.find(id1)
    not_found_p = dds.DomainParticipant.find(id2)
    assert found_p == p
    assert not_found_p is None


def test_close():
    assert dds.DomainParticipant.find(get_test_domain()) is None
    p = utils.create_participant() # PY-26: Use participant fixture from test_utils.fixtures instead
    assert dds.DomainParticipant.find(get_test_domain()) == p
    p.close()
    assert dds.DomainParticipant.find(get_test_domain()) is None


def test_already_closed_exception():
    p = utils.create_participant() # PY-26: Use participant fixture from test_utils.fixtures instead
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


@pytest.mark.skip("PY-26: use test qos")
def test_retain():
    id1 = get_test_domain()
    id2 = get_test_domain() + 1

    def scope_1():
        p1 = utils.create_participant()
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


def test_current_time():
    p = utils.create_participant() # PY-26: Use participant fixture from test_utils.fixtures instead
    t = p.current_time
    assert t != dds.Time.invalid
    assert t > dds.Time(1, 0)


def test_assert_liveliness():
    p = utils.create_participant() # PY-26: Use participant fixture from test_utils.fixtures instead
    p.assert_liveliness()


def test_ignore():
    p1 = utils.create_participant()
    p2 = utils.create_participant()
    dds.DomainParticipant.ignore_participant(p1, p2.instance_handle)


def test_add_peer():
    p = utils.create_participant() # PY-26: Use participant fixture from test_utils.fixtures instead
    p.add_peer("udpv4://")
    with log_capture.expected_exception(dds.InvalidArgumentError):
        p.add_peer("")


def test_remove_peer():
    p = utils.create_participant() # PY-26: Use participant fixture from test_utils.fixtures instead
    p.remove_peer("udpv4://")
    with log_capture.expected_exception(dds.InvalidArgumentError):
        p.remove_peer("")


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


@pytest.mark.skip("PY-26: use test qos")
def test_find_extensions():
    p_qos = get_test_participant_qos()
    p_qos << dds.EntityName("MyParticipant1")
    assert p_qos.participant_name.name == "MyParticipant1"

    p1 = dds.DomainParticipant(get_test_domain(), p_qos)
    assert p1.qos.participant_name.name == "MyParticipant1"

    p_qos << dds.EntityName("MyParticipant2")
    p2 = dds.DomainParticipant(get_test_domain(), p_qos)

    assert dds.DomainParticipant.find("MyParticipant1") == p1
    assert dds.DomainParticipant.find("MyParticipant2") == p2
    assert dds.DomainParticipant.find("MyParticipant3") is None


@pytest.mark.skip("PY-26: use test qos")
@pytest.mark.parametrize("set_after", [True, False])
def test_retain_for_listener(set_after):
    listener = dds.NoOpDomainParticipantListener()
    if set_after:
        p = utils.create_participant() # PY-26: Use participant fixture from test_utils.fixtures instead
        p.set_listener(listener, dds.StatusMask.NONE)
    else:
        p = dds.DomainParticipant(get_test_domain(), get_test_participant_qos(), listener)

    def inner():
        with dds.DomainParticipant.find(get_test_domain()) as new_p:
            assert new_p != None
            new_p.set_listener(None, dds.StatusMask.NONE)

    inner()
    assert dds.DomainParticipant.find(get_test_domain()) is None

# PY-26: does this need to be added back?
#
# def test_discovered_participants():
# qos = get_test_participant_qos()
# p1 = dds.DomainParticipant(get_test_domain(), qos << dds.EntityName("p1"))
# p2 = dds.DomainParticipant(get_test_domain(), qos << dds.EntityName("p2"))
# p3 = dds.DomainParticipant(get_test_domain(), qos << dds.EntityName("p3"))
# time.sleep(3)
# assert p
# def test_dns_polling_period():
# p = dds.DomainParticipant(get_test_domain(), get_test_participant_qos(), None)
# assert p != None
# dur = dds.Duration(42, 12)
# p.
# need to find dns functions


@pytest.mark.skip(reason="to_string not implemented yet")
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


@pytest.mark.skip(reason="to_string not implemented yet")
def test_domain_participant_qos_to_string():
    the_qos = get_test_participant_qos()
    assert str(the_qos) != ""
    assert "<participant_qos>" in str(the_qos)
    assert "</participant_qos>" in str(the_qos)
    min_length = len(str(the_qos))
    the_qos << dds.EntityFactory(False)
    assert min_length != len(str(the_qos))
    assert "<entity_factory>" in str(the_qos)


def test_close_after_close():
    p1 = utils.create_participant()
    p1.close()
    with pytest.raises(dds.AlreadyClosedError):
        p1.close()

    # Shouldn't error out here
    with utils.create_participant() as p2:
        p2.close()

    with pytest.raises(dds.AlreadyClosedError):
        with utils.create_participant() as p3:
            p3.close()
            p3.qos.participant_name.name
