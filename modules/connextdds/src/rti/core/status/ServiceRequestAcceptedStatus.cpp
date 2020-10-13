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
#include <rti/core/status/Status.hpp>

using namespace rti::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<ServiceRequestAcceptedStatus>& cls)
{
    cls.def_property_readonly(
               "total_count",
               &ServiceRequestAcceptedStatus::total_count,
               "The total number of ServiceRequests that have been accepted by "
               "this DataWriter.")
            .def_property_readonly(
                    "current_count",
                    &ServiceRequestAcceptedStatus::current_count,
                    "The current number of ServiceRequests that have been "
                    "accepted by "
                    "this DataWriter.")
            .def_property_readonly(
                    "last_request_handle",
                    &ServiceRequestAcceptedStatus::last_request_handle,
                    "A handle to the last ServiceRequest that caused the "
                    "DataWriter's "
                    "status to change.")
            .def_property_readonly(
                    "service_id",
                    &ServiceRequestAcceptedStatus::service_id,
                    "ID of the service to which the accepted Request belongs.")
            .doc() =
            "Information about the service_request_accepted status\n"
            "Currently, the only service that causes the "
            "ServiceRequestAcceptedStatus to be triggered is "
            "TopicQuery service. A ServiceRequest is accepted when a "
            "DataWriter matches with a DataReader that has created a "
            "TopicQuery.\n"
            "This status is also changed (and the listener, if any, "
            "called) when a ServiceRequest has been cancelled, or "
            "deleted. This will happen when a DataReader deletes a "
            "TopicQuery using TopicQuery.close().";
}

template<>
void process_inits<ServiceRequestAcceptedStatus>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<ServiceRequestAcceptedStatus>(
                m,
                "ServiceRequestAcceptedStatus");
    });
}

}  // namespace pyrti
