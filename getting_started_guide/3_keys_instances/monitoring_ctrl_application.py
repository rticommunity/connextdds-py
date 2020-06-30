import rti.connextdds as dds
import argparse
import pathlib
import threading
import time

FILE = str(pathlib.Path(__file__).parent.absolute()) + "/" + "temperature.xml"

lots_processed = 0
reader = None
provider_type = dds.QosProvider(FILE).type("ChocolateLotState")


def publish_start_lot(writer, lots_to_process):
    sample = dds.DynamicData(provider_type)
    try:
        for count in range(0, lots_to_process):
            sample["lot_id"] = count % 100
            sample["lot_status"] = None  # enum status
            sample["next_station"] = None  # enum station
            print(
                f"Starting lot:\n[lot_id: {sample['lot_id']} next_station: {sample['next_station']}]"
            )
            writer.write(sample)
            time.sleep(8)
    except KeyboardInterrupt:
        print("Process broken by user")


def monitor_lot_state(reader):
    samples_read = 0
    samples = reader.take()
    for sample in samples:
        if sample.info.valid:
            print("Recieved lot update: " + str(sample.data))
            samples_read += 1

    return samples_read


def main(domain_id, lots_to_process, sensor_id):
    participant = dds.DomainParticipant(domain_id)

    topic = dds.DynamicData.Topic(
        participant, "CHOCOLATE_LOT_STATE_TOPIC", provider_type
    )

    publisher = dds.Publisher(participant)

    writer = dds.DynamicData.DataWriter(publisher, topic)

    subscriber = dds.Subscriber(participant)

    global reader
    reader = dds.DynamicData.DataReader(subscriber, topic)

    status_condition = dds.StatusCondition(reader)
    status_condition.enabled_statuses = dds.StatusMask.data_available()

    global lots_processed

    def handler(_):
        global lots_processed
        global reader
        lots_processed += monitor_lot_state(reader)

    status_condition.handler(handler)

    waitset = dds.WaitSet()
    waitset += status_condition

    start_lot_thread = threading.Thread(
        target=publish_start_lot, args=(writer, lots_to_process)
    )
    start_lot_thread.start()
    try:
        while lots_processed < lots_to_process:
            waitset.dispatch(dds.Duration(10))
    except KeyboardInterrupt:
        print("Process broken by user")

    start_lot_thread.join()


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
