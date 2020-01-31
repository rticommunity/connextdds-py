#include "PyConnext.hpp"
#include <dds/core/status/Status.hpp>

using namespace dds::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<OfferedIncompatibleQosStatus>& cls) {
    cls
        .def_property_readonly(
            "total_count",
            &OfferedIncompatibleQosStatus::total_count,
            "Total number of times the concerned DataWriter discovered a DataReader for the same Topic, common partition with a requested QoS that is incompatible with that offered by the DataWriter."
        )
        .def_property_readonly(
            "total_count_change",
            &OfferedIncompatibleQosStatus::total_count_change,
            "The delta in total_count since the last time the listener was called or the status was read."
        )
        .def_property_readonly(
            "last_policy_id",
            &OfferedIncompatibleQosStatus::last_policy_id,
            "The policy ID of one of the policies that was found to be incompatible the last time an incompatibility was detected."
        )
        .def_property_readonly(
            "policies",
            &OfferedIncompatibleQosStatus::policies,
            "A list containing for each policy the total number of times that the concerned DataWriter discovered a DataReader for the same Topic and common partition with a requested QoS that is incompatible with that offered by the DataWriter."
        );
}

template<>
void process_inits<OfferedIncompatibleQosStatus>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<OfferedIncompatibleQosStatus>(m, "OfferedIncompatibleQosStatus");
        }
    ); 
}

}
