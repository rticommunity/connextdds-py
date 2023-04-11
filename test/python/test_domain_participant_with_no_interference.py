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
from test_utils.fixtures import create_participant, get_test_domain, get_test_participant_qos, wait, participant, PubSubFixture
from test_utils import log_capture
from rti.types.builtin import String
from gc import collect

# This file is for when you want to test functions without interference from the
# other tests and shared participants.

# We add this here to force garbage collection for these tests as to ensure
# that there are no participants interfering. Without this, if a test in another
# file fails these could also fail.


@pytest.fixture(autouse=True)
def clean_up(request):
    collect()


def test_find(participant):
    id1 = get_test_domain()
    id2 = get_test_domain() + 1
    found_p = dds.DomainParticipant.find(id1)
    not_found_p = dds.DomainParticipant.find(id2)
    assert found_p == participant
    assert not_found_p is None
    # Find with no id
    assert participant in dds.DomainParticipant.find()

    pub_qos = dds.PublisherQos()
    pub_qos << dds.EntityName('pub1')
    pub1 = dds.Publisher(participant, pub_qos)
    assert pub1 == participant.find_publisher("pub1")

    pub_qos << dds.EntityName('pub2')
    pub2 = dds.Publisher(participant, pub_qos)
    pubs = participant.find_publishers()
    assert len(pubs) == 2
    assert pub1 in pubs
    assert pub2 in pubs

    sub_qos = dds.SubscriberQos()
    sub_qos << dds.EntityName('sub1')
    sub1 = dds.Subscriber(participant, sub_qos)
    assert sub1 == participant.find_subscriber("sub1")

    sub_qos << dds.EntityName('pub2')
    sub2 = dds.Subscriber(participant, sub_qos)
    subs = participant.find_subscribers()
    assert len(subs) == 2
    assert sub1 in subs
    assert sub2 in subs

    topic = dds.Topic(participant, 'TestTopic', String)
    assert topic in participant.find_topics()

    dw_qos = dds.DataWriterQos()
    dw_qos << dds.EntityName('dw')
    dw = dds.DataWriter(pub1, topic, dw_qos)
    assert dw == participant.find_datawriter("pub1::dw")

    dr_qos = dds.DataReaderQos()
    dr_qos << dds.EntityName('dr')
    dr = dds.DataReader(sub1, topic, dr_qos)
    assert dr == participant.find_datareader("sub1::dr")


def test_find_no_datawriter(participant):
    # Testing issue from PY-49
    topic = dds.Topic(participant, 'TestTopic', String)
    dw_qos = dds.DataWriterQos()
    dw_qos << dds.EntityName('dw')
    dw = dds.DataWriter(dds.Publisher(participant), topic, dw_qos)
    assert participant.find_datawriter("foo") is None

    dr_qos = dds.DataReaderQos()
    dr_qos << dds.EntityName('dr')
    dr = dds.DataReader(dds.Subscriber(participant), topic, dr_qos)
    assert participant.find_datareader("foo") is None


def test_close():
    assert dds.DomainParticipant.find(get_test_domain()) is None
    with create_participant() as p:
        assert dds.DomainParticipant.find(get_test_domain()) == p
        p.close()
    assert dds.DomainParticipant.find(get_test_domain()) is None


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
        assert p1.discovered_participant_data(
            p2.instance_handle).participant_name == dds.EntityName("p2")
        for p in p1.discovered_participant_data([p2.instance_handle, p3.instance_handle]):
            assert p.participant_name in [
                dds.EntityName("p2"), dds.EntityName("p3")]


def test_discovered_topics(participant):
    p = participant
    assert type(p.discovered_topics()) is dds.InstanceHandleSeq
    assert len(p.discovered_topics()) == 0
    t1 = dds.Topic(p, 'TestTopic1', String)
    assert len(p.discovered_topics()) == 1
    assert t1.instance_handle in p.discovered_topics()
    assert p.discovered_topic_data(t1.instance_handle).name == 'TestTopic1'
    assert p.discovered_topic_data(t1).name == 'TestTopic1'
    t2 = dds.Topic(p, 'TestTopic2', String)
    assert len(p.discovered_topics()) == 2
    assert t2.instance_handle in p.discovered_topics()
    data_seq = p.discovered_topic_data(dds.InstanceHandleSeq(
        [t1.instance_handle, t2.instance_handle]))
    assert len(data_seq) == 2
    for data in data_seq:
        assert data.name in ['TestTopic1', 'TestTopic2']
    for data in p.discovered_topic_data():
        assert data.name in ['TestTopic1', 'TestTopic2']


def test_participant_subject_name():
    qos = get_test_participant_qos()
    qos.participant_name.name = "p1"
    p1 = dds.DomainParticipant(get_test_domain(), qos)
    qos.participant_name.name = "p2"
    p2 = dds.DomainParticipant(get_test_domain(), qos)
    wait.for_participants_to_match(p1, p2)
    assert p1.discovered_participant_subject_name(
        p2.instance_handle) == p2.qos.participant_name.name
    assert p2.discovered_participant_subject_name(
        p1.instance_handle) == p1.qos.participant_name.name
    participants = p1.discovered_participants_from_subject_name("p2")
    assert len(participants) == 1
    assert participants[0] == p2.instance_handle
    participants = p2.discovered_participants_from_subject_name("p1")
    assert len(participants) == 1
    assert participants[0] == p1.instance_handle


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


def test_discovered_participant_empty_subject_name_outputs_subject_name(participant):
    assert len(participant.discovered_participants()) == 0
    qos = get_test_participant_qos()
    qos.participant_name.name = ""
    p2 = dds.DomainParticipant(get_test_domain(), qos)
    wait.for_participants_to_match(participant, p2)
    assert "" == participant.discovered_participant_subject_name(
        p2.instance_handle)


def test_discovered_participant_null_subject_name_outputs_null(participant):
    assert len(participant.discovered_participants()) == 0
    p2 = create_participant()
    wait.for_participants_to_match(participant, p2)
    assert participant.discovered_participant_subject_name(
        p2.instance_handle) is None


def test_nil_participant_subject_name_throws_argument_exception(participant):
    with log_capture.expected_exception(dds.InvalidArgumentError) as errinfo:
        participant.discovered_participant_subject_name(
            dds.InstanceHandle.nil())
    assert "discovered_participant_handle is NIL" in errinfo.logs


def test_self_participant_subject_name_throws_precondition_not_met_exception(participant):
    with log_capture.expected_exception(dds.PreconditionNotMetError) as errinfo:
        participant.discovered_participant_subject_name(
            participant.instance_handle)
    assert "because the participant was not found" in errinfo.logs


def test_discovered_participants_from_subject_name_contains_remote_participant(participant):
    assert len(participant.discovered_participants()) == 0
    p2 = create_participant()
    qos = get_test_participant_qos()
    qos.participant_name.name = "p3"
    p3 = dds.DomainParticipant(get_test_domain(), qos)
    wait.for_participants_to_match(participant, p2)
    wait.for_participants_to_match(participant, p3)
    wait.for_participants_to_match(p2, p3)
    assert p3.instance_handle in participant.discovered_participants_from_subject_name(
        "p3")
    assert p2.instance_handle not in participant.discovered_participants_from_subject_name(
        "p3")
    assert p2.instance_handle in participant.discovered_participants_from_subject_name(
        None)


@pytest.mark.parametrize("use_list", [True, False])
@pytest.mark.parametrize("entity", [
    dds.DomainParticipant,
    dds.Topic,
    dds.DataReader,
    dds.DataWriter])
def test_ignore(entity, use_list, participant):
    # Create a topic
    topic = dds.Topic(participant, "TestTopic", String)
    system = PubSubFixture(participant, String, topic=topic)
    with create_participant() as ignored_participant:
        ignored_topic = dds.Topic(ignored_participant, "TestTopic", String)
        ignored_system = PubSubFixture(
            ignored_participant, String, topic=ignored_topic)
        wait.until_equal(2, lambda: len(
            ignored_system.reader.matched_publications))
        wait.until_equal(2, lambda: len(system.writer.matched_subscriptions))
        assert system.participant.instance_handle != ignored_system.participant.instance_handle

        if entity == dds.DomainParticipant:
            if not use_list:
                system.participant.ignore_participant(
                    ignored_system.participant.instance_handle)
            else:
                system.participant.ignore_participants(
                    [ignored_system.participant.instance_handle])
        elif entity == dds.Topic:
            if not use_list:
                system.participant.ignore_topic(system.topic.instance_handle)
            else:
                system.participant.ignore_topics(
                    [system.topic.instance_handle])
        elif entity == dds.DataWriter:
            if not use_list:
                system.participant.ignore_datawriter(
                    ignored_system.writer.instance_handle)
            else:
                system.participant.ignore_datawriters(
                    [ignored_system.writer.instance_handle])
            wait.until_equal(1, lambda: len(
                system.reader.matched_publications))
        elif entity == dds.DataReader:
            if not use_list:
                ignored_system.participant.ignore_datareader(
                    system.reader.instance_handle)
            else:
                ignored_system.participant.ignore_datareaders(
                    [system.reader.instance_handle])
            wait.until_equal(1, lambda: len(
                ignored_system.writer.matched_subscriptions))

        ignored_system.writer.write(String("Won't see this"))
        # This should be received
        system.writer.write(String("Will see this!"))

        if entity == dds.Topic:
            # If we're ignoring the topic we won't receive any sample.
            assert len(system.reader.take()) == 0
            return

        wait.for_data(system.reader, 1)
        samples = system.reader.take()
        assert len(samples) == 1
        assert samples[0][0] == String("Will see this!")
