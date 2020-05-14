#include "PyConnext.hpp"
#include <dds/pub/qos/DataWriterQos.hpp>
#include "PyQos.hpp"

namespace py = pybind11;

using namespace dds::pub::qos;
using namespace dds::core::policy;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<DataWriterQos>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a DataWriterQos with the default value for each policy."
        )
        .def(
            py::init(
                [](const PyIAnyDataWriter& dw) {
                    return dw.py_qos();
                }
            ),
             py::arg("writer"),
            "Create a DataWriterQos with settings equivalent to those "
            "of the provided DataWriter."
        );
    
    add_qos_property<DataWriterQos, Durability>(cls, "durability", "Durability");
    add_qos_property<DataWriterQos, DurabilityService>(cls, "durability_service", "DurabilityService");
    add_qos_property<DataWriterQos, Deadline>(cls, "deadline", "Deadline");
    add_qos_property<DataWriterQos, LatencyBudget>(cls, "latency_budget", "LatencyBudget");
    add_qos_property<DataWriterQos, Liveliness>(cls, "liveliness", "Liveliness");
    add_qos_property<DataWriterQos, Reliability>(cls, "reliability", "Reliability");
    add_qos_property<DataWriterQos, DestinationOrder>(cls, "destination_order", "DestinationOrder");
    add_qos_property<DataWriterQos, History>(cls, "history", "History");
    add_qos_property<DataWriterQos, ResourceLimits>(cls, "resource_limits", "ResourceLimits");
    add_qos_property<DataWriterQos, TransportPriority>(cls, "transport_priority", "TransportPriority");
    add_qos_property<DataWriterQos, Lifespan>(cls, "lifespan", "Lifespan");
    add_qos_property<DataWriterQos, UserData>(cls, "user_data", "UserData");
    add_qos_property<DataWriterQos, Ownership>(cls, "ownership", "Ownership");
    add_qos_property<DataWriterQos, OwnershipStrength>(cls, "ownership_strength", "OwnershipStrength");
    add_qos_property<DataWriterQos, WriterDataLifecycle>(cls, "writer_data_lifecycle", "WriterDataLifecycle");
    add_qos_property<DataWriterQos, DataWriterResourceLimits>(cls, "data_writer_resource_limits", "DataWriterResourceLimits");
    add_qos_property<DataWriterQos, DataWriterProtocol>(cls, "data_writer_protocol", "DataWriterProtocol");
    add_qos_property<DataWriterQos, TransportSelection>(cls, "transport_selection", "TransportSelection");
    add_qos_property<DataWriterQos, TransportUnicast>(cls, "transport_unicast", "TransportUnicast");
    add_qos_property<DataWriterQos, PublishMode>(cls, "publish_mode", "PublishMode");
    add_qos_property<DataWriterQos, Property>(cls, "property", "Property");
    add_qos_property<DataWriterQos, Service>(cls, "service", "Service");
    add_qos_property<DataWriterQos, Batch>(cls, "batch", "Batch");
    add_qos_property<DataWriterQos, MultiChannel>(cls, "multi_channel", "MultiChannel");
    add_qos_property<DataWriterQos, Availability>(cls, "availability", "Availability");
    add_qos_property<DataWriterQos, EntityName>(cls, "entity_name", "EntityName");
    add_qos_property<DataWriterQos, TopicQueryDispatch>(cls, "topic_query_dispatch", "TopicQueryDispatch");
    add_qos_property<DataWriterQos, TypeSupport>(cls, "type_support", "TypeSupport");
#if rti_connext_version_gte(6, 0, 0)
    add_qos_property<DataWriterQos, DataRepresentation>(cls, "data_representation", "DataRepresentation");
    add_qos_property<DataWriterQos, DataTag>(cls, "data_tag", "DataTag");
    add_qos_property<DataWriterQos, DataWriterTransferMode>(cls, "data_writer_transfer_mode", "DataWriterTransferMode");
#endif
}

template<>
void process_inits<DataWriterQos>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<DataWriterQos>(m, "DataWriterQos");
        }
    ); 
}

}
