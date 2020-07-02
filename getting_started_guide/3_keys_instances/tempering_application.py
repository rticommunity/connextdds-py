import rti.connextdds as dds
import argparse
import pathlib
import threading
import time
import random

FILE = str(pathlib.Path(__file__).parent.absolute()) + "/chocolate_factory.xml"

provider = dds.QosProvider(FILE)

TEMPERATURE_TYPE = provider.type("Temperature")
CHOCOLATE_LOT_TYPE = provider.type("ChocolateLotState")
STATION_KIND_TYPE = provider.type("StationKind")
LOT_STATUS_KIND_TYPE = provider.type("LotStatusKind")


def publish_temperature(writer, sensor_id):
    temperature = dds.DynamicData(TEMPERATURE_TYPE)
    try:
        while True:
            temperature["sensor_id"] = sensor_id
            temperature["degrees"] = random.randint(30, 32)
            writer.write(temperature)
            time.sleep(0.1)

    except KeyboardInterrupt:
        print("User ended process")


def process_lot(lot_state_reader, lot_state_writer):
    samples = lot_state_reader.take()
    for sample in samples:
        if (
            sample.info.valid
            and sample.data["next_station"]
            == STATION_KIND_TYPE["TEMPERING_CONTROLLER"].ordinal
        ):
            print(f"Processing lot #{sample.data['lot_id']}")
            updated_state = sample.data
            updated_state["lot_status"] = LOT_STATUS_KIND_TYPE["PROCESSING"].ordinal
            updated_state["next_station"] = STATION_KIND_TYPE[
                "INVALID_CONTROLLER"
            ].ordinal
            updated_state["station"] = STATION_KIND_TYPE["TEMPERING_CONTROLLER"].ordinal
            lot_state_writer.write(updated_state)
            time.sleep(5)


def main(domain_id, sensor_id):
    participant = dds.DomainParticipant(domain_id)
    temperature_topic = dds.DynamicData.Topic(
        participant, "CHOCOLATE_TEMPERATURE_TOPIC", TEMPERATURE_TYPE
    )
    lot_state_topic = dds.DynamicData.Topic(
        participant, "CHOCOLATE_LOT_STATE_TOPIC", CHOCOLATE_LOT_TYPE
    )

    publisher = dds.Publisher(participant)
    temperature_writer = dds.DynamicData.DataWriter(publisher, temperature_topic)
    lot_state_writer = dds.DynamicData.DataWriter(publisher, lot_state_topic)

    subscriber = dds.Subscriber(participant)
    lot_state_reader = dds.DynamicData.DataReader(subscriber, lot_state_topic)

    status_condition = dds.StatusCondition(lot_state_reader)
    status_condition.enabled_statuses = dds.StatusMask.data_available()

    def handler(_):
        nonlocal lot_state_reader
        nonlocal lot_state_writer
        process_lot(lot_state_reader, lot_state_writer)

    status_condition.handler(handler)

    waitset = dds.WaitSet()
    waitset += status_condition

    print(f"ChocolateTemperature Sensor with ID: {sensor_id} staring")
    temperature_thread = threading.Thread(
        target=publish_temperature, args=(temperature_writer, sensor_id)
    )
    temperature_thread.start()
    try:
        while True:
            print("Waiting for lot")
            waitset.dispatch(dds.Duration(10))
    except KeyboardInterrupt:
        print("Process ended by user")

    temperature_thread.join()


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
        "--sensor-id",
        type=str,
        action="store",
        required=False,
        default="0",
        dest="sensor_id",
    )
    args = parser.parse_args()

    main(args.domain_id, args.sensor_id)
