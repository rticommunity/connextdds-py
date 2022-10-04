#  (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
#  RTI grants Licensee a license to use, modify, compile, and create derivative
#  works of the Software.  Licensee has the right to distribute object form only
#  for use with RTI products.  The Software is provided "as is", with no warranty
#  of any type, including any warranty for fitness for any purpose. RTI is under
#  no obligation to maintain or support the Software.  RTI shall not be liable for
#  any incidental or consequential damages arising out of the use or inability to
#  use the software.


import rti.connextdds
try:
    from typing import Union, Optional, Callable
except ImportError:
    pass


INT_MAX = (2 ** 31) - 1
GUID_FIELD_NAME = "@related_sample_identity.writer_guid.value"
reader_listeners = {}


class MetaRequestReply(type):
    @property
    def default_writer_qos(cls):
        # type: () -> rti.connextdds.DataWriterQos
        qos = rti.connextdds.DataWriterQos()
        qos.reliability.kind = rti.connextdds.ReliabilityKind.RELIABLE
        qos.reliability.max_blocking_time = (10, 0)
        qos.history.kind = rti.connextdds.HistoryKind.KEEP_ALL
        qos.resource_limits.max_samples = rti.connextdds.LENGTH_UNLIMITED
        qos.data_writer_resource_limits.max_remote_readers = rti.connextdds.LENGTH_UNLIMITED
        rtps_reliable_writer = rti.connextdds.RtpsReliableWriterProtocol()
        rtps_reliable_writer.max_heartbeat_retries = rti.connextdds.LENGTH_UNLIMITED
        rtps_reliable_writer.heartbeats_per_max_samples = 2
        rtps_reliable_writer.heartbeat_period = (0, 100000000)
        rtps_reliable_writer.fast_heartbeat_period = (0, 10000000)
        rtps_reliable_writer.late_joiner_heartbeat_period = (0, 10000000)
        rtps_reliable_writer.max_nack_response_delay = rti.connextdds.Duration.zero
        rtps_reliable_writer.min_nack_response_delay = rti.connextdds.Duration.zero
        rtps_reliable_writer.min_send_window_size = 32
        rtps_reliable_writer.max_send_window_size = 32
        qos.data_writer_protocol.rtps_reliable_writer = rtps_reliable_writer
        return qos


    @property
    def default_reader_qos(cls):
        # type: () -> rti.connextdds.DataReaderQos
        qos = rti.connextdds.DataReaderQos()
        qos.reliability.kind = rti.connextdds.ReliabilityKind.RELIABLE
        qos.history.kind = rti.connextdds.HistoryKind.KEEP_ALL
        qos.data_reader_protocol.rtps_reliable_reader.min_heartbeat_response_delay = rti.connextdds.Duration.zero
        qos.data_reader_protocol.rtps_reliable_reader.max_heartbeat_response_delay = rti.connextdds.Duration.zero
        return qos


def get_or_create_topic_from_name(type_class, data_type, participant, topic_name):
    try:
        topic = type_class.Topic.find(participant, topic_name)
    except rti.connextdds.InvalidDowncastError:
        topic = type_class.ContentFilteredTopic.find(participant, topic_name)
    if topic is None:
        if isinstance(data_type, rti.connextdds.DynamicType):
            topic = type_class.Topic(participant, topic_name, data_type)
        else:
            topic = type_class.Topic(participant, topic_name, data_type)
    return topic


def get_topic(
    data_type,          # type: Union[rti.connextdds.DynamicType, type]
    participant,        # type: rti.connextdds.DomainParticipant
    service_name,       # type: str
    custom_topic,       # type: Optional[Union[rti.connextdds.DynamicData.Topic, rti.connextdds.DynamicData.ContentFilteredTopic, Optional[str], object]]
    service_suffix,     # type: str
    cft_allowed=True    # type: bool
):
    # type: (...) -> Union[rti.connextdds.DynamicData.Topic, rti.connextdds.DynamicData.ContentFilteredTopic, object]
    if isinstance(data_type, rti.connextdds.DynamicType):
        type_class = rti.connextdds.DynamicData
    else:
        type_class = rti.connextdds
    if custom_topic is not None:
        # custom topic overrides service name
        if isinstance(custom_topic, str):
            topic = get_or_create_topic_from_name(type_class, data_type, participant, custom_topic)
        else:
            topic = custom_topic
            if not (isinstance(topic, type_class.Topic) or isinstance(topic, type_class.ContentFilteredTopic)):
                raise rti.connextdds.InvalidArgumentError("Invalid topic type")
    else:
        assert service_name, "Must provide service name or topic"
        topic_name = service_name + service_suffix
        topic = get_or_create_topic_from_name(type_class, data_type, participant, topic_name)
    if not cft_allowed and isinstance(topic, type_class.ContentFilteredTopic):
        raise rti.connextdds.InvalidArgumentError("ContentFilteredTopic cannot be used for this endpoint")
    return topic


def get_listener_class(data_type):
    # type: (Union[rti.connextdds.DynamicType, type]) -> type
    if isinstance(data_type, rti.connextdds.DynamicType):
        data_type = rti.connextdds.DynamicData
        base_class = rti.connextdds.DynamicData.NoOpDataReaderListener
    else:
        base_class = rti.connextdds.NoOpDataReaderListener
    if data_type in reader_listeners:
        return reader_listeners[data_type.__repr__]
    listener_class = type(base_class.__name__ + 'Listener', (base_class,), {})

    def listener_init(self, obj, callback_func):
        super(listener_class, self).__init__()
        self._obj = obj
        self._callback_func = callback_func
    
    def on_data_available(self, reader):
        self._callback_func(self._obj)

    listener_class.__init__ = listener_init
    listener_class.on_data_available = on_data_available
    reader_listeners[data_type.__repr__] = listener_class
    return listener_class


def create_writer(
    role_name,                      # type: str
    writer_type,                    # type: Union[rti.connextdds.DynamicType, type]
    participant,                    # type: rti.connextdds.DomainParticipant
    service_name,                   # type: Optional[str]
    writer_topic,                   # type: Optional[Union[rti.connextdds.DynamicData.Topic, object]]
    datawriter_qos,                 # type: Optional[rti.connextdds.DataWriterQos]
    publisher,                      # type: Optional[rti.connextdds.Publisher]
    default_writer_topic_suffix     # type: str
):
    # type(...) -> Union[rti.connextdds.DynamicData.DataWriter, object]
    topic = get_topic(writer_type, participant, service_name, writer_topic, default_writer_topic_suffix, False)
    if publisher is None:
        publisher = participant.implicit_publisher
    if datawriter_qos is None:
        datawriter_qos = RequestReplyBase.default_writer_qos
    if datawriter_qos.entity_name.role_name is None:
        datawriter_qos.entity_name.role_name = role_name
    if isinstance(writer_type, rti.connextdds.DynamicType):
        writer = rti.connextdds.DynamicData.DataWriter(publisher, topic, datawriter_qos)
    else:
        writer = rti.connextdds.DataWriter(publisher, topic, datawriter_qos)
    return writer


def create_reader(
    role_name,                      # type: str
    reader_type,                    # type: Union[rti.connextdds.DynamicType, type]
    participant,                    # type: rti.connextdds.DomainParticipant
    service_name,                   # type: Optional[str]
    reader_topic,                   # type: Optional[Union[rti.connextdds.DynamicData.Topic, rti.connextdds.DynamicData.ContentFilteredTopic, object]]
    datareader_qos,                 # type: Optional[rti.connextdds.DataReaderQos]
    subscriber,                     # type: Optional[rti.connextdds.Subscriber]
    on_data_available,              # type: Optional[Callable[[object]]]
    callback_obj,                   # type: Optional[object]
    default_reader_topic_suffix,    # type: str
    cft_guid                        # type: Optional[str]
):
    # type: (...) -> None
    topic = get_topic(reader_type, participant, service_name, reader_topic, default_reader_topic_suffix, cft_guid is None)

    if isinstance(reader_type, rti.connextdds.DynamicType):
        reader_cls = rti.connextdds.DynamicData
    else:
        reader_cls = rti.connextdds

    # Create correlation CFT
    if cft_guid is not None:
        filter_name = '{}_{}'.format(topic.name, cft_guid)
        filter_expression = '{} = &hex({})'.format(GUID_FIELD_NAME, cft_guid)
        topic = reader_cls.ContentFilteredTopic(topic, filter_name, rti.connextdds.Filter(filter_expression))

    if subscriber is None:
        subscriber = participant.implicit_subscriber
    if datareader_qos is None:
        datareader_qos = RequestReplyBase.default_reader_qos
    if datareader_qos.entity_name.role_name is None:
        datareader_qos.entity_name.role_name = role_name
    if on_data_available is not None:
        listener_cls = get_listener_class(reader_type)
        listener = listener_cls(callback_obj, on_data_available)
        mask = rti.connextdds.StatusMask.DATA_AVAILABLE
    else:
        listener = None
        mask = rti.connextdds.StatusMask.NONE
    reader = reader_cls.DataReader(subscriber, topic, datareader_qos, listener, mask)
    return reader


def wait_for_samples(
    reader,             # type: Union[rti.connextdds.DynamicData.DataReader, object]
    min_count,          # type: int
    max_wait,           # type: rti.connextdds.Duration
    waitset,            # type: rti.connextdds.WaitSet
    initial_condition,  # type: rti.connextdds.ICondition
    condition           # type: rti.connextdds.ICondition
):
    # type: (...) -> bool
    if min_count == rti.connextdds.LENGTH_UNLIMITED:
        min_count = INT_MAX
        
    sample_count = len(reader.select().max_samples(min_count).condition(initial_condition).read())
    min_count -= sample_count

    participant = reader.subscriber.participant
    remaining_wait = max_wait
    
    while min_count > 0:
        if min_count == 1:
            conditions = waitset.wait(remaining_wait)
        else:
            before_time = participant.current_time
            conditions = waitset.wait(remaining_wait)
            wait_time = participant.current_time - before_time
            remaining_wait -= wait_time

        # WaitSet should only have one condition
        if len(conditions) != 1:
            return False

        if min_count > 1:
            sample_count = reader.select().max_samples(min_count).condition(condition).read().length
            min_count -= sample_count
        else:
            min_count -= 1

    return True


def validate_related_request_id(related_request_id):
    # type: (rti.connextdds.SampleIdentity) -> None
    if (
        related_request_id.writer_guid == rti.connextdds.Guid.automatic or 
        related_request_id.writer_guid == rti.connextdds.Guid.unknown
       ):
        raise rti.connextdds.InvalidArgumentError("related_request_id.writer_guid")
    if (
        related_request_id.sequence_number == rti.connextdds.SequenceNumber.automatic or
        related_request_id.sequence_number == rti.connextdds.SequenceNumber.unknown or
        related_request_id.sequence_number == rti.connextdds.SequenceNumber.maximum or
        related_request_id.sequence_number == rti.connextdds.SequenceNumber.zero
       ):
        raise rti.connextdds.InvalidArgumentError("related_request_id.sequence_number")


def send_with_request_id(
    writer,     # type: Union[rti.connextdds.DynamicData.DataWriter, object]
    reply,      # type: Union[rti.connextdds.DynamicData, object]
    request_id, # type: rti.connextdds.SampleIdentity
    final       # type: bool
):
    # type: (...) -> None
    validate_related_request_id(request_id)
    params = rti.connextdds.WriteParams()
    params.related_sample_identity = request_id
    if not final:
        params.flag = rti.connextdds.SampleFlag.INTERMEDIATE_REPLY_SEQUENCE
    writer.write(reply, params)


def match_count(
    reader,     # type: Union[rti.connextdds.DynamicData.DataReader, object]
    writer,     # type: Union[rti.connextdds.DynamicData.DataWriter, object]
    role_name   # str
):
    # type: (...) -> int
    pubs = reader.matched_publications
    subs = writer.matched_subscriptions
    
    pub_count = len([pub for pub in pubs 
                    if reader.matched_publication_data(pub).publication_name.role_name == role_name])

    sub_count = len([sub for sub in subs
                    if writer.matched_subscription_data(sub).subscription_name.role_name == role_name])

    return min(pub_count, sub_count)


RRMETA = MetaRequestReply('RRMETA', (object,), {'__slots__': ()})

class RequestReplyBase(RRMETA):
    """Base class used for request-reply objects.

    :param role_name: The role name that will be used for the readers and writers created.
    :type role_name: str
    :param writer_type: The type of the writer data.
    :type writer_type: Union[rti.connextdds.DynamicType, type]
    :param reader_type: The type of the reader data.
    :type reader_type: Union[rti.connextdds.DynamicType, type]
    :param participant: The DomainParticipant that will hold the reader and writer.
    :type participant: rti.connextdds.DomainParticipant
    :param default_writer_topic_suffix: Suffix to use for writer topic name if Topic needs to be created.
    :type default_writer_topic_suffix: str
    :param default_reader_topic_suffix: Suffix to use for reader topic name if Topic needs to be created.
    :type default_reader_topic_suffix: str
    :param service_name: Name that will be used to derive the topic name, defaults to None (rely only on custom topics).
    :type service_name: Optional[str]
    :param writer_topic: Topic object or name that will be used for the writer data, must be set if service_name is None, otherwise overrides service_name, defaults to None (use service_name).
    :type writer_topic: Optional[Union[rti.connextdds.DynamicData.Topic, rti.connextdds.DynamicData.ContentFilteredTopic, str, object]]
    :param reader_topic: Topic object or name that will be used for the reader data, must be set if service_name is None, otherwise overrides service_name, defaults to None (use service_name).
    :type reader_topic: Optional[Union[rti.connextdds.DynamicData.Topic, str, object]]
    :param datawriter_qos: QoS object to use for the writer, defaults to None (use default RequestReply QoS).
    :type datawriter_qos: Optional[rti.connextdds.DataWriterQos]
    :param datareader_qos: QoS object to use for the reader, defaults to None (use default RequestReply QoS).
    :type datareader_qos: Optional[rti.connextdds.DataReaderQos]
    :param publisher: Publisher used to hold the writer, defaults to None (use participant builtin publisher).
    :type publisher: Optional[rti.connextdds.Publisher]
    :param subscriber: Subscriber used to hold the reader, defaults to None (use participant builtin subscriber).
    :type subscriber: Optional[rti.connextdds.Subscriber]
    :param on_data_available: Callback function used to process data received by the reader, defaults to None (no callback).
    :type on_data_available: Optional[Callable[[object]]]
    :param use_waitset: Create a waitset for processing received data.
    :type use_waitset: bool
    :param use_cft: Create a CFT on the writer GUID for the reader.
    :type use_cft: bool
    """
    def __init__(
        self,
        role_name,                      # type: str
        writer_type,                    # type: Union[rti.connextdds.DynamicType, type]
        reader_type,                    # type: Union[rti.connextdds.DynamicType, type]
        participant,                    # type: rti.connextdds.DomainParticipant
        default_writer_topic_suffix,    # type: str
        default_reader_topic_suffix,    # type: str
        service_name=None,              # type: Optional[str]
        writer_topic=None,              # type: Optional[Union[rti.connextdds.DynamicData.Topic, str, object]]
        reader_topic=None,              # type: Optional[Union[rti.connextdds.DynamicData.Topic, rti.connextdds.DynamicData.ContentFilteredTopic, str, object]]
        datawriter_qos=None,            # type: Optional[rti.connextdds.DataWriterQos]
        datareader_qos=None,            # type: Optional[rti.connextdds.DataReaderQos]
        publisher=None,                 # type: Optional[rti.connextdds.Publisher]
        subscriber=None,                # type: Optional[rti.connextdds.Subscriber]
        on_data_available=None,         # type: Optional[Callable[[object]]]
        use_waitset=True,               # type: bool
        use_cft=False                   # type: bool
    ):
        # type: (...) -> None
        # Create DataWriter
        self._writer = create_writer(
            role_name,
            writer_type,
            participant,
            service_name,
            writer_topic,
            datawriter_qos,
            publisher,
            default_writer_topic_suffix)

        if use_cft:
            writer_guid = str(self._writer.qos.data_writer_protocol.virtual_guid)
        else:
            writer_guid = None

        # Create DataReader
        self._reader = create_reader(
            role_name,
            reader_type,
            participant,
            service_name,
            reader_topic,
            datareader_qos,
            subscriber,
            on_data_available,
            self,
            default_reader_topic_suffix,
            writer_guid)

        if use_waitset:
            # Create ReadConditions
            self._any_sample_condition = rti.connextdds.ReadCondition(
                    self._reader,
                    rti.connextdds.DataState.any)
            self._notread_sample_condition = rti.connextdds.ReadCondition(
                    self._reader,
                    rti.connextdds.SampleState.NOT_READ)

            # Create reply WaitSet
            self._waitset = rti.connextdds.WaitSet()
            self._waitset += self._notread_sample_condition
        else:
            self._waitset = None

        self._callback = on_data_available
        self._writer_type = writer_type
        self._reader_type = reader_type
        self._closed = False


    def close(self):
        # type() -> None
        """Close the resources for this request-reply object.
        """
        if self._closed:
            raise rti.connextdds.AlreadyClosedError('This request-reply object has already been closed')
        if self._waitset is not None:
            self._waitset.detach_all()
            self._waitset = None
            self._any_sample_condition.close()
            self._notread_sample_condition.close()
        self._callback = None
        self._writer.close()
        self._reader.close()
        self._reply_type = None
        self._request_type = None
        self._closed = True


    @property
    def closed(self):
        # type() -> bool
        """Returns true if this request-reply object has been closed.

        :getter: Returns the number of matched requesters.
        :type: bool
        """
        return self._closed
