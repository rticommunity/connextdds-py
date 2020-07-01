#
# (c) Copyright, Real-Time Innovations, 2020.  All rights reserved.
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the software solely for use with RTI Connext DDS. Licensee may
# redistribute copies of the software provided that all such copies are subject
# to this license. The software is provided "as is", with no warranty of any
# type, including any warranty for fitness for any purpose. RTI is under no
# obligation to maintain or support the software. RTI shall not be liable for
# any incidental or consequential damages arising out of the use or inability
# to use the software.
#

import rti.connextdds as dds
import argparse
import time
import pathlib

FILE = str(pathlib.Path(__file__).parent.absolute()) + "/" + "HelloWorld.xml"


def process_data(reader):
    samples_read = 0
    samples = reader.take()

    for sample in samples:
        if sample.info.valid:
            print(f"Received: {sample.data}")
            samples_read += 1

    return samples_read


def main(domain_id, sample_count):
    with dds.DomainParticipant(domain_id) as participant:
        provider = dds.QosProvider(FILE)

        provider_type = provider.type("HelloWorld")
        topic = dds.DynamicData.Topic(participant, "Example HelloWorld", provider_type)

        subscriber = dds.Subscriber(participant)
        reader = dds.DynamicData.DataReader(subscriber, topic)

        status_condition = dds.StatusCondition(reader)

        status_condition.enabled_statuses = dds.StatusMask.data_available()

        samples_read = 0

        def helper(x):
            nonlocal samples_read
            nonlocal reader
            samples_read += process_data(reader)

        status_condition.handler(helper)

        waitset = dds.WaitSet()
        waitset += status_condition

        try:
            while samples_read < sample_count:
                print(f"Hello World subscriber sleeping for 4 seconds...")
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
        default=10,
        dest="sample_count",
    )
    args = parser.parse_args()

    main(args.domain_id, args.sample_count)
