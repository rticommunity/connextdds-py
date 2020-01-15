#include "PyConnext.hpp"
#include <dds/sub/qos/DataReaderQos.hpp>
#include "PyQos.hpp"

using namespace dds::sub::qos;
using namespace dds::core::policy;
using namespace rti::core::policy;

template<>
void pyrti::init_class_defs(py::class_<DataReaderQos>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a DataReaderQos with the default value for each policy."
        );
    pyrti::add_qos_property<DataReaderQos, Durability>(cls, "durability", "Durability");
    pyrti::add_qos_property<DataReaderQos, Deadline>(cls, "deadline", "Deadline");
    pyrti::add_qos_property<DataReaderQos, LatencyBudget>(cls, "latency_budget", "LatencyBudget");
    pyrti::add_qos_property<DataReaderQos, Liveliness>(cls, "liveliness", "Liveliness");
    pyrti::add_qos_property<DataReaderQos, Reliability>(cls, "reliability", "Reliability");
    pyrti::add_qos_property<DataReaderQos, DestinationOrder>(cls, "destination_order", "DestinationOrder");
    pyrti::add_qos_property<DataReaderQos, History>(cls, "history", "History");
    pyrti::add_qos_property<DataReaderQos, ResourceLimits>(cls, "resource_limits", "ResourceLimits");
    pyrti::add_qos_property<DataReaderQos, UserData>(cls, "user_data", "UserData");
    pyrti::add_qos_property<DataReaderQos, Ownership>(cls, "ownership", "Ownership");
    pyrti::add_qos_property<DataReaderQos, TimeBasedFilter>(cls, "time_based_filter", "TimeBasedFilter");
    pyrti::add_qos_property<DataReaderQos, ReaderDataLifecycle>(cls, "reader_data_lifecycle", "ReaderDataLifecycle");
    pyrti::add_qos_property<DataReaderQos, DataRepresentation>(cls, "data_representation", "DataRepresentation");
    pyrti::add_qos_property<DataReaderQos, TransportPriority>(cls, "transport_priority", "TransportPriority");
    pyrti::add_qos_property<DataReaderQos, TypeConsistencyEnforcement>(cls, "type_consistency_enforcement", "TypeConsistencyEnforcement");
    pyrti::add_qos_property<DataReaderQos, DataTag>(cls, "data_tag", "DataTag");
    pyrti::add_qos_property<DataReaderQos, DataReaderResourceLimits>(cls, "data_reader_resource_limits", "DataReaderResourceLimits");
    pyrti::add_qos_property<DataReaderQos, DataReaderProtocol>(cls, "data_reader_protocol", "DataReaderProtocol");
    pyrti::add_qos_property<DataReaderQos, TransportSelection>(cls, "transport_selection", "TransportSelection");
    pyrti::add_qos_property<DataReaderQos, TransportUnicast>(cls, "transport_unicast", "TransportUnicast");
    pyrti::add_qos_property<DataReaderQos, TransportMulticast>(cls, "transport_multicast", "TransportMulticast");
    pyrti::add_qos_property<DataReaderQos, Property>(cls, "property", "Property");
    pyrti::add_qos_property<DataReaderQos, Service>(cls, "service", "Service");
    pyrti::add_qos_property<DataReaderQos, Availability>(cls, "availability", "Availability");
    pyrti::add_qos_property<DataReaderQos, EntityName>(cls, "entity_name", "EntityName");
    pyrti::add_qos_property<DataReaderQos, TypeSupport>(cls, "type_support", "TypeSupport");
}

template<>
void pyrti::process_inits<DataReaderQos>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<DataReaderQos>(m, "DataReaderQos");
        }
    ); 
}
