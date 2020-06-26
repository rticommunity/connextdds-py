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

FILE = (
    str(pathlib.Path(__file__).parent.absolute())
    + "/"
    + "../HelloWorld.xml",
)


def process_data(reader):
    samples_read = 0
    samples = reader.take()
    
    for sample in samples.valid_data:
        print(f"Received: {sample}")
        samples_read += 1

    return samples_read

def main(domain_id, sample_count):
    with dds.DomainParticipant(domain_id) as participant:
        provider = dds.QosProvider(FILE)
        
        provider_type = provider.type("HelloWorld")
        topic = dds.DynamicData.Topic("Example HelloWorld", provider_type)
        
        subscriber = dds.Subscriber(participant)
        reader = dds.DynamicData.DataReader(publisher, topic)
        
        reader.status_condition.enabled_status = #? Status mask?
        
        samples_read = 0
        reader.status_condition.triggered += (samples_read += process_data(reader))
        while samples_read < sample_count:
            print("Hello World subscriber sleeping for 4 seconds...")
            time.sleep(4)

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

