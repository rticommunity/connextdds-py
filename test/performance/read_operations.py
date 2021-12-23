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

from os import path as os_path
from sys import path as sys_path
import rti.connextdds as dds
import rti.idl as idl
import time
import timeit
import rticonnextdds_connector as con

DOMAIN_ID = 54
ITERATIONS = 100
SAMPLES_PER_ITER = 1000

@idl.struct(member_annotations={'x': [idl.key]})
class Point:
    x: int = 0
    y: int = 0


def create_participant(domain_id):
    qos = dds.DomainParticipantQos()
    qos.database.shutdown_cleanup_period = dds.Duration.from_milliseconds(10)
    return dds.DomainParticipant(domain_id, qos)

class PubSubFixture:
    def __init__(self, participant, create_writer=True, create_reader=True, create_connector=False):
        if type(participant) is int:
            self.participant = create_participant(participant)
        else:
            self.participant = participant
        self.topic = dds.Topic(self.participant, "Test", Point)

        dd_participant = create_participant(participant)
        dd_topic = dds.DynamicData.Topic(dd_participant, "Test", Point.type_support.dynamic_type)

        self.samples_per_iter = SAMPLES_PER_ITER

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

        if create_connector:
            file_path = os_path.dirname(os_path.realpath(__file__)) + '/'

            self.connector = con.Connector(
                config_name = "MyParticipantLibrary::MySubParticipant",
                url = file_path + "PerformanceConnector.xml")

            self.connector_input = self.connector.get_input("MySubscriber::MyPointReader")

            if self.connector_input.wait_for_publications(timeout=2000) == 0:
                raise Exception("No publications")

        time.sleep(2)

    def write_data(self):
        sample_count = self.samples_per_iter
        for i in range(sample_count):
            self.writer.write(Point(i, i))

        if hasattr(self, 'dd_reader'):
            time.sleep(0.5)
            data = self.dd_reader.read()
            while len(data) != sample_count:
                print(f"Waiting for data {len(data)}")
                data = self.dd_reader.read()
                time.sleep(0.5)

    def read_data(self):
        sample_count = self.samples_per_iter
        data = self.reader.read_data()
        if len(data) != sample_count:
            raise Exception(
                f"Received {len(data)} samples instead of {sample_count}")

        for d in data:
            if d.x != d.y:
                raise Exception(f"Received {d.x} instead of {d.y}")

    def read_data_native(self):
        sample_count = self.samples_per_iter
        data = self.reader.read_data_native()
        if len(data) != sample_count:
            raise Exception(
                f"Received {len(data)} samples instead of {sample_count}")

        ts = Point.type_support
        for d in data:
            p = ts._cast_c_sample(d)
            if p.x != p.y:
                raise Exception(f"Received {p.x} instead of {p.y}")

    def read_data_and_info(self):
        sample_count = self.samples_per_iter
        data = self.reader.read()
        if len(data) != sample_count:
            raise Exception(
                f"Received {len(data)} samples instead of {sample_count}")

        for (d, i) in data:
            pass

    def read_data_dynamic(self):
        sample_count = self.samples_per_iter
        with self.dd_reader.read() as samples:
            if len(samples) != sample_count:
                raise Exception(
                    f"Received {len(samples)} samples instead of {sample_count}")
            for data in (s.data for s in samples if s.info.valid):
                if data["x"] != data["y"]:
                    raise Exception(
                        f"Received {data['x']} instead of {data['y']}")

    def read_data_connector(self):
        sample_count = self.samples_per_iter
        self.connector_input.read()
        count = self.connector_input.samples.length
        if count != sample_count:
            raise Exception(
                f"Received {count} samples instead of {sample_count}")
        for sample in self.connector_input.samples.valid_data_iter:
            data = sample.get_dictionary()
            if data["x"] != data["y"]:
                raise Exception(
                    f"Received {data['x']} instead of {data['y']}")




def timeit_per_iter(func, number, title="Test result"):
    MS_PER_SECOND = 1000
    result = MS_PER_SECOND * timeit.timeit(func, number=number) / number
    print(f"{title},{result},ms")
    return result


def perftest_write_data():
    fixture = PubSubFixture(DOMAIN_ID, create_reader=False)

    return timeit_per_iter(fixture.write_data, ITERATIONS)

def perftest_read_data():
    fixture = PubSubFixture(0)
    fixture.write_data()

    return timeit_per_iter(fixture.read_data, number=ITERATIONS, title="python-idl")


def perftest_read_data_native():
    fixture = PubSubFixture(0)
    fixture.write_data()

    return timeit_per_iter(fixture.read_data_native, number=ITERATIONS, title="python-native-idl")


def perftest_read_data_and_info():
    fixture = PubSubFixture(0)
    fixture.write_data()

    return timeit_per_iter(fixture.read_data_and_info, number=ITERATIONS, title="python-w-info-idl")

def pertest_read_dynamic_data():

    fixture = PubSubFixture(0)
    fixture.write_data()

    return timeit_per_iter(fixture.read_data_dynamic, number=20, title="python-dynamic-data")


def perftest_connector():
    fixture = PubSubFixture(DOMAIN_ID, create_connector=True)
    fixture.write_data()

    while True:
        fixture.connector_input.read()
        if fixture.connector_input.samples.length == fixture.samples_per_iter:
            break
        else:
            print(f"Waiting for data {fixture.connector_input.samples.length}")
            time.sleep(0.5)

    return timeit_per_iter(fixture.read_data_connector, number=ITERATIONS, title="connector")

def perftest_baseline():
    fixture = PubSubFixture(0)
    fixture.write_data()

    return timeit_per_iter(lambda: fixture.reader._test_native_reads(1000), number=ITERATIONS, title="c++-idl")


if __name__ == "__main__":

    result_baseline = perftest_baseline()
    result_python = perftest_read_data()

    result_native = perftest_read_data_native()

    # result = perftest_read_data_and_info()
    # print(f"{result} seconds")

    # result_dd = pertest_read_dynamic_data()
    # result_con = perftest_connector()

    # result = perftest_write_data()
    # print(f"Connext Py Write IDL data: {result} ms")

    difference = 100 * (result_python - result_baseline) / result_baseline
    print(f'c++ vs python: {difference} %')

    difference = 100 * (result_native - result_baseline) / result_baseline
    print(f'c++ vs python native: {difference} %')

    # difference = 100 * (result_dd - result_python) / result_python
    # print(f'python vs dynamic: {difference} %')

    # difference = 100 * (result_con - result_python) / result_python
    # print(f'python vs connector: {difference} %')
