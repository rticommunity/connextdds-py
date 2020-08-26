
.. py:currentmodule:: rti.connextdds

Quick reference
~~~~~~~~~~~~~~~
This section includes links to common ``rti.connextdds`` types.

DDS Entities:
   - :class:`DomainParticipant`
   - :class:`Publisher`
   - :class:`Subscriber`
   - :class:`IAnyTopic`, :class:`DynamicData.Topic`, :class:`DynamicData.ContentFilteredTopic`
   - :class:`IAnyDataReader`, :class:`DynamicData.DataReader`
   - :class:`IAnyDataWriter`, :class:`DynamicData.DataWriter`

Data types:
   - :class:`DynamicData` and :class:`DynamicType`
   - Loading types from XML: :meth:`QosProvider.type`
   - Built-in types: :class:`StringTopicType`, :class:`KeyedStringTopicType`,
     :class:`BytesTopicType`, :class:`KeyedBytesTopicType`

Quality of Service (QoS):
   - :class:`QosProvider`
   - :class:`DomainParticipantQos`
   - :class:`TopicQos`
   - :class:`PublisherQos`
   - :class:`SubscriberQos`
   - :class:`DataReaderQos`
   - :class:`DataWriterQos`

Listeners:
   - :class:`DomainParticipantListener`
   - :class:`TopicListener`
   - :class:`PublisherListener`
   - :class:`SubscriberListener`
   - :class:`DynamicData.DataReaderListener`
   - :class:`DynamicData.DataWriterListener`

Conditions:
   - :class:`WaitSet`
   - :class:`Condition`
   - :class:`GuardCondition`
   - :class:`StatusCondition`
   - :class:`ReadCondition`
   - :class:`QueryCondition`

Full module documentation: :mod:`rti.connextdds`
