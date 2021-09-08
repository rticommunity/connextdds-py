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
from . import _util_native
from . import _util_async
from . import _basic
try:
    from typing import Union, Optional, Callable
except ImportError:
    pass


class Requester(_basic.Requester):
    def __init__(
        self,
        request_type,               # type: Union[rti.connextdds.DynamicType, type]
        reply_type,                 # type: Union[rti.connextdds.DynamicType, type]
        participant,                # type: rti.connextdds.DynamicData
        service_name=None,          # type: Optional[str]
        request_topic=None,         # type: Union[rti.connextdds.DynamicData.Topic, str, object]
        reply_topic=None,           # type: Union[rti.connextdds.DynamicData.Topic, str, object]
        datawriter_qos=None,        # type: Optional[rti.connextdds.DataWriterQos]
        datareader_qos=None,        # type: Optional[rti.connextdds.DataReaderQos]
        publisher=None,             # type: Optional[rti.connextdds.Publisher]
        subscriber=None,            # type: Optional[rti.connextdds.Subscriber]
        on_reply_available=None     # type: Optional[Callable[[object]]]
    ):
        # type: (...) -> None
        super(Requester, self).__init__(
            request_type,
            reply_type,
            participant,
            service_name,
            request_topic,
            reply_topic,
            datawriter_qos,
            datareader_qos,
            publisher,
            subscriber,
            on_reply_available,
        )


    async def send_request_async(self, request, params=None):
        # type: (Union[rti.connextdds.DynamicData, object], rti.connextdds.WriteParams) -> rti.connextdds.SampleIdentity
        if params is None:
            params = rti.connextdds.WriteParams()
            params.replace_automatic_values = True
        await self._writer.write_async(request, params)
        return params.identity


    async def receive_replies_async(self, max_wait, min_count=1):
        # type: (rti.connextdds.Duration, int) -> Union[rti.connextdds.DynamicData.LoanedSamples, object]
        if not await _util_async.wait_for_samples_async(
                self._reader, 
                min_count, 
                max_wait, 
                self._waitset, 
                self._any_sample_condition,
                self._notread_sample_condition):
            raise rti.connextdds.TimeoutError("Timed out waiting for replies")
        else:
            return self.__reader.take()


    async def wait_for_replies_async(
        self, 
        max_wait,                   # type: rti.connextdds.Duration
        min_count=1,                # type: int
        related_request_id=None     # type: Optional[rti.connextdds.SampleIdentity]
    ):
        if related_request_id is None:
            return _util.wait_for_samples(
                    self._reader,
                    min_count,
                    max_wait,
                    self._waitset,
                    self._any_sample_condition,
                    self._notread_sample_condition)
        else:
            initial_condition = _util_native.create_correlation_condition(
                    self._reader,
                    rti.connextdds.SampleState.ANY,
                    related_request_id.sequence_number)
            correlation_condition = _util_native.create_correlation_condition(
                    self._reader,
                    rti.connextdds.SampleState.ANY,
                    related_request_id.sequence_number)
            waitset = rti.connextdds.WaitSet()
            waitset += correlation_condition
            return await _util_async.wait_for_samples_async(
                    self._reader,
                    min_count, max_wait,
                    waitset, initial_condition,
                    correlation_condition)


class Replier(_basic.Replier):
    def __init__(
        self,
        request_type,               # type: Union[rti.connextdds.DynamicType, type]
        reply_type,                 # type: Union[rti.connextdds.DynamicType, type]
        participant,                # type: rti.connextdds.DomainParticipant
        service_name=None,          # type: Optional[str]
        request_topic=None,         # type: Optional[Union[rti.connextdds.DynamicData.Topic, rti.connextdds.DynamicData.ContentFilteredTopic, str, object]]
        reply_topic=None,           # type: Optional[Union[rti.connextdds.DynamicData.Topic, str, object]]
        datawriter_qos=None,        # type: Optional[rti.connextdds.DataWriterQos]
        datareader_qos=None,        # type: Optional[rti.connextdds.DataReaderQos]
        publisher=None,             # type: Optional[rti.connextdds.Publisher]
        subscriber=None,            # type: Optional[rti.connextdds.Subscriber]
        on_request_available=None   # type: Optional[Callable[[Replier]]]
    ):
        # type: (...) -> None
        super(Replier, self).__init__(
            request_type,
            reply_type,
            participant,
            service_name,
            request_topic,
            reply_topic,
            datawriter_qos,
            datareader_qos,
            publisher,
            subscriber,
            on_request_available
        )


    def send_reply_async(
        self, 
        reply,      # type: object
        param,      # type: Union[rti.connextdds.SampleIdentity, rti.connextdds.SampleInfo, rti.connextdds.WriteParams]
        final=True  # type: bool
    ):
        # type: (...) -> None
        if isinstance(param, rti.connextdds.SampleIdentity):
            _util.send_with_request_id(self.__writer, reply, param, final)
        elif isinstance(param, rti.connextdds.SampleInfo):
            _util.send_with_request_id(
                    self._writer,
                    reply,
                    param.original_publication_virtual_sample_identity,
                    final)
        elif isinstance(param, rti.connextdds.WriteParams):
            _util.validate_related_request_id(param.related_sample_identity)
            if final:
                param.flag &= rti.connextdds.SampleFlag.INTERMEDIATE_REPLY_SEQUENCE.flip()
            else:
                param.flag |= rti.connextdds.SampleFlag.INTERMEDIATE_REPLY_SEQUENCE
            self._writer.write(reply, param)
        else:
            raise rti.connextdds.InvalidArgumentError("Invalid param type")


    async def receive_requests_async(self, max_wait, min_count=1):
        # type: (rti.connextdds.Duration, int) -> Union[rti.connextdds.DynamicData.LoanedSamples, object]
        if not await _util_async.wait_for_samples_async(
                self._reader,
                min_count, max_wait,
                self._waitset,
                self._any_sample_condition,
                self._notread_sample_condition):
            raise rti.connextdds.TimeoutError("Timed out waiting for requests")
        else:
            return self._reader.take()


    async def wait_for_requests_async(self, max_wait, min_count=1):
        # type: (rti.connextdds.Duration, int) -> bool
        return await _util_async.wait_for_samples_async(
                self._reader,
                min_count,
                max_wait,
                self._waitset,
                self._any_sample_condition,
                self._notread_sample_condition)
