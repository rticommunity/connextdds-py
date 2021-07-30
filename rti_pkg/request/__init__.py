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
        if params is None:
            params = rti.connextdds.WriteParams()
            params.replace_automatic_values = True
        self._writer.write(request, params)
        return params.identity


    def receive_replies(self, max_wait, min_count=1):
        # type: (rti.connextdds.Duration, int) -> Union[rti.connextdds.DynamicData.LoanedSamples, object]
        if not _util.wait_for_samples(
                self._reader, 
                min_count, 
                max_wait, 
                self._waitset, 
                self._any_sample_condition,
                self._notread_sample_condition):
            raise rti.connextdds.TimeoutError("Timed out waiting for replies")
        else:
            return self.__reader.take()


    def take_replies(self, related_request_id=None):
        # type: (Optional[rti.connextdds.SampleIdentity]) -> Union[rti.connextdds.DynamicData.LoanedSamples, object]
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
        return self._writer


    @property
    def reply_datareader(self):
        # type: () -> Union[rti.connextdds.DynamicData.DataReader, object]
        return self._reader


    @property
    def matched_replier_count(self):
        # type: () -> int
        return _util.match_count(self._reader, self._writer, 'Replier')


    @property
    def on_reply_available(self):
        # type: () -> Optional[Callable[[Requester]]]
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


class Replier(_util.RequestReplyBase):
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


    def receive_requests(self, max_wait, min_count=1):
        # type: (rti.connextdds.Duration, int) -> Union[rti.connextdds.DynamicData.LoanedSamples, object]
        if not _util.wait_for_samples(
                self._reader,
                min_count, max_wait,
                self._waitset,
                self._any_sample_condition,
                self._notread_sample_condition):
            raise rti.connextdds.TimeoutError("Timed out waiting for requests")
        else:
            return self._reader.take()


    def take_requests(self):
        # type: () -> Union[rti.connextdds.DynamicData.LoanedSamples, object]
        return self._reader.take()


    def read_requests(self):
        # type: () -> Union[rti.connextdds.DynamicData.LoanedSamples, object]
        return self._reader.read()


    def wait_for_requests(self, max_wait, min_count=1):
        # type: (rti.connextdds.Duration, int) -> bool
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
        return self._writer


    @property
    def request_datareader(self):
        # type(): Union[rti.connextdds.DynamicData.DataReader, object]
        return self._reader

    
    @property
    def matched_requester_count(self):
        # type(): int
        return _util.match_count(self._reader, self._writer, 'Requester')


    @property
    def on_request_available(self):
        # type() -> Optional[Callable[[Replier]]]
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


class SimpleReplier(_util.RequestReplyBase):
    @classmethod
    def _create_data_available_callback(cls, handler):
        # type: (Callable[[object], object]) -> Callable[[SimpleReplier]]
        def callback(replier):
            for sample in replier._reader.take():
                if sample.info.valid:
                    reply = handler(sample.data)
                    _util.send_with_request_id(
                        replier._writer,
                        reply,
                        sample.info.original_publication_virtual_sample_identity,
                        True)

        return callback


    def __init__(
        self,
        request_type,           # type: Union[rti.connextdds.DynamicType, type]
        reply_type,             # type: Union[rti.connextdds.DynamicType, type]
        participant,            # type: rti.connextdds.DomainParticipant
        handler,                # type: Callable[[object], object]
        service_name=None,      # type: Optional[str]
        request_topic=None,     # type: Optional[Union[rti.connextdds.DynamicData.Topic, rti.connextdds.DynamicData.ContentFilteredTopic, str, object]]
        reply_topic=None,       # type: Optional[Union[rti.connextdds.DynamicData.Topic, str, object]]
        datawriter_qos=None,    # type: Optional[rti.connextdds.DataWriterQos]
        datareader_qos=None,    # type: Optional[rti.connextdds.DataReaderQos]
        publisher=None,         # type: Optional[rti.connextdds.Publisher]
        subscriber=None,        # type: Optional[rti.connextdds.Subscriber]
    ):
        # type: (...) -> None
        callback = SimpleReplier._create_data_available_callback(handler)

        super(SimpleReplier, self).__init__(
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
            callback,
            False,
            False
        )


    @property
    def matched_requester_count(self):
        # type: () -> int
        return _util.match_count(self._reader, self._writer, 'Requester')
