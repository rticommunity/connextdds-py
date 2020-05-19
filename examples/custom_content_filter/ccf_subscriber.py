import rti.connextdds as dds
import ccf
import time
import argparse
import textwrap


class CcfListener(dds.DynamicData.NoOpDataReaderListener):
    def on_data_available(self, reader):
        with reader.take() as samples:
            for sample in (s for s in samples if s.info.valid):
                print(sample.data)


def subscriber_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)

    ccf_type = dds.QosProvider('ccf.xml').type('ccf_lib', 'Foo')
    topic = dds.DynamicData.Topic(participant, 'Example ccf', ccf_type)

    participant.register_contentfilter(ccf.CustomFilterType(), 'CustomFilter')

    custom_filter = dds.Filter('%0 %1 x', ['2', 'divides'])
    custom_filter.name = 'CustomFilter'
    topic = dds.DynamicData.ContentFilteredTopic(
        topic,
        'ContentFilteredTopic',
        custom_filter)

    print('Filter: 2 divides x')

    reader = dds.DynamicData.DataReader(dds.Subscriber(participant), topic)
    reader.bind_listener(CcfListener(), dds.StatusMask.data_available())

    count = 0
    while (sample_count == 0) or (count < sample_count):
        time.sleep(1)

        if count == 10:
            print(textwrap.dedent(
                """
                ==========================
                Changing Filter Parameters
                Filter: 15 greater-than x
                =========================="""))
            topic.filter_parameters = ['15', 'greater-than']
        elif count == 20:
            print(textwrap.dedent(
                """
                ==========================
                Changing Filter Parameters
                Filter: 3 divides x
                =========================="""))
            topic.filter_parameters = ['3', 'divides']

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

subscriber_main(args.domain, args.count)
