import rti.connextdds as dds
import time
import argparse

try:
    xrange
except NameError:
    xrange = range


# The listener for a coherent set will only output when a set is received
class CoherentListener(dds.DynamicData.NoOpDataReaderListener):
    def on_data_available(self, reader):
        with reader.take() as samples:
            print("Received updates:")
            for sample in filter(lambda s: s.info.valid, samples):
                data = sample.data
                print(" {} = {};".format(data["field"], data["value"]))


def subscriber_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)

    subscriber_qos = dds.QosProvider.default.subscriber_qos
    subscriber = dds.Subscriber(participant, subscriber_qos)

    coherent_type = dds.QosProvider("coherent.xml").type("coherent_lib", "coherent")
    topic = dds.DynamicData.Topic(participant, "Example coherent", coherent_type)
    datareader_qos = dds.QosProvider.default.datareader_qos
    reader = dds.DynamicData.DataReader(subscriber, topic, datareader_qos)
    reader.set_listener(CoherentListener(), dds.StatusMask.DATA_AVAILABLE)

    count = 0
    while (sample_count == 0) or (count < sample_count):
        time.sleep(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="RTI Connext DDS Example: Using Coherent Presentation (Subscriber)"
    )
    parser.add_argument("-d", "--domain", type=int, default=0, help="DDS Domain ID")
    parser.add_argument(
        "-c", "--count", type=int, default=0, help="Number of samples to send"
    )

    args = parser.parse_args()
    assert 0 <= args.domain < 233
    assert args.count >= 0

    subscriber_main(args.domain, args.count)
