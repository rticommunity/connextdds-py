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
#include <dds/sub/qos/DataReaderQos.hpp>
#include "PyQos.hpp"

using namespace dds::sub::qos;
using namespace dds::core::policy;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<DataReaderQos>& cls)
{
    cls.def(py::init<>(),
            "Create a DataReaderQos with the default value for each policy.")
            .def(py::init([](const PyIAnyDataReader& dr) {
                     return dr.py_qos();
                 }),
                 py::arg("reader"),
                 "Create a DataReaderQos with settings equivalent to those "
                 "of the provided DataReader.")
            .def(py::self == py::self, "Test for equality")
            .def(py::self != py::self, "Test for inequality.");

    add_qos_property<DataReaderQos, Durability>(
            cls,
            "durability",
            "Durability");
    add_qos_property<DataReaderQos, Deadline>(cls, "deadline", "Deadline");
    add_qos_property<DataReaderQos, LatencyBudget>(
            cls,
            "latency_budget",
            "LatencyBudget");
    add_qos_property<DataReaderQos, Liveliness>(
            cls,
            "liveliness",
            "Liveliness");
    add_qos_property<DataReaderQos, Reliability>(
            cls,
            "reliability",
            "Reliability");
    add_qos_property<DataReaderQos, DestinationOrder>(
            cls,
            "destination_order",
            "DestinationOrder");
    add_qos_property<DataReaderQos, History>(cls, "history", "History");
    add_qos_property<DataReaderQos, ResourceLimits>(
            cls,
            "resource_limits",
            "ResourceLimits");
    add_qos_property<DataReaderQos, UserData>(cls, "user_data", "UserData");
    add_qos_property<DataReaderQos, Ownership>(cls, "ownership", "Ownership");
    add_qos_property<DataReaderQos, TimeBasedFilter>(
            cls,
            "time_based_filter",
            "TimeBasedFilter");
    add_qos_property<DataReaderQos, ReaderDataLifecycle>(
            cls,
            "reader_data_lifecycle",
            "ReaderDataLifecycle");
    add_qos_property<DataReaderQos, TransportPriority>(
            cls,
            "transport_priority",
            "TransportPriority");
    add_qos_property<DataReaderQos, TypeConsistencyEnforcement>(
            cls,
            "type_consistency_enforcement",
            "TypeConsistencyEnforcement");
    add_qos_property<DataReaderQos, DataReaderResourceLimits>(
            cls,
            "data_reader_resource_limits",
            "DataReaderResourceLimits");
    add_qos_property<DataReaderQos, DataReaderProtocol>(
            cls,
            "data_reader_protocol",
            "DataReaderProtocol");
    add_qos_property<DataReaderQos, TransportSelection>(
            cls,
            "transport_selection",
            "TransportSelection");
    add_qos_property<DataReaderQos, TransportUnicast>(
            cls,
            "transport_unicast",
            "TransportUnicast");
    add_qos_property<DataReaderQos, TransportMulticast>(
            cls,
            "transport_multicast",
            "TransportMulticast");
    add_qos_property<DataReaderQos, Property>(cls, "property", "Property");
    add_qos_property<DataReaderQos, Service>(cls, "service", "Service");
    add_qos_property<DataReaderQos, Availability>(
            cls,
            "availability",
            "Availability");
    add_qos_property<DataReaderQos, EntityName>(
            cls,
            "entity_name",
            "EntityName");
    add_qos_property<DataReaderQos, TypeSupport>(
            cls,
            "type_support",
            "TypeSupport");
#if rti_connext_version_gte(6, 0, 0)
    add_qos_property<DataReaderQos, DataRepresentation>(
            cls,
            "data_representation",
            "DataRepresentation");
    add_qos_property<DataReaderQos, DataTag>(cls, "data_tag", "DataTag");
#endif
#if rti_connext_version_gte(6, 1, 0)
    add_qos_string_conversions(cls);
#endif
}

template<>
void process_inits<DataReaderQos>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DataReaderQos>(m, "DataReaderQos");
    });
}

}  // namespace pyrti
