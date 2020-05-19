import rti.connextdds as dds
import time
import argparse
import textwrap


class CftListener(dds.DynamicData.NoOpDataReaderListener):
    def on_data_available(self, reader):
        with reader.take() as samples:
            for sample in (s for s in samples if s.info.valid):
                print(sample.data)


def subscriber_main(domain_id, sample_count, is_cft):
    participant = dds.DomainParticipant(domain_id)

    cft_type = dds.QosProvider('cft.xml').type('cft_lib', 'cft')
    topic = dds.DynamicData.Topic(participant, 'Example cft', cft_type)

    if is_cft:
        str_filter = dds.Filter('name MATCH %0', ['SOME_STRING'])
        str_filter.name = dds.Filter.stringmatch_filter_name
        topic = dds.DynamicData.ContentFilteredTopic(
            topic,
            'ContentFilteredTopic',
            str_filter)
        print(textwrap.dedent(
            """
            ==========================
            Using ContentFilteredTopic
            name MATCH %0
            =========================="""))
    else:
        print(textwrap.dedent(
            """
            ==========================
            Using Normal Topic
            =========================="""))

    reader_qos = dds.QosProvider.default().datareader_qos
    reader = dds.DynamicData.DataReader(dds.Subscriber(participant), topic, reader_qos)
    reader.bind_listener(CftListener(), dds.StatusMask.data_available())

    # Change the filter
    if is_cft:
        print('Now setting a new filter: name MATCH \"EVEN\"')
        topic.append_to_expression_parameter(0, 'EVEN')

    count = 0
    while (sample_count == 0) or (count < sample_count):
        time.sleep(1)

        if is_cft:
            if count == 10:
                print(textwrap.dedent(
                    """
                    ==========================
                    Changing Filter Parameters
                    Append 'ODD' filter
                    =========================="""))
                topic.append_to_expression_parameter(0, 'ODD')
            if count == 20:
                print(textwrap.dedent(
                    """
                    ==========================
                    Changing Filter Parameters
                    Remove 'EVEN' filter
                    =========================="""))
                topic.remove_from_expression_parameter(0, 'EVEN')
        count += 1


parser = argparse.ArgumentParser(description='RTI Connext DDS Example: Using String Filters (Subscriber)')
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
    '-f',
    '--filter',
    action='store_true',
    default=False,
    help='Use a CFT')

args = parser.parse_args()
assert(0 <= args.domain < 233)
assert(args.count >= 0)

subscriber_main(args.domain, args.count, args.filter)
