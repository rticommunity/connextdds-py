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
#include "PyQosPolicy.hpp"

using namespace dds::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<OfferedIncompatibleQosStatus>& cls)
{
    cls.def_property_readonly(
               "total_count",
               &OfferedIncompatibleQosStatus::total_count,
               "Total number of times the concerned DataWriter discovered a "
               "DataReader for the same Topic, common partition with a "
               "requested QoS that is incompatible with that offered by the "
               "DataWriter.")
            .def_property_readonly(
                    "total_count_change",
                    &OfferedIncompatibleQosStatus::total_count_change,
                    "The delta in total_count since the last time the listener "
                    "was called or the status was read.")
            .def_property_readonly(
                    "last_policy",
                    [](OfferedIncompatibleQosStatus& self) {
                        auto m = py::module::import("rti.connextdds");
                        return get_policy_type_from_id(m, self.last_policy_id());
                    },
                    "The policy class of one of the policies that was found to "
                    "be incompatible the last time an incompatibility was "
                    "detected.")
            .def_property_readonly(
                    "policies",
                    &OfferedIncompatibleQosStatus::policies,
                    "A list containing for each policy the total number of "
                    "times that the concerned DataWriter discovered a "
                    "DataReader for the same Topic and common partition with a "
                    "requested QoS that is incompatible with that offered by "
                    "the DataWriter.");
}

template<>
void process_inits<OfferedIncompatibleQosStatus>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<OfferedIncompatibleQosStatus>(
                m,
                "OfferedIncompatibleQosStatus");
    });
}

}  // namespace pyrti
