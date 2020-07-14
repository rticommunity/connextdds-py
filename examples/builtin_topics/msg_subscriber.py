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
import argparse

try:
    xrange
except NameError:
    xrange = range

# A listener for msg samples
class MsgListener(dds.DynamicData.NoOpDataReaderListener):
    def on_data_available(self, reader):
        with reader.take() as samples:
            # Use a filter to process only valid samples
            for sample in filter(lambda s: s.info.valid, samples):
                print(sample.data)


def subscriber_main(domain_id, sample_count, participant_auth):
    participant_qos = dds.QosProvider.default().participant_qos
    resource_limits_qos = participant_qos.resource_limits
    max_participant_user_data = resource_limits_qos.participant_user_data_max_length

    # Set the participant auth string as user data bytes
    if len(participant_auth) > max_participant_user_data:
        raise ValueError("participant user data exceeds resource limits")
    else:
        participant_qos.user_data.value = bytearray(participant_auth.encode())

    participant = dds.DomainParticipant(domain_id, participant_qos)
    participant.enable()

    msg_type = dds.QosProvider("msg.xml").type("builtin_topics_lib", "msg")
    topic = dds.DynamicData.Topic(participant, "Example msg", msg_type)
    reader = dds.DynamicData.DataReader(dds.Subscriber(participant), topic)
    reader.bind_listener(MsgListener(), dds.StatusMask.data_available())

    count = 0
    while (sample_count == 0) or (count < sample_count):
        time.sleep(1)
        count += 1


parser = argparse.ArgumentParser(
    description="RTI Connext DDS Example: Using Builtin Topics (Subscriber)"
)
parser.add_argument("-d", "--domain", type=int, default=0, help="DDS Domain ID")
parser.add_argument(
    "-c", "--count", type=int, default=0, help="Number of samples to send"
)
parser.add_argument(
    "-a", "--auth", type=str, default="password", help="Default authorization string"
)

args = parser.parse_args()
# assert(0 <= args.domain < 233)
# assert(args.count >= 0)

subscriber_main(args.domain, args.count, args.auth)
