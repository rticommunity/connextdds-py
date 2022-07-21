 #
 # (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 #
 # RTI grants Licensee a license to use, modify, compile, and create derivative
 # works of the Software solely for use with RTI products.  The Software is
 # provided "as is", with no warranty of any type, including any warranty for
 # fitness for any purpose. RTI is under no obligation to maintain or support
 # the Software.  RTI shall not be liable for any incidental or consequential
 # damages arising out of the use or inability to use the software.
 #

import time
import pathlib

import rti.connextdds as dds
from rti.types.builtin import String

import test_utils.fixtures as fixtures

class TestSystem:
    def __init__(self, sample_type):
        self.participant = fixtures.create_participant()

        reader_qos = self.participant.implicit_subscriber.default_datareader_qos
        reader_qos << dds.Durability.transient_local
        reader_qos << dds.Reliability.reliable()
        reader_qos << dds.History.keep_all

        writer_qos = self.participant.implicit_publisher.default_datawriter_qos
        writer_qos << dds.Durability.transient_local
        writer_qos << dds.Reliability.reliable()
        writer_qos << dds.History.keep_all
        if sample_type == "StringTopicType":
            self.topic = dds.Topic(self.participant, "StringTopicType", String)
            self.reader = dds.DataReader(
                self.participant, self.topic, reader_qos
            )
            self.writer = dds.DataWriter(
                self.participant, self.topic, writer_qos
            )
        elif sample_type == "KeyedStringTopicType":
            self.topic = dds.KeyedStringTopicType.Topic(
                self.participant, "KeyedStringTopicType"
            )
            self.reader = dds.KeyedStringTopicType.DataReader(
                self.participant, self.topic, reader_qos
            )
            self.writer = dds.KeyedStringTopicType.DataWriter(
                self.participant, self.topic, writer_qos
            )
        elif sample_type == "PerformanceTest":
            provider = dds.QosProvider(
                str(pathlib.Path(__file__).parent.absolute()) + "/../xml/PerfTest.xml"
            )
            provider_type = provider.type("PerformanceTest")
            self.topic = dds.DynamicData.Topic(
                self.participant, "PerformanceTest", provider_type
            )
            self.reader = dds.DynamicData.DataReader(
                dds.Subscriber(self.participant), self.topic, reader_qos
            )
            self.writer = dds.DynamicData.DataWriter(
                dds.Publisher(self.participant), self.topic, writer_qos
            )
        else:
            raise Exception(sample_type + " not supported in test system")