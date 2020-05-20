import rti.connextdds as dds
import time
import argparse
import textwrap


class CftListener(dds.DynamicData.NoOpDataReaderListener):
    def on_data_available(self, reader):
        with reader.take() as samples:
            for sample in filter(lambda s: s.info.valid, samples):
                print(sample.data)


def subscriber_main(domain_id, sample_count, is_cft):
    participant = dds.DomainParticipant(domain_id)

    cft_type = dds.QosProvider('cft.xml').type('cft_lib', 'cft')
    topic = dds.DynamicData.Topic(participant, 'Example cft', cft_type)

    if is_cft:
        topic = dds.DynamicData.ContentFilteredTopic(
            topic,
            'ContentFilteredTopic',
            dds.Filter('x >= %0 AND x <= %1', ['1', '4']))
        print(textwrap.dedent(
            """
            ==========================
            Using CFT
            Filter: 1 <= x <= 4
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

    count = 0
    while (sample_count == 0) or (count < sample_count):
        time.sleep(1)

        if is_cft:
            if count == 10:
                print(textwrap.dedent(
                    """
                    ==========================
                    Changing Filter Parameters
                    Filter: 5 <= x <= 9
                    =========================="""))
                topic.filter_parameters = ['5', '9']
            if count == 20:
                print(textwrap.dedent(
                    """
                    ==========================
                    Changing Filter Parameters
                    Filter: 3 <= x <= 9
                    =========================="""))
                topic.filter_parameters = ['3', '9']
        count += 1


parser = argparse.ArgumentParser(description='RTI Connext DDS Example: Using CFTs (Subscriber)')
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
