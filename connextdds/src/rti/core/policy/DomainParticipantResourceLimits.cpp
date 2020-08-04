#include "PyConnext.hpp"
#include "PySafeEnum.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template <>
void init_class_defs(py::class_<DomainParticipantResourceLimits> &cls) {
  cls.def(py::init<>(), "Create a default policy.")
      .def_property(
          "local_writer_allocation",
          (rti::core::AllocationSettings(DomainParticipantResourceLimits::*)()
               const) &
              DomainParticipantResourceLimits::local_writer_allocation,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(const rti::core::
                                                    AllocationSettings &)) &
              DomainParticipantResourceLimits::local_writer_allocation,
          "Allocation settings applied to local DataWriters."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "local_reader_allocation",
          (rti::core::AllocationSettings(DomainParticipantResourceLimits::*)()
               const) &
              DomainParticipantResourceLimits::local_reader_allocation,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(const rti::core::
                                                    AllocationSettings &)) &
              DomainParticipantResourceLimits::local_reader_allocation,
          "Allocation settings applied to local DataReaders."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "local_publisher_allocation",
          (rti::core::AllocationSettings(DomainParticipantResourceLimits::*)()
               const) &
              DomainParticipantResourceLimits::local_publisher_allocation,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(const rti::core::
                                                    AllocationSettings &)) &
              DomainParticipantResourceLimits::local_publisher_allocation,
          "Allocation settings applied to local Publisher."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "local_subscriber_allocation",
          (rti::core::AllocationSettings(DomainParticipantResourceLimits::*)()
               const) &
              DomainParticipantResourceLimits::local_subscriber_allocation,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(const rti::core::
                                                    AllocationSettings &)) &
              DomainParticipantResourceLimits::local_subscriber_allocation,
          "Allocation settings applied to local Subscriber."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property("local_topic_allocation",
                    (rti::core::AllocationSettings(
                        DomainParticipantResourceLimits::*)() const) &
                        DomainParticipantResourceLimits::local_topic_allocation,
                    (DomainParticipantResourceLimits &
                     (DomainParticipantResourceLimits::
                          *)(const rti::core::AllocationSettings &)) &
                        DomainParticipantResourceLimits::local_topic_allocation,
                    "Allocation settings applied to local Topic."
                    "\n\n"
                    "This property's getter returns a deep copy.")
      .def_property(
          "remote_writer_allocation",
          (rti::core::AllocationSettings(DomainParticipantResourceLimits::*)()
               const) &
              DomainParticipantResourceLimits::remote_writer_allocation,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(const rti::core::
                                                    AllocationSettings &)) &
              DomainParticipantResourceLimits::remote_writer_allocation,
          "Allocation settings applied to remote DataWriters."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "remote_reader_allocation",
          (rti::core::AllocationSettings(DomainParticipantResourceLimits::*)()
               const) &
              DomainParticipantResourceLimits::remote_reader_allocation,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(const rti::core::
                                                    AllocationSettings &)) &
              DomainParticipantResourceLimits::remote_reader_allocation,
          "Allocation settings applied to remote DataReaders."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "remote_participant_allocation",
          (rti::core::AllocationSettings(DomainParticipantResourceLimits::*)()
               const) &
              DomainParticipantResourceLimits::remote_participant_allocation,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(const rti::core::
                                                    AllocationSettings &)) &
              DomainParticipantResourceLimits::remote_participant_allocation,
          "Allocation settings applied to remote DomainParticipants."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property("matching_writer_reader_pair_allocation",
                    (rti::core::AllocationSettings(
                        DomainParticipantResourceLimits::*)() const) &
                        DomainParticipantResourceLimits::
                            matching_writer_reader_pair_allocation,
                    (DomainParticipantResourceLimits &
                     (DomainParticipantResourceLimits::
                          *)(const rti::core::AllocationSettings &)) &
                        DomainParticipantResourceLimits::
                            matching_writer_reader_pair_allocation,
                    "Allocation settings applied to matching local writer and "
                    "remote/local reader pairs."
                    "\n\n"
                    "This property's getter returns a deep copy.")
      .def_property("matching_reader_writer_pair_allocation",
                    (rti::core::AllocationSettings(
                        DomainParticipantResourceLimits::*)() const) &
                        DomainParticipantResourceLimits::
                            matching_reader_writer_pair_allocation,
                    (DomainParticipantResourceLimits &
                     (DomainParticipantResourceLimits::
                          *)(const rti::core::AllocationSettings &)) &
                        DomainParticipantResourceLimits::
                            matching_reader_writer_pair_allocation,
                    "Allocation settings applied to matching local reader and "
                    "remote/local writer pairs."
                    "\n\n"
                    "This property's getter returns a deep copy.")
      .def_property(
          "ignored_entity_allocation",
          (rti::core::AllocationSettings(DomainParticipantResourceLimits::*)()
               const) &
              DomainParticipantResourceLimits::ignored_entity_allocation,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(const rti::core::
                                                    AllocationSettings &)) &
              DomainParticipantResourceLimits::ignored_entity_allocation,
          "Allocation settings applied to ignored entities."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property("content_filtered_topic_allocation",
                    (rti::core::AllocationSettings(
                        DomainParticipantResourceLimits::*)() const) &
                        DomainParticipantResourceLimits::
                            content_filtered_topic_allocation,
                    (DomainParticipantResourceLimits &
                     (DomainParticipantResourceLimits::
                          *)(const rti::core::AllocationSettings &)) &
                        DomainParticipantResourceLimits::
                            content_filtered_topic_allocation,
                    "Allocation settings applied to content filtered topic."
                    "\n\n"
                    "This property's getter returns a deep copy.")
      .def_property(
          "content_filter_allocation",
          (rti::core::AllocationSettings(DomainParticipantResourceLimits::*)()
               const) &
              DomainParticipantResourceLimits::content_filter_allocation,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(const rti::core::
                                                    AllocationSettings &)) &
              DomainParticipantResourceLimits::content_filter_allocation,
          "Allocation settings applied to content filter."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "read_condition_allocation",
          (rti::core::AllocationSettings(DomainParticipantResourceLimits::*)()
               const) &
              DomainParticipantResourceLimits::read_condition_allocation,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(const rti::core::
                                                    AllocationSettings &)) &
              DomainParticipantResourceLimits::read_condition_allocation,
          "Allocation settings applied to read condition pool."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "query_condition_allocation",
          (rti::core::AllocationSettings(DomainParticipantResourceLimits::*)()
               const) &
              DomainParticipantResourceLimits::query_condition_allocation,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(const rti::core::
                                                    AllocationSettings &)) &
              DomainParticipantResourceLimits::query_condition_allocation,
          "Allocation settings applied to query condition pool."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "outstanding_asynchronous_sample_allocation",
          (rti::core::AllocationSettings(DomainParticipantResourceLimits::*)()
               const) &
              DomainParticipantResourceLimits::
                  outstanding_asynchronous_sample_allocation,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(const rti::core::
                                                    AllocationSettings &)) &
              DomainParticipantResourceLimits::
                  outstanding_asynchronous_sample_allocation,
          "Allocation settings applied to the maximum number of samples "
          "(from DataWriter) waiting to be asynchronously written."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "flow_controller_allocation",
          (rti::core::AllocationSettings(DomainParticipantResourceLimits::*)()
               const) &
              DomainParticipantResourceLimits::flow_controller_allocation,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(const rti::core::
                                                    AllocationSettings &)) &
              DomainParticipantResourceLimits::flow_controller_allocation,
          "Allocation settings applied to flow controllers."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "local_writer_hash_buckets",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::local_writer_hash_buckets,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::local_writer_hash_buckets,
          "Hash buckets settings applied to local DataWriters.")
      .def_property(
          "local_reader_hash_buckets",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::local_reader_hash_buckets,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::local_reader_hash_buckets,
          "Hash buckets settings applied to local DataReaders.")
      .def_property(
          "local_publisher_hash_buckets",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::local_publisher_hash_buckets,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::local_publisher_hash_buckets,
          "Number of hash buckets for local Publisher.")
      .def_property(
          "local_subscriber_hash_buckets",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::local_subscriber_hash_buckets,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::local_subscriber_hash_buckets,
          "Number of hash buckets for local Subscriber.")
      .def_property(
          "local_topic_hash_buckets",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::local_topic_hash_buckets,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::local_topic_hash_buckets,
          "Number of hash buckets for local Topic.")
      .def_property(
          "remote_writer_hash_buckets",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::remote_writer_hash_buckets,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::remote_writer_hash_buckets,
          "Number of hash buckets for remote DataWriters.")
      .def_property(
          "remote_reader_hash_buckets",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::remote_reader_hash_buckets,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::remote_reader_hash_buckets,
          "Number of hash buckets for remote DataReaders.")
      .def_property(
          "remote_participant_hash_buckets",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::remote_participant_hash_buckets,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::remote_participant_hash_buckets,
          "Number of hash buckets for remote DomainParticipants.")
      .def_property("matching_writer_reader_pair_hash_buckets",
                    (int32_t(DomainParticipantResourceLimits::*)() const) &
                        DomainParticipantResourceLimits::
                            matching_writer_reader_pair_hash_buckets,
                    (DomainParticipantResourceLimits &
                     (DomainParticipantResourceLimits::*)(int32_t)) &
                        DomainParticipantResourceLimits::
                            matching_writer_reader_pair_hash_buckets,
                    "Number of hash buckets for matching local writer and "
                    "remote/local reader pairs.")
      .def_property("matching_reader_writer_pair_hash_buckets",
                    (int32_t(DomainParticipantResourceLimits::*)() const) &
                        DomainParticipantResourceLimits::
                            matching_reader_writer_pair_hash_buckets,
                    (DomainParticipantResourceLimits &
                     (DomainParticipantResourceLimits::*)(int32_t)) &
                        DomainParticipantResourceLimits::
                            matching_reader_writer_pair_hash_buckets,
                    "Number of hash buckets for matching local reader and "
                    "remote/local writer pairs.")
      .def_property(
          "ignored_entity_hash_buckets",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::ignored_entity_hash_buckets,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::ignored_entity_hash_buckets,
          "Number of hash buckets for ignored entities.")
      .def_property("content_filtered_topic_hash_buckets",
                    (int32_t(DomainParticipantResourceLimits::*)() const) &
                        DomainParticipantResourceLimits::
                            content_filtered_topic_hash_buckets,
                    (DomainParticipantResourceLimits &
                     (DomainParticipantResourceLimits::*)(int32_t)) &
                        DomainParticipantResourceLimits::
                            content_filtered_topic_hash_buckets,
                    "Number of hash buckets for content filtered topics.")
      .def_property(
          "content_filter_hash_buckets",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::content_filter_hash_buckets,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::content_filter_hash_buckets,
          "Number of hash buckets for content filters.")
      .def_property(
          "flow_controller_hash_buckets",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::flow_controller_hash_buckets,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::flow_controller_hash_buckets,
          "Number of hash buckets for flow controllers.")
      .def_property(
          "max_gather_destinations",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::max_gather_destinations,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::max_gather_destinations,
          "Maximum number of destinations per RTI Connext send.")
      .def_property(
          "participant_user_data_max_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::participant_user_data_max_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::participant_user_data_max_length,
          "Maximum length of user data in DomainParticipantQos and "
          "ParticipantBuiltinTopicData.")
      .def_property(
          "topic_data_max_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::topic_data_max_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::topic_data_max_length,
          "Maximum length of topic data in TopicQos, TopicBuiltinTopicData, "
          "PublicationBuiltinTopicData and SubscriptionBuiltinTopicData.")
      .def_property(
          "publisher_group_data_max_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::publisher_group_data_max_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::publisher_group_data_max_length,
          "Maximum length of group data in PublisherQos and "
          "PublicationBuiltinTopicData.")
      .def_property(
          "subscriber_group_data_max_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::subscriber_group_data_max_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::subscriber_group_data_max_length,
          "Maximum length of group data in SubscriberQos and "
          "SubscriptionBuiltinTopicData.")
      .def_property(
          "writer_user_data_max_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::writer_user_data_max_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::writer_user_data_max_length,
          "Maximum length of user data in DataWriterQos and "
          "PublicationBuiltinTopicData.")
      .def_property(
          "reader_user_data_max_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::reader_user_data_max_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::reader_user_data_max_length,
          "Maximum length of user data in DataReaderQos and "
          "SubscriptionBuiltinTopicData.")
      .def_property("max_partitions",
                    (int32_t(DomainParticipantResourceLimits::*)() const) &
                        DomainParticipantResourceLimits::max_partitions,
                    (DomainParticipantResourceLimits &
                     (DomainParticipantResourceLimits::*)(int32_t)) &
                        DomainParticipantResourceLimits::max_partitions,
                    "Maximum number of partition name strings allowable in a "
                    "Partition.")
      .def_property(
          "max_partition_cumulative_characters",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::
                  max_partition_cumulative_characters,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::
                  max_partition_cumulative_characters,
          "Maximum number of combined characters allowable in all partition "
          "names in a Partition.")
      .def_property(
          "type_code_max_serialized_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::type_code_max_serialized_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::type_code_max_serialized_length,
          "Maximum size of serialized string for type code.")
      .def_property(
          "type_object_max_serialized_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::
                  type_object_max_serialized_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::
                  type_object_max_serialized_length,
          "The maximum length, in bytes, that the buffer to serialize a "
          "TypeObject can consume.")
      .def_property(
          "type_object_max_deserialized_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::
                  type_object_max_deserialized_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::
                  type_object_max_deserialized_length,
          "The maximum number of bytes that a deserialized TypeObject can "
          "consume.")
      .def_property(
          "deserialized_type_object_dynamic_allocation_threshold",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::
                  deserialized_type_object_dynamic_allocation_threshold,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::
                  deserialized_type_object_dynamic_allocation_threshold,
          "A threshold, in bytes, for dynamic memory allocation for the "
          "deserialized TypeObject.")
      .def_property("contentfilter_property_max_length",
                    (int32_t(DomainParticipantResourceLimits::*)() const) &
                        DomainParticipantResourceLimits::
                            contentfilter_property_max_length,
                    (DomainParticipantResourceLimits &
                     (DomainParticipantResourceLimits::*)(int32_t)) &
                        DomainParticipantResourceLimits::
                            contentfilter_property_max_length,
                    "This field is the maximum length of all data related to a "
                    "Content-filtered topic.")
      .def_property(
          "channel_seq_max_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::channel_seq_max_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::channel_seq_max_length,
          "Maximum number of channels that can be specified in MultiChannel "
          "for MultiChannel DataWriters.")
      .def_property(
          "channel_filter_expression_max_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::
                  channel_filter_expression_max_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::
                  channel_filter_expression_max_length,
          "Maximum length of a channel ChannelSettings.filter_expression in "
          "a MultiChannel DataWriter.")
      .def_property("participant_property_list_max_length",
                    (int32_t(DomainParticipantResourceLimits::*)() const) &
                        DomainParticipantResourceLimits::
                            participant_property_list_max_length,
                    (DomainParticipantResourceLimits &
                     (DomainParticipantResourceLimits::*)(int32_t)) &
                        DomainParticipantResourceLimits::
                            participant_property_list_max_length,
                    "Maximum number of properties associated with the "
                    "DomainParticipant.")
      .def_property(
          "participant_property_string_max_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::
                  participant_property_string_max_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::
                  participant_property_string_max_length,
          "Maximum string length of the properties associated with the "
          "DomainParticipant.")
      .def_property(
          "writer_property_list_max_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::writer_property_list_max_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::writer_property_list_max_length,
          "Maximum number of properties associated with a DataWriter.")
      .def_property("writer_property_string_max_length",
                    (int32_t(DomainParticipantResourceLimits::*)() const) &
                        DomainParticipantResourceLimits::
                            writer_property_string_max_length,
                    (DomainParticipantResourceLimits &
                     (DomainParticipantResourceLimits::*)(int32_t)) &
                        DomainParticipantResourceLimits::
                            writer_property_string_max_length,
                    "Maximum string length of the properties associated with a "
                    "DataWriter.")
      .def_property(
          "reader_property_list_max_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::reader_property_list_max_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::reader_property_list_max_length,
          "Maximum number of properties associated with a DataReader.")
      .def_property("reader_property_string_max_length",
                    (int32_t(DomainParticipantResourceLimits::*)() const) &
                        DomainParticipantResourceLimits::
                            reader_property_string_max_length,
                    (DomainParticipantResourceLimits &
                     (DomainParticipantResourceLimits::*)(int32_t)) &
                        DomainParticipantResourceLimits::
                            reader_property_string_max_length,
                    "Maximum string length of the properties associated with a "
                    "DataReader.")
      .def_property(
          "max_endpoint_groups",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::max_endpoint_groups,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::max_endpoint_groups,
          "Maximum number of EndpointGroup allowable in a Availability.")
      .def_property(
          "max_endpoint_group_cumulative_characters",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::
                  max_endpoint_group_cumulative_characters,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::
                  max_endpoint_group_cumulative_characters,
          "Maximum number of combined role_name characters allowed in all "
          "EndpointGroup in a Availability.")
      .def_property(
          "transport_info_list_max_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::transport_info_list_max_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::transport_info_list_max_length,
          "Maximum number of installed transports to send and receive "
          "information about in ParticipantBuiltinTopicData.transport_info.")
      .def_property(
          "ignored_entity_replacement_kind",
          (IgnoredEntityReplacementKind(DomainParticipantResourceLimits::*)()
               const) &
              DomainParticipantResourceLimits::ignored_entity_replacement_kind,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(IgnoredEntityReplacementKind)) &
              DomainParticipantResourceLimits::ignored_entity_replacement_kind,
          "Replacement policy for the ignored entities. It sets what entity "
          "can be replaced when resource limits set in "
          "DomainParticipantResourceLimits.ignored_entity_allocation are "
          "reached.")
      .def_property(
          "remote_topic_query_allocation",
          (rti::core::AllocationSettings(DomainParticipantResourceLimits::*)()
               const) &
              DomainParticipantResourceLimits::remote_topic_query_allocation,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(const rti::core::
                                                    AllocationSettings &)) &
              DomainParticipantResourceLimits::remote_topic_query_allocation,
          "Allocation settings applied to remote TopicQueries."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "remote_topic_query_hash_buckets",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::remote_topic_query_hash_buckets,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::remote_topic_query_hash_buckets,
          "Number of hash buckets for remote TopicQueries.")
#if rti_connext_version_gte(6, 0, 0)
      .def_property(
          "writer_data_tag_list_max_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::writer_data_tag_list_max_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::writer_data_tag_list_max_length,
          "Maximum number of data tags associated with a DataWriter.")
      .def_property("writer_data_tag_string_max_length",
                    (int32_t(DomainParticipantResourceLimits::*)() const) &
                        DomainParticipantResourceLimits::
                            writer_data_tag_string_max_length,
                    (DomainParticipantResourceLimits &
                     (DomainParticipantResourceLimits::*)(int32_t)) &
                        DomainParticipantResourceLimits::
                            writer_data_tag_string_max_length,
                    "Maximum string length of the data tags associated with a "
                    "DataWriter.")
      .def_property(
          "reader_data_tag_list_max_length",
          (int32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::reader_data_tag_list_max_length,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(int32_t)) &
              DomainParticipantResourceLimits::reader_data_tag_list_max_length,
          "Maximum number of data tags associated with a DataReader.")
      .def_property("reader_data_tag_string_max_length",
                    (int32_t(DomainParticipantResourceLimits::*)() const) &
                        DomainParticipantResourceLimits::
                            reader_data_tag_string_max_length,
                    (DomainParticipantResourceLimits &
                     (DomainParticipantResourceLimits::*)(int32_t)) &
                        DomainParticipantResourceLimits::
                            reader_data_tag_string_max_length,
                    "Maximum string length of the data tags associated with a "
                    "DataReader.")
      .def_property(
          "shmem_ref_transfer_mode_max_segments",
          (uint32_t(DomainParticipantResourceLimits::*)() const) &
              DomainParticipantResourceLimits::
                  shmem_ref_transfer_mode_max_segments,
          (DomainParticipantResourceLimits &
           (DomainParticipantResourceLimits::*)(uint32_t)) &
              DomainParticipantResourceLimits::
                  shmem_ref_transfer_mode_max_segments,
          "Maximum number of segments created by all DataWriters belonging "
          "to a DomainParticipant.")
#endif
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.");
}

template <>
void process_inits<DomainParticipantResourceLimits>(py::module &m,
                                                    ClassInitList &l) {
  init_dds_safe_enum<IgnoredEntityReplacementKind_def>(
      m, "IgnoredEntityReplacementKind", [](py::object &o) {
        py::enum_<IgnoredEntityReplacementKind::type>(o, "Enum")
            .value("NO_REPLACEMENT",
                   IgnoredEntityReplacementKind::type::NO_REPLACEMENT)
            .value("NOT_ALIVE_FIRST",
                   IgnoredEntityReplacementKind::type::NOT_ALIVE_FIRST)
            .export_values();
      });

  l.push_back([m]() mutable {
    return init_class<DomainParticipantResourceLimits>(
        m, "DomainParticipantResourceLimits");
  });
}

} // namespace pyrti
