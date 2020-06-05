"""
 (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 """

import rti.connextdds as dds
import time
import argparse
import os
import sys

try:
    xrange
except NameError:
    xrange = range


# A Listenener class for DDS Participant data that also enforces "password" authentication
class BuiltinParticipantListener(dds.ParticipantBuiltinTopicData.NoOpDataReaderListener):
    def __init__(self, auth='password'):
        super(BuiltinParticipantListener, self).__init__()
        self.expected_password = auth

    def on_data_available(self, reader):
        # only process previously unseen Participants
        with reader.select().state(dds.DataState.new_instance()).take() as samples:
            for sample in filter(lambda s: s.info.valid, samples):
                # Convert Participant user data to a string
                user_data = sample.data.user_data.value
                user_auth = ''.join((chr(c) for c in user_data))
                key = sample.data.key

                print('Built-in Reader: found participant')
                print("\tkey->'{:08X} {:08X} {:08X}'".format(*key.value[:3]))
                print("\tuser_data->'{}'".format(user_auth))
                print('\tinstance_handle: {}'.format(sample.info.instance_handle))

                # Check if the password match.Otherwise, ignore the participant.
                if user_auth != self.expected_password:
                    print('Bad authorization, ignoring participant')
                    participant = reader.subscriber.participant
                    participant.ignore_participant(sample.info.instance_handle)


# Create a Subscription listener, print discovered DataReader information
class BuiltinSubscriptionListener(dds.SubscriptionBuiltinTopicData.NoOpDataReaderListener):
    def __init__(self):
        super(BuiltinSubscriptionListener, self).__init__()

    def on_data_available(self, reader):
        # only process previously unseen DataReaders
        with reader.select().state(dds.DataState.new_instance()).take() as samples:
            for sample in filter(lambda s: s.info.valid, samples):
                participant_key = sample.data.participant_key
                key = sample.data.key

                print('Built-in Reader: found subscriber')
                print("\tparticipant_key->'{:08X} {:08X} {:08X}'".format(*participant_key.value[0:3]))
                print("\tkey->'{:08X} {:08X} {:08X}'".format(*key.value[0:3]))
                print('instance_handle: {}'.format(sample.info.instance_handle))


def publisher_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)

    # Participant properties give access to the builtin readers
    participant.participant_reader.bind_listener(
        BuiltinParticipantListener(),
        dds.StatusMask.data_available())

    participant.subscription_reader.bind_listener(
        BuiltinSubscriptionListener(),
        dds.StatusMask.data_available())

    participant.enable()

    msg_type = dds.QosProvider('msg.xml').type('builtin_topics_lib', 'msg')
    topic = dds.DynamicData.Topic(participant, 'Example msg', msg_type)
    writer = dds.DynamicData.DataWriter(dds.Publisher(participant), topic)
    instance = dds.DynamicData(msg_type)

    # write samples in a loop, incrementing the 'x' field
    count = 0
    while (sample_count == 0) or (count < sample_count):
        time.sleep(1)
        instance['x'] = count
        writer.write(instance, dds.InstanceHandle())
        count += 1
    assert count == 5


# A listener for msg samples
class MsgListener(dds.DynamicData.NoOpDataReaderListener):
    def on_data_available(self, reader):
        with reader.take() as samples:
            # Use a filter to process only valid samples
            count = 0
            for sample in filter(lambda s: s.info.valid, samples):
                print(sample.data)
                count +=1

            assert count == 5


def subscriber_main(domain_id, sample_count, participant_auth):
    participant_qos = dds.QosProvider.default().participant_qos
    resource_limits_qos = participant_qos.domain_participant_resource_limits
    max_participant_user_data = resource_limits_qos.participant_user_data_max_length

    # Set the participant auth string as user data bytes
    if len(participant_auth) > max_participant_user_data:
        raise ValueError('participant user data exceeds resource limits')
    else:
        participant_qos.user_data.value = bytearray(participant_auth.encode())

    participant = dds.DomainParticipant(domain_id, participant_qos)
    participant.enable()

    msg_type = dds.QosProvider('msg.xml').type('builtin_topics_lib', 'msg')
    topic = dds.DynamicData.Topic(participant, 'Example msg', msg_type)
    reader = dds.DynamicData.DataReader(dds.Subscriber(participant), topic)
    reader.bind_listener(MsgListener(), dds.StatusMask.data_available())

    count = 0
    while (sample_count == 0) or (count < sample_count):
        time.sleep(1)
        count += 1


#def test_builtin():
#    pid = os.fork()
#    if pid == 0:
#        subscriber_main(0, 5, "password")
#        return
#    else:
#        publisher_main(0, 5)
