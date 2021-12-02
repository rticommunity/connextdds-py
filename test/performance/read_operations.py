#
# (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

import rti.connextdds as dds
import rti.idl as idl
import time
import timeit


def create_participant(domain_id):
    qos = dds.DomainParticipantQos()
    qos.database.shutdown_cleanup_period = dds.Duration.from_milliseconds(10)
    return dds.DomainParticipant(domain_id, qos)


@idl.struct(member_annotations={'x': [idl.key]})
class Point:
    x: int = 0
    y: int = 0


class PubSubFixture:
    def __init__(self, participant, create_writer=True, create_reader=True):
        if type(participant) is int:
            self.participant = create_participant(participant)
        else:
            self.participant = participant
        self.topic = dds.Topic(self.participant, "Test", Point)

        dd_participant = create_participant(participant)
        dd_topic = dds.DynamicData.Topic(dd_participant, "Test", Point.type_support.dynamic_type)

        if create_writer:
            writer_qos = self.participant.implicit_publisher.default_datawriter_qos
            writer_qos << dds.Reliability.reliable()
            writer_qos << dds.History.keep_all
            writer_qos << dds.Durability.transient_local
            self.writer = dds.DataWriter(
                self.participant.implicit_publisher, self.topic, writer_qos)

        if create_reader:
            reader_qos = self.participant.implicit_subscriber.default_datareader_qos
            reader_qos << dds.Reliability.reliable()
            reader_qos << dds.History.keep_all
            reader_qos << dds.Durability.transient_local
            self.reader = dds.DataReader(
                self.participant.implicit_subscriber, self.topic, reader_qos)

            self.dd_reader = dds.DynamicData.DataReader(
                dd_participant.implicit_subscriber, dd_topic, reader_qos)

        time.sleep(2)

    def write_data(self, sample_count: int = 1000):
        for i in range(sample_count):
            self.writer.write(Point(i, i))

    def read_data(self, sample_count: int = 1000):
        data = self.reader.read_data()
        if len(data) != sample_count:
            raise Exception(
                f"Received {len(data)} samples instead of {sample_count}")

        for d in data:
            pass

    def read_data_and_info(self, sample_count: int = 1000):
        data = self.reader.read()
        if len(data) != sample_count:
            raise Exception(
                f"Received {len(data)} samples instead of {sample_count}")

        for (d, i) in data:
            pass


def perftest_read_data():
    fixture = PubSubFixture(0)
    fixture.write_data()

    return timeit.timeit(fixture.read_data, number=2000)


def perftest_read_data_and_info():
    fixture = PubSubFixture(0)
    fixture.write_data()

    return timeit.timeit(fixture.read_data_and_info, number=2000)

def pertest_read_dynamic_data():

    fixture = PubSubFixture(0)
    fixture.write_data()

    def read_data():
        data = fixture.dd_reader.read()
        if len(data) != 1000:
            raise Exception(
                f"Received {len(data)} samples instead of {1000}")
        for (d, i) in data:
            pass

    return timeit.timeit(read_data, number=2)

def perftest_baseline():
    fixture = PubSubFixture(0)
    fixture.write_data()

    return timeit.timeit(lambda: fixture.reader.test_alex(1000), number=2000)


if __name__ == "__main__":
    result = perftest_read_data()
    print(f"{result} seconds")

    result = perftest_read_data_and_info()
    print(f"{result} seconds")

    # result = perftest_baseline()
    # print(f"{result} seconds")

    # result = pertest_read_dynamic_data()
    # print(f"{result} seconds")
