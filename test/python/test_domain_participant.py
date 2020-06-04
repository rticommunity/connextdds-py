import rti.connextdds as dds
import pytest
import time

DOMAIN_ID = 0

# def test_participant_qos_value_type():
#    pass

# def test_participant_factory_qos_value_type():
#    pass


def test_participant_default_creation():
    p = dds.DomainParticipant(DOMAIN_ID)
    assert p.domain_id == DOMAIN_ID


# def test_participant_creation_w_default_qos():
#    p = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos())
#    assert p.domain_id = DOMAIN_ID
#    default_qos << p.qos.policy
#    default_qos.domain_participant_resource_limits.writer_property_string_max_length =
#    Need to find writer_property_string


def test_participant_creation_w_qos():
    user_data_values = [10, 11, 12, 13, 14, 15]
    qos = dds.DomainParticipantQos()
    qos.user_data.value = user_data_values
    qos.database.shutdown_cleanup_period = dds.Duration.from_milliseconds(100)
    p = dds.DomainParticipant(DOMAIN_ID, qos)
    retrieved_qos = p.qos
    assert retrieved_qos.user_data.value == user_data_values
    assert (
        retrieved_qos.database.shutdown_cleanup_period
        == dds.Duration.from_milliseconds(100)
    )


def test_participant_creation_w_listener():
    l = dds.NoOpDomainParticipantListener()
    p = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos(), l)
    assert p.listener == l
    p.close()


def test_participant_creation_failure():
    with pytest.raises(dds.Error):
        qos = dds.DomainParticipantQos()
        qos.domain_participant_resource_limits.type_object_max_serialized_length = -2
        dds.DomainParticipant(0, qos)


# def test_set_get_qos():
#    pass


def test_set_qos_exception():
    with pytest.raises(dds.ImmutablePolicyError):
        p = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos())
        qos = p.qos
        d = dds.Duration(77)
        db = dds.Database()
        db.shutdown_cleanup_period = d
        qos << db
        p.qos = qos


# def test_set_get_default_qos():
#    pass

# def test_set_get_factory_qos():
#    pass


def test_set_get_listener():
    p = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos())
    assert p.listener == None

    l = dds.NoOpDomainParticipantListener()
    p.bind_listener(l, dds.StatusMask.all())
    assert p.listener == l

    p.bind_listener(None, dds.StatusMask.none())
    assert p.listener == None


# def test_reference_assignment():
#    pass


def test_find():
    id1 = DOMAIN_ID
    id2 = DOMAIN_ID + 1

    p = dds.DomainParticipant(id1, dds.DomainParticipantQos())
    assert p.delegate.use_count == 1
    found_p = dds.DomainParticipant.find(id1)
    not_found_p = dds.DomainParticipant.find(id2)
    assert found_p == p
    assert found_p.delegate == p.delegate
    assert not_found_p == None

    assert p.delegate == 2


def test_close():
    p = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos())
    assert dds.DomainParticipant.find(DOMAIN_ID) == p
    p.close()
    assert dds.DomainParticipant.find(DOMAIN_ID) == None


def test_already_closed_exception():
    p = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos())
    p.close()
    with pytest.raises(dds.AlreadyClosedError):
        p.contains_entity(dds.InstanceHandle())

    with pytest.raises(dds.AlreadyClosedError):
        p.listener

    with pytest.raises(dds.AlreadyClosedError):
        p.bind_listener(None, dds.StatusMask.none())

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


# def test_retain():
# id1 = DOMAIN_ID
# id2 = DOMAIN_ID + 1
# How to handle scopes?

# def test_creation_from_native():
#    pass


def test_current_time():
    p = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos())
    t = p.current_time
    assert t != dds.Time.invalid()
    assert t > dds.Time(1, 0)


def test_assert_liveliness():
    p = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos())
    p.assert_liveliness()


def test_ignore():
    p1 = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos())
    p2 = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos())
    dds.DomainParticipant.ignore_participant(p1, p2.instance_handle)


def test_add_peer():
    p = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos())
    p.add_peer("udpv4://")
    with pytest.raises(dds.InvalidArgumentError):
        p.add_peer("")


def test_remove_peer():
    p = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos())
    p.remove_peer("udpv4://")
    with pytest.raises(dds.InvalidArgumentError):
        p.remove_peer("")


# def test_domain_participant_config_params():
#    pass

# def test_find_extensions():
#    pass


def test_entity_lock():
    p = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos())
    # Check how to get lock


# def test_retain_for_listener():
#    pass

# def test_discovered_participants():
# qos = dds.DomainParticipantQos()
# p1 = dds.DomainParticipant(DOMAIN_ID, qos << dds.EntityName("p1"))
# p2 = dds.DomainParticipant(DOMAIN_ID, qos << dds.EntityName("p2"))
# p3 = dds.DomainParticipant(DOMAIN_ID, qos << dds.EntityName("p3"))
# time.sleep(3)
# assert p


# def test_dns_polling_period():
# p = dds.DomainParticipant(DOMAIN_ID, dds.DomainParticipantQos(), None)
# assert p != None
# dur = dds.Duration(42, 12)
# p.
# need to find dns functions


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
    the_qos = dds.DomainParticipantQos()
    assert str(the_qos) != ""
    assert "<participant_qos>" in str(the_qos)
    assert "</participant_qos>" in str(the_qos)
    min_length = len(str(the_qos))
    the_qos << dds.EntityFactory(False)
    assert min_length != len(str(the_qos))
    assert "<entity_factory>" in str(the_qos)
