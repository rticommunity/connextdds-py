/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */

#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>
#include <dds_c/dds_c_infrastructure.h>
#include "PyQosPolicy.hpp"

namespace pyrti {
py::object get_policy_type_from_id(py::module& m, int id)
{
    switch (id) {
    case DDS_INVALID_QOS_POLICY_ID:
        return py::none();
    case DDS_USERDATA_QOS_POLICY_ID:
        return m.attr("UserData");
    case DDS_DURABILITY_QOS_POLICY_ID:
        return m.attr("Durability");
    case DDS_PRESENTATION_QOS_POLICY_ID:
        return m.attr("Presentation");
    case DDS_DEADLINE_QOS_POLICY_ID:
        return m.attr("Deadline");
    case DDS_LATENCYBUDGET_QOS_POLICY_ID:
        return m.attr("LatencyBudget");
    case DDS_OWNERSHIP_QOS_POLICY_ID:
        return m.attr("Ownership");
    case DDS_OWNERSHIPSTRENGTH_QOS_POLICY_ID:
        return m.attr("OwnershipStrength");
    case DDS_LIVELINESS_QOS_POLICY_ID:
        return m.attr("Liveliness");
    case DDS_TIMEBASEDFILTER_QOS_POLICY_ID:
        return m.attr("TimeBasedFilter");
    case DDS_PARTITION_QOS_POLICY_ID:
        return m.attr("Partition");
    case DDS_RELIABILITY_QOS_POLICY_ID:
        return m.attr("Reliability");
    case DDS_DESTINATIONORDER_QOS_POLICY_ID:
        return m.attr("DestinationOrder");
    case DDS_HISTORY_QOS_POLICY_ID:
        return m.attr("History");
    case DDS_RESOURCELIMITS_QOS_POLICY_ID:
        return m.attr("ResourceLimits");
    case DDS_ENTITYFACTORY_QOS_POLICY_ID:
        return m.attr("EntityFactory");
    case DDS_WRITERDATALIFECYCLE_QOS_POLICY_ID:
        return m.attr("WriterDataLifecycle");
    case DDS_READERDATALIFECYCLE_QOS_POLICY_ID:
        return m.attr("ReaderDataLifecycle");
    case DDS_TOPICDATA_QOS_POLICY_ID:
        return m.attr("TopicData");
    case DDS_GROUPDATA_QOS_POLICY_ID:
        return m.attr("GroupData");
    case DDS_TRANSPORTPRIORITY_QOS_POLICY_ID:
        return m.attr("TransportPriority");
    case DDS_LIFESPAN_QOS_POLICY_ID:
        return m.attr("Lifespan");
    case DDS_DURABILITYSERVICE_QOS_POLICY_ID:
        return m.attr("DurabilityService");
    case DDS_DATA_REPRESENTATION_QOS_POLICY_ID:
        return m.attr("DataRepresentation");
    case DDS_TYPE_CONSISTENCY_ENFORCEMENT_QOS_POLICY_ID:
        return m.attr("TypeConsistencyEnforcement");
    case DDS_DATATAG_QOS_POLICY_ID:
        return m.attr("DataTag");
    case DDS_WIREPROTOCOL_QOS_POLICY_ID:
        return m.attr("WireProtocol");
    case DDS_DISCOVERY_QOS_POLICY_ID:
        return m.attr("Discovery");
    case DDS_DATAREADERRESOURCELIMITS_QOS_POLICY_ID:
        return m.attr("DataReaderResourceLimits");
    case DDS_DATAWRITERRESOURCELIMITS_QOS_POLICY_ID:
        return m.attr("DataWriterResourceLimits");
    case DDS_DATAREADERPROTOCOL_QOS_POLICY_ID:
        return m.attr("DataReaderProtocol");
    case DDS_DATAWRITERPROTOCOL_QOS_POLICY_ID:
        return m.attr("DataWriterProtocol");
    case DDS_DOMAINPARTICIPANTRESOURCELIMITS_QOS_POLICY_ID:
        return m.attr("DomainParticipantResourceLimits");
    case DDS_EVENT_QOS_POLICY_ID:
        return m.attr("Event");
    case DDS_DATABASE_QOS_POLICY_ID:
        return m.attr("Database");
    case DDS_RECEIVERPOOL_QOS_POLICY_ID:
        return m.attr("ReceiverPool");
    case DDS_DISCOVERYCONFIG_QOS_POLICY_ID:
        return m.attr("DiscoveryConfig");
    case DDS_EXCLUSIVEAREA_QOS_POLICY_ID:
        return m.attr("ExclusiveArea");
    case DDS_SYSTEMRESOURCELIMITS_QOS_POLICY_ID:
        return m.attr("SystemResourceLimits");
    case DDS_TRANSPORTSELECTION_QOS_POLICY_ID:
        return m.attr("TransportSelection");
    case DDS_TRANSPORTUNICAST_QOS_POLICY_ID:
        return m.attr("TransportUnicast");
    case DDS_TRANSPORTMULTICAST_QOS_POLICY_ID:
        return m.attr("TransportMulticast");
    case DDS_TRANSPORTBUILTIN_QOS_POLICY_ID:
        return m.attr("TransportBuiltin");
    case DDS_TYPESUPPORT_QOS_POLICY_ID:
        return m.attr("TypeSupport");
    case DDS_PROPERTY_QOS_POLICY_ID:
        return m.attr("Property");
    case DDS_PUBLISHMODE_QOS_POLICY_ID:
        return m.attr("PublishMode");
    case DDS_ASYNCHRONOUSPUBLISHER_QOS_POLICY_ID:
        return m.attr("AsynchronousPublisher");
    case DDS_ENTITYNAME_QOS_POLICY_ID:
        return m.attr("EntityName");
    case DDS_SERVICE_QOS_POLICY_ID:
        return m.attr("Service");
    case DDS_BATCH_QOS_POLICY_ID:
        return m.attr("Batch");
    case DDS_LOCATORFILTER_QOS_POLICY_ID:
        return m.attr("LocatorFilter");
    case DDS_MULTICHANNEL_QOS_POLICY_ID:
        return m.attr("MultiChannel");
    case DDS_AVAILABILITY_QOS_POLICY_ID:
        return m.attr("Availability");
    case DDS_TRANSPORTMULTICASTMAPPING_QOS_POLICY_ID:
        return m.attr("TransportMulticastMapping");
    case DDS_TOPICQUERYDISPATCH_QOS_POLICY_ID:
        return m.attr("TopicQueryDispatch");
    case DDS_DATAWRITERTRANSFERMODE_QOS_POLICY_ID:
        return m.attr("DataWriterTransferMode");

    case DDS_PROFILE_QOS_POLICY_ID:
    case DDS_LOGGING_QOS_POLICY_ID:
    case DDS_USEROBJECT_QOS_POLICY_ID:
    case DDS_TRANSPORTENCAPSULATION_QOS_POLICY_ID:
    case DDS_PUBLISHERPROTOCOL_QOS_POLICY_ID:
    case DDS_SUBSCRIBERPROTOCOL_QOS_POLICY_ID:
    case DDS_TOPICPROTOCOL_QOS_POLICY_ID:
    case DDS_DOMAINPARTICIPANTPROTOCOL_QOS_POLICY_ID:
    default:
        throw dds::core::Error(
                "The policy " + std::to_string(id)
                + "is not supported in the Python binding");
    }
}
}  // namespace pyrti