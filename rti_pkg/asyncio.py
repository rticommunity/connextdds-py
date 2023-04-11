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

# The code requires a few functions in asyncio that are new in 3.7
if sys.version_info < (3, 7):
    raise ImportError("rti.asyncio requires Python 3.7 or newer")

import asyncio
from typing import Optional
import rti.connextdds as dds

class WaitSetAsyncDispatcher:
    def __init__(self):
        self.waitset = dds._FastWaitSet()
        self.cancel_condition = dds.GuardCondition()
        self.run_task = None
        self.finish_event = asyncio.Event()
        self.cancel_token = None

    class CancelToken:
        def __init__(self, task: asyncio.Task, cancel_condition: dds.GuardCondition, finish_event: asyncio.Event):
            self._task = task
            self._cancel_condition = cancel_condition
            self._finish_event = finish_event

        async def cancel(self):
            # Wake up the waitset in _run_loop so it can exit
            self._cancel_condition.trigger_value = True
            # Await until _run_loop ends
            await self._finish_event.wait()

    async def _run_loop(self):
        try:
            while not self.cancel_condition.trigger_value:
                # Wait runs in an executor thread
                await self.waitset.wait_async()
                # Dispatch runs in the asyncio loop thread. This is required
                # for the asyncio.Events being awaited to wake up (see
                # condition_handler() in wait(), below).
                self.waitset.dispatch()
            self.finish_event.set()
        except asyncio.CancelledError:
            # Let the wait_async() operation end
            self.cancel_condition.trigger_value = True

    def run(self) -> CancelToken:
        self.waitset.attach_condition(self.cancel_condition)

        impl_task = asyncio.create_task(self._run_loop())
        self.run_task = WaitSetAsyncDispatcher.CancelToken(
            impl_task,
            self.cancel_condition,
            self.finish_event)
        self.cancel_token = self.run_task
        return self.run_task

    async def close(self):
        if self.cancel_token is not None:
            await self.cancel_token.cancel()

    class WaitToken:
        def __init__(
            self,
            condition: dds.ReadCondition
        ):
            self.condition = condition

        def __enter__(self):
            return self

        def __exit__(self, exc_type, exc_val, exc_tb):
            self.condition.reset_handler()


    def register(self, condition: dds.ReadCondition) -> WaitToken:
        return WaitSetAsyncDispatcher.WaitToken(condition)

    async def wait(self, token: WaitToken):
        if token.condition.trigger_value:
            return # No need to wait

        token.condition.reset_handler()
        wait_event = asyncio.Event()

        def condition_handler():
            wait_event.set()

        token.condition.set_handler_no_args(condition_handler)
        self.waitset.attach_condition(token.condition)

        await wait_event.wait()

    async def take_data(
        self,
        reader: dds.DataReader,
        condition: dds.Condition
    ):
        with self.register(condition) as wait_token:
            try:
                await self.wait(wait_token)
                while True:
                    for data in reader.take_data():
                        yield data
                    await self.wait(wait_token)
            except asyncio.CancelledError:
                self.waitset.detach_condition(wait_token.condition)
                raise

    async def take(
        self,
        reader: dds.DataReader,
        condition: dds.Condition
    ):
        with self.register(condition) as wait_token:
            try:
                await self.wait(wait_token)
                while True:
                    for sample in reader.take():
                        yield sample
                    await self.wait(wait_token)
            except asyncio.CancelledError:
                self.waitset.detach_condition(wait_token.condition)
                raise


_DEFAULT_DISPATCHER = None

def get_default_dispatcher() -> WaitSetAsyncDispatcher:
    global _DEFAULT_DISPATCHER
    if _DEFAULT_DISPATCHER is None:
        _DEFAULT_DISPATCHER = WaitSetAsyncDispatcher()
        _DEFAULT_DISPATCHER.run()
    return _DEFAULT_DISPATCHER


def take_data_async(reader: dds.DataReader, condition: Optional[dds.ReadCondition] = None):
    dispatcher = get_default_dispatcher()
    if condition is None:
        return dispatcher.take_data(reader, condition=dds.ReadCondition(reader, dds.DataState.any))
    else:
        return dispatcher.take_data(reader.select().condition(condition), condition=condition)


def take_async(reader: dds.DataReader, condition: Optional[dds.ReadCondition] = None):
    dispatcher = get_default_dispatcher()
    if condition is None:
        return dispatcher.take(reader, condition=dds.ReadCondition(reader, dds.DataState.any))
    else:
        return dispatcher.take(reader.select().condition(condition), condition=condition)

# Inject async functions to DataReader
dds.DataReader.take_data_async = take_data_async
dds.DataReader.take_async = take_async
dds.DynamicData.DataReader.take_data_async = take_data_async
dds.DynamicData.DataReader.take_async = take_async

async def close():
    global _DEFAULT_DISPATCHER
    if _DEFAULT_DISPATCHER is not None:
        await _DEFAULT_DISPATCHER.close()
        _DEFAULT_DISPATCHER = None


def run(coroutine):
    """Uses the current event loop to run the given coroutine and waits until it
    finishes. If there is no current running event loop, a new one is created
    """

    try:
        loop = asyncio.get_running_loop()
    except RuntimeError:
        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)

    try:
        loop.run_until_complete(coroutine)
    finally:
        loop.run_until_complete(close())
