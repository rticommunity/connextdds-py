import rti.connextdds as dds
import time
import argparse


def publisher_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)

    cft_type = dds.QosProvider('cft.xml').type('cft_lib', 'cft')
    topic = dds.DynamicData.Topic(participant, 'Example cft', cft_type)

    writer_qos = dds.QosProvider.default().datawriter_qos
    writer = dds.DynamicData.DataWriter(dds.Publisher(participant), topic, writer_qos)

    instance = dds.DynamicData(cft_type)
    handle = dds.InstanceHandle.nil()

    count = 0
    while (sample_count == 0) or (count < sample_count):
        instance['count'] = count
        instance['x'] = count % 10
        print('Writing cft, count={}\tx={}'.format(instance['count'], instance['x']))
        writer.write(instance, handle)
        time.sleep(1)
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
