import rti.connextdds as dds
import ccf
import time
import argparse


def publisher_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)
    participant.register_contentfilter(ccf.CustomFilterType(), 'CustomFilter')

    ccf_type = dds.QosProvider('ccf.xml').type('ccf_lib', 'Foo')
    topic = dds.DynamicData.Topic(participant, 'Example ccf', ccf_type)

    writer = dds.DynamicData.DataWriter(dds.Publisher(participant), topic)

    instance = dds.DynamicData(ccf_type)

    count = 0
    while (sample_count == 0) or (count < sample_count):
        print('Writing ccf, count={}'.format(count))
        instance['x'] = count
        writer.write(instance)
        time.sleep(1)
        count += 1


parser = argparse.ArgumentParser(description='RTI Connext DDS Example: Using Custom Filters (Publisher)')
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
