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

import rti.connextdds as dds
import utils
import pytest
import pathlib
import time

FILE = str(pathlib.Path(__file__).parent.absolute()) + "/../../xml/PerformanceTester.xml"
PROVIDER = dds.QosProvider(FILE)
# A type with an x and a y value
COORD_TYPE = PROVIDER.type("NonPrimitiveType")


def test_sample_creation():
    s1 = dds.DynamicData(COORD_TYPE)
    s1["x"] = 1
    s1["y"] = 2

    my_dict = {"x": 1, "y": 2}
    s2 = dds.DynamicData(COORD_TYPE, my_dict)

    assert s1 == s2


def test_sending_dict():
    s1 = dds.DynamicData(COORD_TYPE)
    s1["x"] = 1
    s1["y"] = 2

    my_dict = {"x": 1, "y": 2}
    with dds.DomainParticipant(0) as participant:
        topic = dds.DynamicData.Topic(participant, "dictionary_test", COORD_TYPE)
        # Create qos for both, add reliable and keep all
        writer_qos = participant.implicit_publisher.default_datawriter_qos
        writer_qos << dds.Reliability.reliable()
        writer_qos << dds.History.keep_all
        writer_qos << dds.Durability.transient_local

        # Create the reader qos
        reader_qos = participant.implicit_subscriber.default_datareader_qos
        reader_qos << dds.Reliability.reliable()
        reader_qos << dds.History.keep_all
        reader_qos << dds.Durability.transient_local
        writer = dds.DynamicData.DataWriter(
            participant.implicit_publisher, topic, writer_qos
        )
        reader = dds.DynamicData.DataReader(
            participant.implicit_subscriber, topic, reader_qos
        )

        writer.write(my_dict)
        while len(reader.read()) < 1:
            time.sleep(0.001)
        for s2 in reader.take():
            print(s2.data)
            print(type(s2.data))
            # Makes for easy debugging
            assert type(s2.data) == type(s1)
            assert s2.data == s1
