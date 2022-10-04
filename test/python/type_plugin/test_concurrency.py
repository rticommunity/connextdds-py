#
# (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#
# RTI grants Licensee a license to use, modify, compile, and create derivative
# works of the Software solely for use with RTI products.  The Software is
# provided "as is", with no warranty of any type, including any warranty for
# fitness for any purpose. RTI is under no obligation to maintain or support
# the Software.  RTI shall not be liable for any incidental or consequential
# damages arising out of the use or inability to use the software.
#

import sys
if sys.version_info >= (3, 7):
    import asyncio
    import rti.asyncio

from threading import Thread

import rti.connextdds as dds
from test_utils.fixtures import *

# We use a very complex type with sequences and optionals to increase the
# chances to detect any unwanted race condition in the type support
from test_sequences_of_sequences import SequenceTest, create_sequence_sample


# A dds.DataWriter uses a scratchpad ctypes sample in each write. This object
# has to be in a exclusive area so that parallel writes don't use it at the
# same time.
def test_concurrent_write(shared_participant):
    fixture = PubSubFixture(shared_participant, SequenceTest)
    def write_thread(index: int):
        samples = [create_sequence_sample((i + index) % 4) for i in range(100)]
        for sample in samples:
            fixture.writer.write(sample)

    threads = [Thread(target=write_thread, args=(i,)) for i in range(15)]
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()

if sys.version_info >= (3, 7):

    # This test creates a scenario in which the writer will timeout several times
    # until the reader receives all the samples it expects.
    async def blocking_write(shared_participant: dds.DomainParticipant):
        num_samples = 5
        writer_history_depth = num_samples + 2

        writer_qos = shared_participant.default_datawriter_qos
        writer_qos.reliability.kind = dds.ReliabilityKind.RELIABLE
        writer_qos.durability.kind = dds.DurabilityKind.TRANSIENT_LOCAL
        writer_qos.history.kind = dds.HistoryKind.KEEP_ALL
        writer_qos.reliability.max_blocking_time = dds.Duration.from_milliseconds(200)
        writer_qos.resource_limits.max_samples = writer_history_depth
        writer_qos.resource_limits.max_samples_per_instance = writer_history_depth
        writer_qos.resource_limits.initial_samples = writer_history_depth
        writer_qos.data_writer_protocol.rtps_reliable_writer.heartbeats_per_max_samples = 1
        writer_qos.data_writer_protocol.rtps_reliable_writer.min_send_window_size = 1
        writer_qos.data_writer_protocol.rtps_reliable_writer.max_send_window_size = num_samples
        writer_qos.data_writer_protocol.rtps_reliable_writer.heartbeat_period = dds.Duration(seconds=0, nanoseconds=10000000)
        writer_qos.data_writer_protocol.rtps_reliable_writer.fast_heartbeat_period = dds.Duration(seconds=0, nanoseconds=10000000)
        writer_qos.data_writer_protocol.rtps_reliable_writer.late_joiner_heartbeat_period = dds.Duration(seconds=0, nanoseconds=10000000)
        writer_qos.data_writer_protocol.rtps_reliable_writer.max_nack_response_delay = dds.Duration.zero

        reader_qos = shared_participant.default_datareader_qos
        reader_qos.resource_limits.max_samples = 1
        reader_qos.resource_limits.initial_samples = 1
        reader_qos.history.kind = dds.HistoryKind.KEEP_ALL
        reader_qos.reliability.kind = dds.ReliabilityKind.RELIABLE
        reader_qos.durability.kind = dds.DurabilityKind.TRANSIENT_LOCAL
        reader_qos.data_reader_protocol.rtps_reliable_reader.max_heartbeat_response_delay = dds.Duration.zero

        topic = dds.Topic(shared_participant, "SequenceTest", SequenceTest)
        writer = dds.DataWriter(shared_participant.implicit_publisher, topic, writer_qos)
        reader = dds.DataReader(shared_participant.implicit_subscriber, topic, reader_qos)
        wait.for_discovery(reader, writer)

        samples = [create_sequence_sample(), SequenceTest()] * 4
        expected_sample_count = len(samples) * 2

        def write():
            # Write the samples, retrying after every timeout
            samples_to_write = samples[:]
            while len(samples_to_write) > 0:
                try:
                    writer.write(samples_to_write[0])
                    samples_to_write.pop(0)
                except dds.TimeoutError:
                    pass

        async def read():
            received_samples = []
            async for data in reader.take_data_async():
                received_samples.append(data)
                if len(received_samples) == expected_sample_count:
                    break
                # Simulate a slow reader to trigger write timeouts
                await asyncio.sleep(.3)

            return received_samples

        write_thread1 = Thread(target=write)
        write_thread2 = Thread(target=write)
        write_thread1.start()
        write_thread2.start()

        # Run the reader and two writers
        received_samples = await read()

        write_thread1.join()
        write_thread2.join()

        assert len(received_samples) == expected_sample_count
        assert received_samples.count(samples[0]) == expected_sample_count / 2
        assert received_samples.count(samples[1]) == expected_sample_count / 2


    def test_blocking_write(shared_participant):
        rti.asyncio.run(blocking_write(shared_participant))