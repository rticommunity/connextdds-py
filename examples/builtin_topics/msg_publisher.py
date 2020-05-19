import rti.connextdds as dds
import time
import argparse

try:
    xrange
except NameError:
    xrange = range


class BuiltinParticipantListener(dds.ParticipantBuiltinTopicData.DataReaderListener):
    def __init__(self, auth='password'):
        dds.ParticipantBuiltinTopicData.DataReaderListener.__init__(self)
        self.expected_password = auth

    def on_data_available(self, reader):
        with reader.select().state(dds.DataState.new_instance()).take() as samples:
            # can use a take_valid or read_valid in 6.0.x, use generator expression for 5.3.1
            for sample in (s for s in samples if s.info.valid):
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


class BuiltinSubscriptionListener(dds.SubscriptionBuiltinTopicData.DataReaderListener):
    def __init__(self):
        dds.SubscriptionBuiltinTopicData.DataReaderListener.__init__(self)

    def on_data_available(self, reader):
        with reader.select().state(dds.DataState.new_instance()).take() as samples:
            for sample in (s for s in samples if s.info.valid):
                participant_key = sample.data.participant_key
                key = sample.data.key

                print('Built-in Reader: found subscriber')
                print("\tparticipant_key->'{:08X} {:08X} {:08X}'".format(*participant_key.value[0:3]))
                print("\tkey->'{:08X} {:08X} {:08X}'".format(*key.value[0:3]))
                print('instance_handle: {}'.format(sample.info.instance_handle))


def publisher_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)

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

    count = 0
    while (sample_count == 0) or (count < sample_count):
        time.sleep(1)
        instance['x'] = count
        writer.write(instance, dds.InstanceHandle())
        count += 1


parser = argparse.ArgumentParser(description='RTI Connext DDS Example: Using Builtin Topics (Publisher)')
parser.add_argument(
    '-d',
    '--domain',
    type=int,
    default=0,
    help='DDS Domain ID')
parser.add_argument(
    '-c',
    '--count',
    type=int,
    default=0,
    help='Number of samples to send')

args = parser.parse_args()
assert(0 <= args.domain < 233)
assert(args.count >= 0)

publisher_main(args.domain, args.count)
