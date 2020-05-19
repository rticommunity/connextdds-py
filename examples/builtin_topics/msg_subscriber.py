import rti.connextdds as dds
import time
import argparse

try:
    xrange
except NameError:
    xrange = range

class MsgListener(dds.DynamicData.NoOpDataReaderListener):
    def on_data_available(self, reader):
        with reader.take() as samples:
            # can use a take_valid or read_valid in 6.0.x, use generator expression for 5.3.1
            for sample in (s for s in samples if s.info.valid):
                print(sample.data)


def subscriber_main(domain_id, sample_count, participant_auth):
    participant_qos = dds.QosProvider.default().participant_qos
    resource_limits_qos = participant_qos.domain_participant_resource_limits
    max_participant_user_data = resource_limits_qos.participant_user_data_max_length

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


parser = argparse.ArgumentParser(description='RTI Connext DDS Example: Using Builtin Topics (Subscriber)')
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
parser.add_argument(
    '-a',
    '--auth',
    type=str,
    default='password',
    help='Default authorization string')

args = parser.parse_args()
#assert(0 <= args.domain < 233)
#assert(args.count >= 0)

subscriber_main(args.domain, args.count, args.auth)
