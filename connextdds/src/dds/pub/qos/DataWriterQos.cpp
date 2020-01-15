#include "PyConnext.hpp"
#include <dds/pub/qos/DataWriterQos.hpp>
#include "PyQos.hpp"

namespace py = pybind11;

using namespace dds::pub::qos;
using namespace dds::core::policy;
using namespace rti::core::policy;

template<>
void pyrti::init_class_defs(py::class_<DataWriterQos>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a DataWriterQos with the default value for each policy."
        );
    pyrti::add_qos_property<DataWriterQos, Durability>(cls, "durability", "Durability");
    pyrti::add_qos_property<DataWriterQos, DurabilityService>(cls, "durability_service", "DurabilityService");
    pyrti::add_qos_property<DataWriterQos, Deadline>(cls, "deadline", "Deadline");
    pyrti::add_qos_property<DataWriterQos, LatencyBudget>(cls, "latency_budget", "LatencyBudget");
    pyrti::add_qos_property<DataWriterQos, Liveliness>(cls, "liveliness", "Liveliness");
    pyrti::add_qos_property<DataWriterQos, Reliability>(cls, "reliability", "Reliability");
    pyrti::add_qos_property<DataWriterQos, DestinationOrder>(cls, "destination_order", "DestinationOrder");
    pyrti::add_qos_property<DataWriterQos, History>(cls, "history", "History");
    pyrti::add_qos_property<DataWriterQos, ResourceLimits>(cls, "resource_limits", "ResourceLimits");
    pyrti::add_qos_property<DataWriterQos, TransportPriority>(cls, "transport_priority", "TransportPriority");
    pyrti::add_qos_property<DataWriterQos, Lifespan>(cls, "lifespan", "Lifespan");
    pyrti::add_qos_property<DataWriterQos, UserData>(cls, "user_data", "UserData");
    pyrti::add_qos_property<DataWriterQos, Ownership>(cls, "ownership", "Ownership");
    pyrti::add_qos_property<DataWriterQos, OwnershipStrength>(cls, "ownership_strength", "OwnershipStrength");
    pyrti::add_qos_property<DataWriterQos, WriterDataLifecycle>(cls, "writer_data_lifecycle", "WriterDataLifecycle");
    pyrti::add_qos_property<DataWriterQos, DataRepresentation>(cls, "data_representation", "DataRepresentation");
    pyrti::add_qos_property<DataWriterQos, DataTag>(cls, "data_tag", "DataTag");
    pyrti::add_qos_property<DataWriterQos, DataWriterResourceLimits>(cls, "data_writer_resource_limits", "DataWriterResourceLimits");
    pyrti::add_qos_property<DataWriterQos, DataWriterProtocol>(cls, "data_writer_protocol", "DataWriterProtocol");
    pyrti::add_qos_property<DataWriterQos, TransportSelection>(cls, "transport_selection", "TransportSelection");
    pyrti::add_qos_property<DataWriterQos, TransportUnicast>(cls, "transport_unicast", "TransportUnicast");
    pyrti::add_qos_property<DataWriterQos, PublishMode>(cls, "publish_mode", "PublishMode");
    pyrti::add_qos_property<DataWriterQos, Property>(cls, "property", "Property");
    pyrti::add_qos_property<DataWriterQos, Service>(cls, "service", "Service");
    pyrti::add_qos_property<DataWriterQos, Batch>(cls, "batch", "Batch");
    pyrti::add_qos_property<DataWriterQos, MultiChannel>(cls, "multi_channel", "MultiChannel");
    pyrti::add_qos_property<DataWriterQos, Availability>(cls, "availability", "Availability");
    pyrti::add_qos_property<DataWriterQos, EntityName>(cls, "entity_name", "EntityName");
    pyrti::add_qos_property<DataWriterQos, TopicQueryDispatch>(cls, "topic_query_dispatch", "TopicQueryDispatch");
    pyrti::add_qos_property<DataWriterQos, DataWriterTransferMode>(cls, "data_writer_transfer_mode", "DataWriterTransferMode");
    pyrti::add_qos_property<DataWriterQos, TypeSupport>(cls, "type_support", "TypeSupport");
}

template<>
void pyrti::process_inits<DataWriterQos>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<DataWriterQos>(m, "DataWriterQos");
        }
    ); 
}
