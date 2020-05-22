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
import textwrap
import time
import argparse


class Counter(object):
    def __init__(self):
        self.value = 0


# Handler function for liveliness events
def status_handler(reader):
    status_mask = reader.status_changes
    if dds.StatusMask.liveliness_changed() in status_mask:
        liveliness_status = reader.liveliness_changed_status
        print('Liveliness changed => active writers = {}'.format(
            liveliness_status.alive_count))


# Handle incoming data with a ReadCondition
def rc_handler(reader, count):
    with reader.take() as samples:
        for sample in filter(lambda s: s.info.valid, samples):
            count.value += 1
            print(sample.data)


def subscriber_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)

    wsqc_type = dds.QosProvider('waitset_cond_modern.xml').type('wssc_lib', 'Foo')
    topic = dds.DynamicData.Topic(participant, 'Example Foo', wsqc_type)
    reader_qos = dds.QosProvider.default().datareader_qos
    reader = dds.DynamicData.DataReader(dds.Subscriber(participant), topic, reader_qos)

    # Get the StatusCondition associated with the reader and set the mask to get liveliness updates
    status_condition = dds.StatusCondition(reader)
    status_condition.enabled_statuses = dds.StatusMask.liveliness_changed()

    # Create a ReadCondition to get any data
    read_condition = dds.ReadCondition(reader, dds.DataState.any_data())

    # Create WaitSet and attach conditions
    waitset = dds.WaitSet()
    waitset += status_condition
    waitset += read_condition

    count = Counter()
    while (sample_count == 0) or (count.value < sample_count):
        active = waitset.wait(4.0)
        # Check conditions after wait to see if anything triggered
        if status_condition in active:
            status_handler(reader)
        if read_condition in active:
            rc_handler(reader, count)


parser = argparse.ArgumentParser(description='RTI Connext DDS Example: Waitsets with Status Condition & Read Condition (Subscriber)')
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

subscriber_main(args.domain, args.count)
