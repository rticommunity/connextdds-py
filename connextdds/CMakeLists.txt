cmake_minimum_required(VERSION 3.15)
project(connextdds)
include(CheckCXXCompilerFlag)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{NDDSHOME}/resource/cmake")
find_package(
    RTIConnextDDS "5.3.1"
    REQUIRED
    COMPONENTS
        core
        routing_service
    )

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../pybind11 ${CMAKE_CURRENT_SOURCE_DIR}/../pybind11)

add_library(
    connextdds 
    MODULE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/misc/Constants.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/misc/PyDynamicTypeMap.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/misc/PyAsyncioExecutor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/misc/InitMisc.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/misc/DDSSTLBinds.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/status/StatusNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/status/EventCount64.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/status/DataWriterCacheStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/status/DataReaderProtocolStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/status/DataReaderCacheStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/status/ReliableWriterCacheChangedStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/status/ReliableReaderActivityChangedStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/status/EventCount32.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/status/ServiceRequestAcceptedStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/status/SampleLostState.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/status/DataWriterProtocolStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/status/DomainParticipantProtocolStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/cond/WaitSetProperty.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/cond/CondNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/xtypes/ACTMember.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/xtypes/LoanedDynamicData.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/xtypes/DynamicDataProperty.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/xtypes/ACTEnumMember.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/xtypes/ACTUnionMember.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/xtypes/DynamicDataMemberInfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/xtypes/DynamicDataTypeSerializationProperty.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/xtypes/UnidimensionalCollectionType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/xtypes/XtypesNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/MultiChannel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/TypeSupport.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/SystemResourceLimits.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/EntityName.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/TopicQueryDispatch.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/DiscoveryConfig.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/DomainParticipantResourceLimits.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/BuiltinTopicReaderResourceLimits.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/DataWriterResourceLimits.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/PolicyNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/DataReaderProtocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/Property.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/PublishMode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/Discovery.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/TransportMulticast.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/RtpsReliableWriterProtocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/DestinationOrderScopeKind.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/DataWriterProtocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/Database.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/TransportUnicast.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/WireProtocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/Service.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/Availability.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/TransportBuiltin.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/ReceiverPool.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/RefilterKind.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/DataReaderResourceLimits.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/TransportSelection.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/AcknowledgmentKind.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/LocatorFilter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/Event.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/DataWriterTransferMode.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/Batch.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/AsynchronousPublisher.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/policy/ExclusiveArea.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/BuiltinProfiles.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/QosProviderParams.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/LocatorFilterElement.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/LongDouble.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/ChannelSettings.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/VendorId.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/RtpsReliableReaderProtocol.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/ContentFilterProperty.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/ThreadSettings.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/SampleFlag.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/SampleIdentity.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/CoreNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/AllocationSettings.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/ProtocolVersion.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/SequenceNumber.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/Cookie.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/ProductVersion.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/Locator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/Guid.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/TransportMulticastSettings.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/core/EndpointGroup.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/config/ConfigNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/config/Logger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/topic/ContentFilterBase.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/topic/FilterSampleInfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/topic/ExpressionProperty.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/topic/TopicNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/topic/PrintFormatProperty.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/topic/ServiceRequest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/sub/status/StatusNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/sub/status/DataStateEx.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/sub/TopicQuery.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/sub/SubNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/pub/PubNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/pub/FlowController.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/domain/DomainNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/domain/DomainParticipantConfigParams.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/rti/RTINamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/status/RequestedDeadlineMissedStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/status/SampleLostStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/status/StatusNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/status/OfferedIncompatibleQosStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/status/PublicationMatchedStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/status/SubscriptionMatchedStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/status/SampleRejectedStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/status/LivelinessChangedStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/status/OfferedDeadlineMissedStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/status/RequestedIncompatibleQosStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/status/InconsistentTopicStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/status/SampleRejectedState.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/status/StatusMask.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/status/LivelinessLostStatus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/cond/Condition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/cond/CondNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/cond/StatusCondition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/cond/WaitSet.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/cond/GuardCondition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/EnumMember.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/AliasType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/StringType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/SequenceType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/UnionType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/UnionMember.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/StructType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/DynamicData.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/WStringType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/CollectionType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/ArrayType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/Member.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/EnumType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/ExtensibilityKind.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/TypeKind.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/DynamicType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/PrimitiveType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/xtypes/XtypesNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/LatencyBudget.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/QosPolicyCount.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/TopicData.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/DataTag.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/PolicyNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/Reliability.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/TimeBasedFilter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/History.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/Lifespan.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/WriterDataLifecycle.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/TypeConsistencyEnforcement.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/DestinationOrder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/Ownership.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/DurabilityService.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/Presentation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/ReaderDataLifecycle.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/Deadline.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/Partition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/ResourceLimits.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/GroupData.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/Liveliness.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/EntityFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/Durability.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/DataRepresentation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/OwnershipStrength.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/TransportPriority.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/policy/UserData.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/QosProvider.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/Duration.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/BytesTopicType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/KeyedBytesTopicType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/StringTopicType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/KeyedStringTopicType.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/Exception.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/Entity.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/InstanceHandle.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/CoreNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/Vector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/core/Time.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/topic/qos/QosNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/topic/qos/TopicQos.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/topic/AnyTopicListener.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/topic/BuiltinTopicKey.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/topic/SubscriptionBuiltinTopicData.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/topic/Filter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/topic/TopicBuiltinTopicData.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/topic/ParticipantBuiltinTopicData.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/topic/PublicationBuiltinTopicData.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/topic/AnyTopic.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/topic/TopicNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/qos/DataReaderQos.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/qos/QosNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/qos/SubscriberQos.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/status/StatusNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/status/DataState.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/cond/ReadCondition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/cond/CondNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/cond/QueryCondition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/GenerationCount.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/Query.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/CoherentAccess.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/PyIDataReader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/SampleInfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/Subscriber.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/SubNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/Rank.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/SubscriberListener.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/AnyDataReaderListener.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/sub/AnyDataReader.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/pub/qos/QosNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/pub/qos/PublisherQos.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/pub/qos/DataWriterQos.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/pub/PubNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/pub/Publisher.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/pub/SuspendedPublication.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/pub/PublisherListener.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/pub/CoherentSet.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/pub/AnyDataWriterListener.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/pub/AnyDataWriter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/domain/qos/DomainParticipantQos.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/domain/qos/QosNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/domain/qos/DomainParticipantFactoryQos.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/domain/DomainNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/domain/DomainParticipant.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/domain/DomainParticipantListener.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dds/DDSNamespace.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/connextdds.cpp"
)

target_link_libraries(connextdds PRIVATE pybind11::module)

if (RTICONNEXTDDS_VERSION VERSION_GREATER_EQUAL "6.0.0")
    target_link_libraries(connextdds PRIVATE
        ${CONNEXTDDS_EXTERNAL_LIBS}
        RTIConnextDDS::cpp2_api
        RTIConnextDDS::routing_service_infrastructure)
else()
    string(TOUPPER ${CMAKE_BUILD_TYPE} RTI_LIB_BUILD_TYPE)
    target_link_libraries(connextdds PRIVATE
        ${CONNEXTDDS_EXTERNAL_LIBS}
        ${CONNEXTDDS_CPP2_API_LIBRARIES_${RTI_LIB_BUILD_TYPE}_SHARED}
        ${ROUTING_SERVICE_API_LIBRARIES_${RTI_LIB_BUILD_TYPE}_SHARED})

    target_include_directories(connextdds PRIVATE ${CONNEXTDDS_INCLUDE_DIRS})

    if(CONNEXTDDS_DEFINITIONS MATCHES "-m64")
        string(REGEX REPLACE "-m64[ \t\r\n]*" "" CONNEXTDDS_DEFINITIONS "${CONNEXTDDS_DEFINITIONS}")
        target_compile_options(connextdds PRIVATE "-m64")
    elseif(CONNEXTDDS_DEFINITIONS MATCHES "-m32")
        string(REGEX REPLACE "-m32[ \t\r\n]*" "" CONNEXTDDS_DEFINITIONS "${CONNEXTDDS_DEFINITIONS}")
        target_compile_options(connextdds PRIVATE "-m32")
    endif()

    target_compile_definitions(connextdds PRIVATE "${CONNEXTDDS_DEFINITIONS}")
endif()


target_include_directories(connextdds PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

set_target_properties(connextdds PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                         SUFFIX "${PYTHON_MODULE_EXTENSION}")



check_cxx_compiler_flag(-std=c++17 HAVE_FLAG_STD_CXX17)
check_cxx_compiler_flag(-std=c++14 HAVE_FLAG_STD_CXX14)
if (HAVE_FLAG_STD_CXX17 AND RTICONNEXTDDS_VERSION VERSION_GREATER_EQUAL "6.0.0")
    set_target_properties(
        connextdds
        PROPERTIES
            CXX_STANDARD 17
    )
elseif(HAVE_FLAG_STD_CXX14)
    set_target_properties(
        connextdds
        PROPERTIES
            CXX_STANDARD 14
    )
else()
    set_target_properties(
        connextdds
        PROPERTIES
            CXX_STANDARD 11
    )
endif()

target_compile_options(
    connextdds
    PRIVATE
    "-fpic"
    "-fvisibility=hidden"
    "-flto"
)

target_link_options(
    connextdds
    PRIVATE
    "-fpic"
    "-flto"
)
