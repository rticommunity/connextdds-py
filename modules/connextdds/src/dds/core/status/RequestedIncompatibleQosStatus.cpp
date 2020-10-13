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
#include <dds/core/status/Status.hpp>

using namespace dds::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<RequestedIncompatibleQosStatus>& cls)
{
    cls.def("total_count",
            &RequestedIncompatibleQosStatus::total_count,
            "Total count of how many times the concerned DataReader discovered "
            "a DataWriter for the same Topic with an offered QoS that is "
            "incompatible with that requested by the DataReader.")
            .def("total_count_change",
                 &RequestedIncompatibleQosStatus::total_count_change,
                 "The delta in total_count since the last time the listener "
                 "was called or the status was read.")
            .def("last_policy_id",
                 &RequestedIncompatibleQosStatus::last_policy_id,
                 "The policy ID of one of the policies that was found to be "
                 "incompatible the last time an incompatibility was detected. ")
            .def_property_readonly(
                    "policies",
                    &RequestedIncompatibleQosStatus::policies,
                    "A list containing, for each policy, the total number of "
                    "times that the concerned DataReader discovered a "
                    "DataWriter for the same Topic with an offered QoS that is "
                    "incompatible with that requested by the DataReader.");
}

template<>
void process_inits<RequestedIncompatibleQosStatus>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<RequestedIncompatibleQosStatus>(
                m,
                "RequestedIncompatibleQosStatus");
    });
}

}  // namespace pyrti
