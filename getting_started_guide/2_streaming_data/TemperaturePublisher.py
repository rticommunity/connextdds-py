import rti.connextdds as dds
import pathlib
import random
import argparse
import time

FILE = str(pathlib.Path(__file__).parent.absolute()) + "/" + "temperature.xml"


def main(domain_id, sample_count, sensor_id):
    participant = dds.DomainParticipant(domain_id)

    provider_type = dds.QosProvider(FILE).type("Temperature")

    topic = dds.DynamicData.Topic(participant, "ChocolateTemperature", provider_type)

    publisher = dds.Publisher(participant)

    writer = dds.DynamicData.DataWriter(publisher, topic)

    sample = dds.DynamicData(provider_type)
    i = 0
    while sample_count is None or i < sample_count:
        sample["sensor_id"] = sensor_id
        sample["degrees"] = random.randint(30, 32)
        print("Writing ChocolateTemperature, count " + str(i))
        writer.write(sample)
        time.sleep(1)
        i += 1


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
