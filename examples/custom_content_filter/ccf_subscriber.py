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
import ccf
import time
import argparse
import textwrap


class CcfListener(dds.DynamicData.NoOpDataReaderListener):
    def on_data_available(self, reader):
        with reader.take() as samples:
            for sample in filter(lambda s: s.info.valid, samples):
                print(sample.data)


# Use a custom filter on a reader to perform complex filtering
def subscriber_main(domain_id, sample_count):
    participant = dds.DomainParticipant(domain_id)

    ccf_type = dds.QosProvider("ccf.xml").type("ccf_lib", "Foo")
    topic = dds.DynamicData.Topic(participant, "Example ccf", ccf_type)

    # Register the custom filter with the Participant
    participant.register_contentfilter(ccf.CustomFilterType(), "CustomFilter")

    # Set a filter expression and the filter name, then create the CFT
    custom_filter = dds.Filter("%0 %1 x", ["2", "divides"])
    custom_filter.name = "CustomFilter"
    topic = dds.DynamicData.ContentFilteredTopic(
        topic, "ContentFilteredTopic", custom_filter
    )

    print("Filter: 2 divides x")

    reader = dds.DynamicData.DataReader(dds.Subscriber(participant), topic)
    reader.bind_listener(CcfListener(), dds.StatusMask.DATA_AVAILABLE)

    count = 0
    while (sample_count == 0) or (count < sample_count):
        time.sleep(1)

        if count == 10:
            # Update the filter parameters after 10 seconds
            print(
                textwrap.dedent(
                    """
                ==========================
                Changing Filter Parameters
                Filter: 15 greater-than x
                =========================="""
                )
            )
            topic.filter_parameters = ["15", "greater-than"]
        elif count == 20:
            # Update the filter again after 20 seconds
            print(
                textwrap.dedent(
                    """
                ==========================
                Changing Filter Parameters
                Filter: 3 divides x
                =========================="""
                )
            )
            topic.filter_parameters = ["3", "divides"]

        count += 1


parser = argparse.ArgumentParser(
    description="RTI Connext DDS Example: Using Custom Filters (Publisher)"
)
parser.add_argument("-d", "--domain", type=int, default=0, help="DDS Domain ID")
parser.add_argument(
    "-c", "--count", type=int, default=0, help="Number of samples to send"
)

args = parser.parse_args()
assert 0 <= args.domain < 233
assert args.count >= 0

subscriber_main(args.domain, args.count)
