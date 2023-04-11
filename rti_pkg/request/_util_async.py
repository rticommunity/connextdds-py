#  (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#  RTI grants Licensee a license to use, modify, compile, and create derivative
#  works of the Software.  Licensee has the right to distribute object form only
#  for use with RTI products.  The Software is provided "as is", with no warranty
#  of any type, including any warranty for fitness for any purpose. RTI is under
#  no obligation to maintain or support the Software.  RTI shall not be liable for
#  any incidental or consequential damages arising out of the use or inability to
#  use the software.


import rti.connextdds
from . import _util
import asyncio
try:
    from typing import Union, Optional, Callable
except ImportError:
    pass


async def wait_for_samples_async(
    reader,             # type: Union[rti.connextdds.DynamicData.DataReader, object]
    min_count,          # type: int
    max_wait,           # type: rti.connextdds.Duration
    waitset,            # type: rti.connextdds.WaitSet
    initial_condition,  # type: rti.connextdds.ICondition
    condition           # type: rti.connextdds.ICondition
):
    # type: (...) -> bool
    if min_count == rti.connextdds.LENGTH_UNLIMITED:
        min_count = _util.INT_MAX
        
    sample_count = reader.select().max_samples(
        min_count).condition(initial_condition).read_loaned().length
    min_count -= sample_count

    participant = reader.subscriber.participant
    remaining_wait = max_wait
    
    while min_count > 0:
        if min_count == 1:
            conditions = await waitset.wait_async(remaining_wait)
        else:
            before_time = participant.current_time
            conditions = await waitset.wait_async(remaining_wait)
            wait_time = participant.current_time - before_time
            remaining_wait -= wait_time

        # WaitSet should only have one condition
        if len(conditions) != 1:
            return False

        if min_count > 1:
            sample_count = reader.select().max_samples(
                min_count).condition(condition).read_loaned().length
            min_count -= sample_count
        else:
            min_count -= 1

    return True


async def send_with_request_id_async(
    writer,     # type: Union[rti.connextdds.DynamicData.DataWriter, object]
    reply,      # type: Union[rti.connextdds.DynamicData, object]
    request_id, # type: rti.connextdds.SampleIdentity
    final       # type: bool
):
    # type: (...) -> None
    _util.validate_related_request_id(request_id)
    params = rti.connextdds.WriteParams()
    params.related_sample_identity = request_id
    if not final:
        params.flag = rti.connextdds.SampleFlag.INTERMEDIATE_REPLY_SEQUENCE
    await writer.write_async(reply, params)

