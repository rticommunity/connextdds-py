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
try:
    from typing import Union, Optional, Callable
except ImportError:
    pass


class Requester(_util.RequestReplyBase):
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
            'Requester',
            request_type,
            reply_type,
            participant,
            'Request',
            'Reply',
            service_name,
            request_topic,
            reply_topic,
            datawriter_qos,
            datareader_qos,
            publisher,
            subscriber,
            on_reply_available,
            True,
            True
        )

        _util_native.create_correlation_index(self._reader)


    def send_request(self, request, params=None):
        # type: (Union[rti.connextdds.DynamicData, object], rti.connextdds.WriteParams) -> rti.connextdds.SampleIdentity
        """Send a request and return the identity of the request for correlating received replies.
        
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
        self._writer.write(request, params)
        return params.identity


    def receive_replies(
        self,
        max_wait,                   # type: rti.connextdds.Duration
        min_count=1,                # type: int
        related_request_id=None     # type: Optional[rti.connextdds.SampleIdentity]
    ):
        # type: (...) -> Union[rti.connextdds.DynamicData.LoanedSamples, object]
        """Wait for replies and take them.
        
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
        if not self.wait_for_replies(max_wait, min_count, related_request_id):
            raise rti.connextdds.TimeoutError("Timed out waiting for replies")
        else:
            return self.take_replies(related_request_id)


    def take_replies(self, related_request_id=None):
        # type: (Optional[rti.connextdds.SampleIdentity]) -> Union[rti.connextdds.DynamicData.LoanedSamples, object]
        """Take received replies.

        :param related_request_id: The id used to correlate replies to a specific request, default None (take any replies).
        :type related_request_id: Optional[rti.connextdds.SampleIdentity]
        :return: A loaned samples object containing the replies.
        :rtype: Union[rti.connextdds.DynamicData.LoanedSamples, object]
        """
        if related_request_id is None:
            return self._reader.take()
        else:
            condition = _util_native.create_correlation_condition(
                self._reader,
                rti.connextdds.SampleState.ANY,
                related_request_id.sequence_number)
            return self._reader.select().condition(condition).take()


    def read_replies(self, related_request_id=None):
        # type: (Optional[rti.connextdds.SampleIdentity]) -> Union[rti.connextdds.DynamicData.LoanedSamples, object]
        """Read received replies.

        :param related_request_id: The id used to correlate replies to a specific request, default None (read any replies).
        :type related_request_id: Optional[rti.connextdds.SampleIdentity]
        :return: A loaned samples object containing the replies.
        :rtype: Union[rti.connextdds.DynamicData.LoanedSamples, object]
        """
        if related_request_id is None:
            return self._reader.read()
        else:
            condition = _util_native.create_correlation_condition(
                self._reader,
                rti.connextdds.SampleState.ANY,
                related_request_id.sequence_number)
            return self._reader.select().condition(condition).read()


    def wait_for_replies(
        self, 
        max_wait,                   # type: rti.connextdds.Duration
        min_count=1,                # type: int
        related_request_id=None     # type: Optional[rti.connextdds.SampleIdentity]
    ):
        # type(...) -> bool
        """Wait for received replies.

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
            return _util.wait_for_samples(
                    self._reader,
                    min_count, max_wait,
                    waitset, initial_condition,
                    correlation_condition)


    @property
    def request_datawriter(self):
        # type: () -> Union[rti.connextdds.DynamicData.DataWriter, object]
        """The DataWriter used to send request data.

        :getter: Returns the request DataWriter.
        :type: Union[rti.connextdds.DynamicData.DataWriter, object]
        """
        return self._writer


    @property
    def reply_datareader(self):
        # type: () -> Union[rti.connextdds.DynamicData.DataReader, object]
        """The DataReader used to receive reply data.

        :getter: Returns the reply DataReader.
        :type: Union[rti.connextdds.DynamicData.DataReader, object]
        """
        return self._reader


    @property
    def matched_replier_count(self):
        # type: () -> int
        """The number of discovered matched repliers.

        :getter: Returns the number of matched repliers.
        :type: int
        """
        return _util.match_count(self._reader, self._writer, 'Replier')


    @property
    def on_reply_available(self):
        # type: () -> Optional[Callable[[Requester]]]
        """The listener callback used to process received replies.

        :getter: Returns the callback function.
        :setter: Set the callback function.
        :type: Optional[Callable[[Requester]]]
        """
        return self._callback


    @on_reply_available.setter
    def on_reply_available(self, callback):
        # type: (Optional[Callable[[Requester]]]) -> None
        if callback is None:
            self._callback = callback
            self._reader.bind_listener(None, rti.connextdds.StatusMask.NONE)
        else:
            self.__callback = callback
            listener_cls = _util.get_listener_class(self.__reply_type)
            listener = listener_cls(self, callback)
            self._reader.bind_listener(listener, rti.connextdds.StatusMask.DATA_AVAILABLE)


    @classmethod
    def is_related_reply(cls, request_id, reply_info):
        # type (rti.connextdds.SampleIdentity, Union[rti.connextdds.SampleInfo, object]) -> bool
        """Check a request if against a reply's metadata for correlation.

        :param request_id: The request id used to correlate replies.
        :type request_id: Optional[rti.connextdds.SampleIdentity]
        :param reply_info: The reply info used for the correlation check.
        :type reply_info: Union[rti.connextdds.SampleInfo, object]
        :return: Boolean indicating whether the request and reply are correlated.
        :rtype: bool
        """
        if isinstance(reply_info, rti.connextdds.SampleInfo):
            return reply_info.related_original_publication_virtual_sample_identity == request_id
        return reply_info.info.related_original_publication_virtual_sample_identity == request_id


    @classmethod
    def is_final_reply(cls, reply_info):
        # type (rti.connextdds.SampleIdentity, Union[rti.connextdds.SampleInfo, object]) -> bool
        """Check a reply is the last of the sequence.

        :param reply_info: The reply info with the flags to check.
        :type reply_info: Union[rti.connextdds.SampleInfo, object]
        :return: Boolean indicating whether reply is the last for a request.
        :rtype: bool
        """
        if isinstance(reply_info, rti.connextdds.SampleInfo):
            return rti.connextdds.SampleFlag.INTERMEDIATE_REPLY_SEQUENCE not in reply_info.flag
        return rti.connextdds.SampleFlag.INTERMEDIATE_REPLY_SEQUENCE not in reply_info.info.flag

class Replier(_util.RequestReplyBase):
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
            'Replier',
            reply_type,
            request_type,
            participant,
            'Reply',
            'Request',
            service_name,
            reply_topic,
            request_topic,
            datawriter_qos,
            datareader_qos,
            publisher,
            subscriber,
            on_request_available,
            True,
            False
        )


    def send_reply(
        self, 
        reply,      # type: Union[rti.connextdds.DynamicData, object]
        param,      # type: Union[rti.connextdds.SampleIdentity, rti.connextdds.SampleInfo, rti.connextdds.WriteParams]
        final=True  # type: bool
    ):
        # type: (...) -> None
        """Send a reply to a received request.
        
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


    def receive_requests(self, max_wait, min_count=1):
        # type: (rti.connextdds.Duration, int) -> Union[rti.connextdds.DynamicData.LoanedSamples, object]
        """Receive a minimum number of requests within a timeout period.

        :param max_wait: Maximum time to wait for requests before timing out. .
        :type max_wait: rti.connextdds.Duration
        :param min_count: Minimum number of requests to receive, default 1.
        :type min_count: int
        :raises rti.connextdds.TimeoutError: Thrown if min_count not received within max_wait.
        :return: A loaned samples object containing the requests.
        :rtype: Union[rti.connextdds.DynamicData.LoanedSamples, object]
        """
        if not self.wait_for_requests(max_wait, min_count):
            raise rti.connextdds.TimeoutError("Timed out waiting for requests")
        else:
            return self.take_requests()


    def take_requests(self):
        # type: () -> Union[rti.connextdds.DynamicData.LoanedSamples, object]
        """Take received requests.

        :return: A loaned samples object containing the requests.
        :rtype: Union[rti.connextdds.DynamicData.LoanedSamples, object]
        """
        return self._reader.take()


    def read_requests(self):
        # type: () -> Union[rti.connextdds.DynamicData.LoanedSamples, object]
        """Read received requests.

        :return: A loaned samples object containing the requests.
        :rtype: Union[rti.connextdds.DynamicData.LoanedSamples, object]
        """
        return self._reader.read()


    def wait_for_requests(self, max_wait, min_count=1):
        # type: (rti.connextdds.Duration, int) -> bool
        """Wait for a minimum number of requests within a timeout period.

        :param max_wait: Maximum time to wait for requests before timing out. .
        :type max_wait: rti.connextdds.Duration
        :param min_count: Minimum number of requests to receive, default 1.
        :type min_count: int
        :return: Boolean indicating whether min_count requests were received within max_wait time.
        :rtype: bool
        """
        return _util.wait_for_samples(
                self._reader,
                min_count,
                max_wait,
                self._waitset,
                self._any_sample_condition,
                self._notread_sample_condition)


    @property
    def reply_datawriter(self):
        # type(): Union[rti.connextdds.DynamicData.DataWriter, object]
        """The DataWriter used to send reply data.

        :getter: Returns the reply DataWriter.
        :type: Union[rti.connextdds.DynamicData.DataWriter, object]
        """
        return self._writer


    @property
    def request_datareader(self):
        # type(): Union[rti.connextdds.DynamicData.DataReader, object]
        """The DataReader used to receive request data.

        :getter: Returns the request DataReader.
        :type: Union[rti.connextdds.DynamicData.DataReader, object]
        """
        return self._reader

    
    @property
    def matched_requester_count(self):
        # type(): int
        """The number of discovered matched requesters.

        :getter: Returns the number of matched requesters.
        :type: int
        """
        return _util.match_count(self._reader, self._writer, 'Requester')


    @property
    def on_request_available(self):
        # type() -> Optional[Callable[[Replier]]]
        """The listener callback used to process received requests.

        :getter: Returns the callback function.
        :setter: Set the callback function.
        :type: Optional[Callable[[Replier]]]
        """
        return self._callback


    @on_request_available.setter
    def on_request_available(self, callback):
        # type(Optional[Callable[[Replier]]]) -> None
        if callback is None:
            self._reader.bind_listener(None, rti.connextdds.StatusMask.NONE)
            self._callback = None
        else:
            self._callback = callback
            listener_cls = _util.get_listener_class(self._reply_type)
            listener = listener_cls(self, callback)
            self._reader.bind_listener(listener, rti.connextdds.StatusMask.DATA_AVAILABLE)