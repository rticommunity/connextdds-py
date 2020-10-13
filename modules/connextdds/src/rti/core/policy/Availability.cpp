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
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<Availability>& cls)
{
    cls.def(py::init<>(), "Create a default Availability QoS policy.")
            .def(py::init<
                         bool,
                         const dds::core::Duration&,
                         const dds::core::Duration&,
                         const dds::core::vector<EndpointGroup>&>(),
                 py::arg("enable_required_subscriptions"),
                 py::arg("data_waiting_time"),
                 py::arg("endpoint_waiting_time"),
                 py::arg("required_endpount_groups"),
                 "Create an Availability QoS policy with the provided "
                 "parameters.")
            .def_property(
                    "enable_required_subscriptions",
                    (bool (Availability::*)() const)
                            & Availability::enable_required_subscriptions,
                    (Availability & (Availability::*) (bool) )
                            & Availability::enable_required_subscriptions,
                    "Get/set the status of required subscriptions for this "
                    "policy.")
            .def_property(
                    "max_data_availability_waiting_time",
                    (dds::core::Duration(Availability::*)() const)
                            & Availability::max_data_availability_waiting_time,
                    (Availability
                     & (Availability::*) (const dds::core::Duration&) )
                            & Availability::max_data_availability_waiting_time,
                    "Get/set the amount of time to wait before delivering a "
                    "sample to "
                    "the application without having received some of the "
                    "previous "
                    "samples.")
            .def_property(
                    "max_endpoint_availability_waiting_time",
                    (dds::core::Duration(Availability::*)() const)
                            & Availability::
                                    max_endpoint_availability_waiting_time,
                    (Availability
                     & (Availability::*) (const dds::core::Duration&) )
                            & Availability::
                                    max_endpoint_availability_waiting_time,
                    "Get/set the amount of time to wait to discover "
                    "DataWriters "
                    "providing samples for the same data source (virtual "
                    "GUID).")
            .def_property(
                    "required_matched_endpoint_groups",
                    [](Availability& a) {
                        return std::vector<EndpointGroup>(
                                a.required_matched_endpoint_groups());
                    },
                    [](Availability& a, std::vector<EndpointGroup> egv) {
                        return a.required_matched_endpoint_groups(egv);
                    },
                    "Get/set a copy of the required endpoint groups.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<Availability>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<Availability>(m, "Availability");
    });
}

}  // namespace pyrti
