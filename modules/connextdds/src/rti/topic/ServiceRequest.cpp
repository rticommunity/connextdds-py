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
#include <dds/topic/BuiltinTopic.hpp>
#include "PySafeEnum.hpp"
#include "PySeq.hpp"
#include "PyInitType.hpp"
#include "PyInitOpaqueTypeContainers.hpp"

INIT_OPAQUE_TYPE_CONTAINERS(rti::topic::ServiceRequest);

using namespace rti::topic;

namespace pyrti {

template<>
void init_dds_typed_topic_template(
        py::class_<
                PyTopic<rti::topic::ServiceRequest>,
                PyITopicDescription<rti::topic::ServiceRequest>,
                PyIAnyTopic>& cls)
{
    init_dds_typed_topic_base_template(cls);
}

template<>
void init_class_defs(py::class_<ServiceRequest>& cls)
{
    cls.def_property_readonly(
               "service_id",
               &ServiceRequest::service_id,
               "The service id of the request.")
            .def_property_readonly(
                    "instance_id",
                    &ServiceRequest::instance_id,
                    "Get the instance id of the request.")
            .def_property_readonly(
                    "request_body",
                    &ServiceRequest::request_body,
                    "Get the request body of the request.")
            .def_property_readonly_static(
                    "topic_name",
                    [](py::object&) {
                        return rti::topic::service_request_topic_name();
                    },
                    "ServiceRequest built-in topic name.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<ServiceRequest>(py::module& m, ClassInitList& l)
{
    init_dds_safe_enum<rti::core::ServiceRequestId_def>(
            m,
            "ServiceRequestId",
            [](py::object& o) {
                py::enum_<rti::core::ServiceRequestId::type>(o, "ServiceRequestId")
                        .value("UNKNOWN",
                               rti::core::ServiceRequestId::type::UNKNOWN,
                               "An unknown service.")
                        .value("TOPIC_QUERY",
                               rti::core::ServiceRequestId::type::TOPIC_QUERY,
                               "The topic query service.")
                        .value("LOCATOR_REACHABILITY",
                               rti::core::ServiceRequestId::type::
                                       LOCATOR_REACHABILITY,
                               "The locator reachability service.")
                        .export_values();
            });

    l.push_back([m, &l]() mutable {
        return init_type_class<ServiceRequest>(m, l, "ServiceRequest");
    });
}

}  // namespace pyrti
