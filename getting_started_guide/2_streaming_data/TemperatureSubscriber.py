import rti.connextdds as dds
import pathlib
import random
import argparse
import time

FILE = str(pathlib.Path(__file__).parent.absolute()) + "/" + "temperature.xml"

samples_read = 0
reader = None


def process_data(reader):
    samples_read = 0
    samples = reader.take()

    for sample in samples:
        if sample.info.valid:
            samples_read += 1
            print(f"Received {sample.data}")
    return samples_read


def main(domain_id, sample_count, sensor_id):
    participant = dds.DomainParticipant(domain_id)

    provider_type = dds.QosProvider(FILE).type("Temperature")

    topic = dds.DynamicData.Topic(participant, "ChocolateTemperature", provider_type)

    subscriber = dds.Subscriber(participant)

    global reader
    reader = dds.DynamicData.DataReader(subscriber, topic)

    status_condition = dds.StatusCondition(reader)

    status_condition.enabled_statuses = dds.StatusMask.data_available()

    global samples_read

    def helper(_):
        global samples_read
        global reader
        samples_read += process_data(reader)

    status_condition.handler(helper)
    waitset = dds.WaitSet()
    waitset += status_condition

    try:
        while sample_count is None or samples_read < sample_count:
            print("ChocolateTemperature subcriber sleeping for 4 sec...")
            waitset.dispatch(dds.Duration(4))
    except KeyboardInterrupt:
        print("Keyboard interrupt caught")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--domain-id",
        type=int,
        action="store",
        required=False,
        default=0,
        dest="domain_id",
    )
    parser.add_argument(
        "--sample-count",
        type=int,
        action="store",
        required=False,
        default=None,
        dest="sample_count",
    )
    parser.add_argument(
        "--sensor-id",
        type=str,
        action="store",
        required=False,
        default="0",
        dest="sensor_id",
    )
    args = parser.parse_args()

    main(args.domain_id, args.sample_count, args.sensor_id)
