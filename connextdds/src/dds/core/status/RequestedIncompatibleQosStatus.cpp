#include "PyConnext.hpp"
#include <dds/core/status/Status.hpp>

using namespace dds::core::status;

template<>
void pyrti::init_class_defs(py::class_<RequestedIncompatibleQosStatus>& cls) {
    cls
        .def(
            "total_count",
            &RequestedIncompatibleQosStatus::total_count,
            "Total count of how many times the concerned DataReader discovered a DataWriter for the same Topic with an offered QoS that is incompatible with that requested by the DataReader."
        )
        .def(
            "total_count_change",
            &RequestedIncompatibleQosStatus::total_count_change,
            "The delta in total_count since the last time the listener was called or the status was read."
        )
        .def(
            "last_policy_id",
            &RequestedIncompatibleQosStatus::last_policy_id,
            "The policy ID of one of the policies that was found to be incompatible the last time an incompatibility was detected. "
        )
        .def_property_readonly(
            "policies",
            &RequestedIncompatibleQosStatus::policies,
            "A list containing, for each policy, the total number of times that the concerned DataReader discovered a DataWriter for the same Topic with an offered QoS that is incompatible with that requested by the DataReader."
        );
}

template<>
void pyrti::process_inits<RequestedIncompatibleQosStatus>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<RequestedIncompatibleQosStatus>(m, "RequestedIncompatibleQosStatus");
        }
    ); 
}