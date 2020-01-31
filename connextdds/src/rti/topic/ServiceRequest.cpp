#include "PyConnext.hpp"
#include <dds/topic/BuiltinTopic.hpp>
#include "PySafeEnum.hpp"

using namespace rti::topic;

namespace pyrti {

template<>
void init_class_defs(py::class_<ServiceRequest>& cls) {
    cls
        .def_property_readonly(
            "service_id",
            &ServiceRequest::service_id,
            "The service id of the request."
        )
        .def_property_readonly(
            "instance_id",
            &ServiceRequest::instance_id,
            "Get the instance id of the request."
        )
        .def_property_readonly(
            "request_body",
            &ServiceRequest::request_body,
            "Get the request body of the request."
        )
        .def_property_readonly_static(
            "topic_name",
            [](py::object&) {
                return rti::topic::service_request_topic_name();
            },
            "ServiceRequest built-in topic name."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

template<>
void process_inits<ServiceRequest>(py::module& m, ClassInitList& l) {
    auto sri = init_dds_safe_enum<rti::core::ServiceRequestId_def>(m, "ServiceRequestId");

    py::enum_<rti::core::ServiceRequestId::type>(sri, "ServiceRequestId")
        .value(
            "UNKNOWN",
            rti::core::ServiceRequestId::type::UNKNOWN,
            "An unknown service."
        )
        .value(
            "TOPIC_QUERY",
            rti::core::ServiceRequestId::type::TOPIC_QUERY,
            "The topic query service."
        )
        .value(
            "LOCATOR_REACHABILITY",
            rti::core::ServiceRequestId::type::LOCATOR_REACHABILITY,
            "The locator reachability service."
        )
        .export_values();

    l.push_back(
        [m]() mutable {
            return init_class<ServiceRequest>(m, "ServiceRequest");
        }
    );
}

}
