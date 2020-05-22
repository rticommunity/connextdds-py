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


def publisher_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)

    wssc_type = dds.QosProvider('waitset_cond_modern.xml').type('wssc_lib', 'Foo')
    topic = dds.DynamicData.Topic(participant, 'Example Foo', wssc_type)
    writer = dds.DynamicData.DataWriter(dds.Publisher(participant), topic)

    sample = dds.DynamicData(wssc_type)

    count = 0
    while (sample_count == 0) or (count < sample_count):
        print('Writing Foo, count = {}'.format(count))
        sample['x'] = count
        writer.write(sample)

        count += 1
        time.sleep(0.99)


parser = argparse.ArgumentParser(description='RTI Connext DDS Example: Waitsets with Status Condition & Read Condition (Publisher)')
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
assert (0 <= args.domain < 233)
assert (args.count >= 0)

publisher_main(args.domain, args.count)
