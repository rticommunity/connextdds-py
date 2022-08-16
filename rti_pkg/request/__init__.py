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
try:
    from typing import Union, Optional, Callable, TYPE_CHECKING
except ImportError:
    pass
if TYPE_CHECKING:
    from ._async import *
else:
    try:
        from ._async import *
    except (ImportError, SyntaxError):
        from ._basic import *


class SimpleReplier(_util.RequestReplyBase):
    """A requester object that requires a callback to process a request and
    provide a single reply.

    :param request_type: The type of the request data.
    :type request_type: Union[rti.connextdds.DynamicType, type]
    :param reply_type: The type of the reply data.
    :type reply_type: Union[rti.connextdds.DynamicType, type]
    :param participant: The DomainParticipant that will hold the request reader and reply writer.
    :type participant: rti.connextdds.DomainParticipant
    :param handler: The callback that handles incoming requests and returns a reply.
    :type handler: Callable[[object], object]
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
    """
    @classmethod
    def _create_data_available_callback(cls, handler):
        # type: (Callable[[object], object]) -> Callable[[SimpleReplier]]
        def callback(replier):
            for data, info in replier._reader.take():
                if info.valid:
                    reply = handler(data)
                    _util.send_with_request_id(
                        replier._writer,
                        reply,
                        info.original_publication_virtual_sample_identity,
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
        """
        
        """
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
        """The number of discovered matched requesters.

        :getter: Returns the number of matched requesters.
        :type: int
        """
        return _util.match_count(self._reader, self._writer, 'Requester')
