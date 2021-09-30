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
    """A requester object for handling request-reply interactions with DDS.

    :param request_type: The type of the request data.
    :type request_type: Union[rti.connextdds.DynamicType, type]
    :param reply_type: The type of the reply data.
    :type reply_type: Union[rti.connextdds.DynamicType, type]
    :param participant: The DomainParticipant that will hold the request writer and reply reader.
    :type participant: rti.connextdds.DomainParticipant
    :param service_name: Name that will be used to derive the topic name, defaults to None (rely only on custom topics).
    :type service_name: Optional[str]
    :param request_topic: Topic object or name that will be used for the request data, must be set if service_name is None, otherwise overrides service_name, defaults to None (use service_name).
    :type request_topic: Optional[Union[rti.connextdds.DynamicData.Topic, rti.connextdds.DynamicData.ContentFilteredTopic, str, object]]
    :param reply_topic: Topic object or name that will be used for the reply data, must be set if service_name is None, otherwise overrides service_name, defaults to None (use service_name).
    :type reply_topic: Optional[Union[rti.connextdds.DynamicData.Topic, str, object]]
    :param datawriter_qos: QoS object to use for request writer, defaults to None (use default RequestReply QoS).
    :type datawriter_qos: Optional[rti.connextdds.DataWriterQos]
    :param datareader_qos: QoS object to use for reply reader, defaults to None (use default RequestReply QoS).
    :type datareader_qos: Optional[rti.connextdds.DataReaderQos]
    :param publisher: Publisher used to hold request writer, defaults to None (use participant builtin publisher).
    :type publisher: Optional[rti.connextdds.Publisher]
    :param subscriber: Subscriber used to hold reply reader, defaults to None (use participant builtin subscriber).
    :type subscriber: Optional[rti.connextdds.Subscriber]
    :param on_reply_available: The callback that handles incoming replies.
    :type on_reply_available: Callable[[object], object]
    """
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
        """Send a request asynchronously and return the identity of the request for correlating received replies.
        
        :param request: The request to send.
        :type request: Union[rti.connextdds.DynamicData, object]
        :param params: Parameters used for writing the request.
        :type params: rti.connextdds.WriteParams
        :return: The identity of the request.
        :rtype: rti.connextdds.SampleIdentity
        """
        if params is None:
            params = rti.connextdds.WriteParams()
            params.replace_automatic_values = True
        await self._writer.write_async(request, params)
        return params.identity


    async def receive_replies_async(
        self,
        max_wait,                   # type: rti.connextdds.Duration  
        min_count=1,                # type: int
        related_request_id=None     # type: Optional[rti.connextdds.SampleIdentity]
    ):
        # type: (...) -> Union[rti.connextdds.DynamicData.LoanedSamples, object]
        """Wait for replies asynchronously and take them.
        
        :param max_wait: Maximum time to wait for replies before timing out.
        :type max_wait: rti.connextdds.Duration
        :param min_count: Minimum number of replies to receive, default 1.
        :type min_count: int
        :param related_request_id: The request id used to correlate replies, default None (receive any replies).
        :type related_request_id: Optional[rti.connextdds.SampleIdentity]
        :raises rti.connextdds.TimeoutError: Thrown if min_count not received within max_wait.
        :return: A loaned samples object containing the replies.
        :rtype: Union[rti.connextdds.DynamicData.LoanedSamples, object]
        """
        if not await self.wait_for_replies_async(max_wait, min_count, related_request_id):
            raise rti.connextdds.TimeoutError("Timed out waiting for replies")
        else:
            return self.take_replies(related_request_id)


    async def wait_for_replies_async(
        self, 
        max_wait,                   # type: rti.connextdds.Duration
        min_count=1,                # type: int
        related_request_id=None     # type: Optional[rti.connextdds.SampleIdentity]
    ):
        # type(...) -> bool
        """Wait for received replies asynchronously.

        :param max_wait: Maximum time to wait for replies before timing out.
        :type max_wait: rti.connextdds.Duration
        :param min_count: Minimum number of replies to receive, default 1.
        :type min_count: int
        :param related_request_id: The request id used to correlate replies, default None (receive any replies).
        :type related_request_id: Optional[rti.connextdds.SampleIdentity]
        :return: Boolean indicating whether min_count replies were received within max_wait time.
        :rtype: bool
        """
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
    """A replier object for handling request-reply interactions with DDS.

    :param request_type: The type of the request data.
    :type request_type: Union[rti.connextdds.DynamicType, type]
    :param reply_type: The type of the reply data.
    :type reply_type: Union[rti.connextdds.DynamicType, type]
    :param participant: The DomainParticipant that will hold the reply writer and request reader.
    :type participant: rti.connextdds.DomainParticipant
    :param service_name: Name that will be used to derive the topic name, defaults to None (rely only on custom topics).
    :type service_name: Optional[str]
    :param request_topic: Topic object or name that will be used for the request data, must be set if service_name is None, otherwise overrides service_name, defaults to None (use service_name).
    :type request_topic: Optional[Union[rti.connextdds.DynamicData.Topic, rti.connextdds.DynamicData.ContentFilteredTopic, str, object]]
    :param reply_topic: Topic object or name that will be used for the reply data, must be set if service_name is None, otherwise overrides service_name, defaults to None (use service_name).
    :type reply_topic: Optional[Union[rti.connextdds.DynamicData.Topic, str, object]]
    :param datawriter_qos: QoS object to use for reply writer, defaults to None (use default RequestReply QoS).
    :type datawriter_qos: Optional[rti.connextdds.DataWriterQos]
    :param datareader_qos: QoS object to use for request reader, defaults to None (use default RequestReply QoS).
    :type datareader_qos: Optional[rti.connextdds.DataReaderQos]
    :param publisher: Publisher used to hold reply writer, defaults to None (use participant builtin publisher).
    :type publisher: Optional[rti.connextdds.Publisher]
    :param subscriber: Subscriber used to hold request reader, defaults to None (use participant builtin subscriber).
    :type subscriber: Optional[rti.connextdds.Subscriber]
    :param on_reply_available: The callback that handles incoming requests.
    :type on_reply_available: Callable[[object], object]
    """
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
        """Send a reply asynchronously to a received request.
        
        :param reply: The reply to send.
        :type reply: Union[rti.connextdds.DynamicData, object]
        :param param: Parameters used for writing the request.
        :type param: Union[rti.connextdds.SampleIdentity, rti.connextdds.SampleInfo, rti.connextdds.WriteParams]
        :param final: Indicates whether this is the final reply for a specific request, default True.
        :type final: bool
        :raises rti.connextdds.InvalidArgumentError: Thrown if param is not a type that can be used for correlation.
        """
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
        """Receive a minimum number of requests asynchronously within a timeout period.

        :param max_wait: Maximum time to wait for requests before timing out. .
        :type max_wait: rti.connextdds.Duration
        :param min_count: Minimum number of requests to receive, default 1.
        :type min_count: int
        :raises rti.connextdds.TimeoutError: Thrown if min_count not received within max_wait.
        :return: A loaned samples object containing the requests.
        :rtype: Union[rti.connextdds.DynamicData.LoanedSamples, object]
        """
        if not await self.wait_for_requests_async(max_wait, min_count):
            raise rti.connextdds.TimeoutError("Timed out waiting for requests")
        else:
            return self.take_requests()


    async def wait_for_requests_async(self, max_wait, min_count=1):
        # type: (rti.connextdds.Duration, int) -> bool
        """Wait asynchronously for a minimum number of requests within a timeout period.

        :param max_wait: Maximum time to wait for requests before timing out. .
        :type max_wait: rti.connextdds.Duration
        :param min_count: Minimum number of requests to receive, default 1.
        :type min_count: int
        :return: Boolean indicating whether min_count requests were received within max_wait time.
        :rtype: bool
        """
        return await _util_async.wait_for_samples_async(
                self._reader,
                min_count,
                max_wait,
                self._waitset,
                self._any_sample_condition,
                self._notread_sample_condition)
