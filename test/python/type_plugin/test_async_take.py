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
import pytest

if sys.version_info < (3, 7):
    pytest.skip("rti.asyncio requires Python 3.7 or later", allow_module_level=True)

import asyncio

import rti.connextdds as dds
import rti.types as idl
import rti.asyncio

from test_utils.fixtures import *


@idl.struct(member_annotations={'y': [idl.key]})
class Point:
    x: int = 0
    y: int = 0


async def publisher_run(fixture: PubSubFixture, num_samples: int = 100000):
    # Write for a long time; this task will be cancelled by the tests as needed
    try:
        for i in range(num_samples):
            fixture.writer.write(Point(x=i, y=i%2))
            await asyncio.sleep(0.05)
    except asyncio.CancelledError:
        pass

# Reads a number of samples and checks that samples are consecutive
async def take_data_async_base(
    reader: dds.DataReader,
    condition = None,
    num_samples: int = 10
):
    count = 0
    saved_x = -1
    # when a condition is passed, the tests select only half of the samples
    expected_increment = 1 if condition is None else 2
    async for data in reader.take_data_async(condition=condition):
        if saved_x != -1:
            assert data.x == saved_x + expected_increment
        saved_x = data.x
        count += expected_increment
        if count == num_samples:
            break

# Basic test: one reader
async def take_data_async_basic(shared_participant):
    fixture = PubSubFixture(shared_participant, Point)
    pub_task = asyncio.create_task(publisher_run(fixture))
    await take_data_async_base(fixture.reader)
    pub_task.cancel()

# For each async test we define a function that can run it synchronously. If
# many more async tests are added, we can use one of pytest's
# async fixtures (extra packages are required)
def test_take_data_async_basic(shared_participant):
    rti.asyncio.run(take_data_async_basic(shared_participant))


async def take_data_async_w_selector(shared_participant):
    fixture = PubSubFixture(shared_participant, Point)
    pub_task = asyncio.create_task(publisher_run(fixture))
    await take_data_async_base(
        fixture.reader,
        condition=dds.QueryCondition(dds.Query(fixture.reader, "y = 1"), dds.DataState.any))
    pub_task.cancel()


def test_take_data_async_w_selector(shared_participant):
    rti.asyncio.run(take_data_async_w_selector(shared_participant))


# Test several readers calling take_async() concurrently
async def many_take_data_async(shared_participant):
    fixture = PubSubFixture(shared_participant, Point)
    r1 = dds.DataReader(fixture.participant, fixture.topic)
    r2 = dds.DataReader(fixture.participant, fixture.topic)
    r3 = dds.DataReader(fixture.participant, fixture.topic)
    pub_task = asyncio.create_task(publisher_run(fixture))

    for _ in range(10):
        await asyncio.gather(
            take_data_async_base(fixture.reader),
            take_data_async_base(r1),
            take_data_async_base(r2),
            take_data_async_base(r3))

    pub_task.cancel()

def test_take_data_async_many(shared_participant):
    rti.asyncio.run(many_take_data_async(shared_participant))


async def take_infinite(
    reader: dds.DataReader,
    condition = None
):
    saved_x = -1
    expected_increment = 1 if condition is None else 2
    try:
        async for data, _ in reader.take_async(condition=condition):
            if saved_x != -1:
                assert data.x == saved_x + expected_increment
            saved_x = data.x
    except asyncio.CancelledError:
        return saved_x
    except Exception as e:
        print(e)
        return -1

    return saved_x


# Tests that a take_async() call can be cancelled
async def cancel_async(shared_participant):
    fixture = PubSubFixture(shared_participant, Point)
    pub_task = asyncio.create_task(publisher_run(fixture))
    task = asyncio.create_task(take_infinite(fixture.reader))

    # Receive a few samples
    await asyncio.sleep(1)
    pub_task.cancel()

    # Cancel the task, and await for it to return the value of the last sample,
    # which should be a positive number (-1 is an error and 0 means nothing was
    # received)
    task.cancel()
    assert await task > 0


def test_cancel_async(shared_participant):
    rti.asyncio.run(cancel_async(shared_participant))


async def take_cancel_async_w_selector(shared_participant):
    fixture = PubSubFixture(shared_participant, Point)
    reader = fixture.reader
    pub_task = asyncio.create_task(publisher_run(fixture))
    task = asyncio.create_task(
        take_infinite(
            reader,
            condition=dds.QueryCondition(dds.Query(reader, "y = 1"), dds.DataState.any)))

    # Receive a few samples
    await asyncio.sleep(1)
    pub_task.cancel()

    # Cancel the task, and await for it to return the value of the last sample,
    # which should be a positive number (-1 is an error and 0 means nothing was
    # received)
    task.cancel()
    assert await task > 0


def test_take_cancel_async_w_selector(shared_participant):
    rti.asyncio.run(take_cancel_async_w_selector(shared_participant))


async def take_n(reader: dds.DataReader, n: int, condition = None):
    samples = []
    count = 0
    try:
        async for data in reader.take_data_async(condition=condition):
            samples.append(data)
            count += 1
            if count == n:
                return samples
    except asyncio.CancelledError:
        pass

    return samples


async def cancel_async_without_data(shared_participant):
    fixture = PubSubFixture(shared_participant, Point)
    read_task = asyncio.create_task(take_n(fixture.reader, 10))
    await asyncio.sleep(0.1) # let the task begin
    read_task.cancel() # cancel without take_async yielding any value
    samples = await read_task
    assert len(samples) == 0


def test_cancel_async_without_data(shared_participant):
    rti.asyncio.run(cancel_async_without_data(shared_participant))


async def begin_async_with_data_available(shared_participant):
    fixture = PubSubFixture(shared_participant, Point)
    # Publish and receive the data before we attempt to take it
    await publisher_run(fixture, 10)
    await asyncio.to_thread(wait.for_data, fixture.reader)
    samples = await take_n(fixture.reader, 1)
    assert len(samples) == 1

def test_begin_async_with_data_available(shared_participant):
    rti.asyncio.run(begin_async_with_data_available(shared_participant))


async def change_query_condition_while_reading(shared_participant):
    fixture = PubSubFixture(shared_participant, Point)
    reader = fixture.reader
    condition = dds.QueryCondition(dds.Query(reader, "y = %0", ["0"]), dds.DataState.any)
    pub_task = asyncio.create_task(publisher_run(fixture))
    read_task = asyncio.create_task(take_n(reader, 20, condition))
    await asyncio.sleep(.5)
    condition.parameters = ["1"]
    samples = await read_task
    pub_task.cancel()
    await pub_task

    assert any(sample.y == 1 for sample in samples)
    assert any(sample.y == 0 for sample in samples)

def test_change_query_condition_while_reading(shared_participant):
    rti.asyncio.run(change_query_condition_while_reading(shared_participant))

