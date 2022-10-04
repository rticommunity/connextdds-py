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
void init_class_defs(py::class_<SampleRejectedStatus>& cls)
{
    cls.def_property_readonly(
               "total_count",
               &SampleRejectedStatus::total_count,
               "Total count of samples rejected by the DataReader.")
            .def_property_readonly(
                    "total_count_change",
                    &SampleRejectedStatus::total_count_change,
                    "The delta number of samples rejected since the last time "
                    "the "
                    "listener fired or the status was read.")
            .def_property_readonly(
                    "last_reason",
                    &SampleRejectedStatus::last_reason,
                    "Reason for the DataReader's most recent sample rejection.")
            .def_property_readonly(
                    "last_instance_handle",
                    &SampleRejectedStatus::last_instance_handle,
                    "Handle for the instance of the sample that was most "
                    "recently "
                    "rejected.");
}

template<>
void process_inits<SampleRejectedStatus>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<SampleRejectedStatus>(m, "SampleRejectedStatus");
    });
}

}  // namespace pyrti
