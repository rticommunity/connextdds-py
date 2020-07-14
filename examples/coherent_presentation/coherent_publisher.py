"""
 (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 RTI grants Licensee a license to use, modify, compile, and create derivative
 works of the Software.  Licensee has the right to distribute object form only
 for use with RTI products.  The Software is provided "as is", with no warranty
 of any type, including any warranty for fitness for any purpose. RTI is under
 no obligation to maintain or support the Software.  RTI shall not be liable for
 any incidental or consequential damages arising out of the use or inability to
 use the software.
 """

import rti.connextdds as dds
import time
import random
import argparse

try:
    xrange
except NameError:
    xrange = range


def publisher_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)

    publisher_qos = dds.QosProvider.default().publisher_qos
    publisher = dds.Publisher(participant, publisher_qos)

    coherent_type = dds.QosProvider("coherent.xml").type("coherent_lib", "coherent")
    topic = dds.DynamicData.Topic(participant, "Example coherent", coherent_type)
    datawriter_qos = dds.QosProvider.default().datawriter_qos
    writer = dds.DynamicData.DataWriter(publisher, topic, datawriter_qos)

    sample = dds.DynamicData(coherent_type)
    sample["id"] = 0
    handle = writer.register_instance(sample)

    num_samples = 7
    count = 0
    while (sample_count == 0) or (count < sample_count):
        # Use a context manager to scope the coherent set writes
        with dds.CoherentSet(publisher):
            print("Begin Coherent Changes")

            for i in xrange(num_samples):
                time.sleep(1)
                sample["field"] = chr(ord("a") + i)
                sample["value"] = random.randint(0, 10)
                print(
                    "Updating instance, {}->{}".format(sample["field"], sample["value"])
                )
                writer.write(sample, handle)
                count += 1

            print("End Coherent Changes")

    writer.unregister_instance(handle)


parser = argparse.ArgumentParser(
    description="RTI Connext DDS Example: Using Coherent Presentation (Publisher)"
)
parser.add_argument("-d", "--domain", type=int, default=0, help="DDS Domain ID")
parser.add_argument(
    "-c", "--count", type=int, default=0, help="Number of samples to send"
)

args = parser.parse_args()
assert 0 <= args.domain < 233
assert args.count >= 0

publisher_main(args.domain, args.count)
