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
void init_class_defs(py::class_<OfferedDeadlineMissedStatus>& cls)
{
    cls.def_property_readonly(
               "total_count",
               &OfferedDeadlineMissedStatus::total_count,
               "The count of a DataWriter's failures to meet write deadlines.")
            .def_property_readonly(
                    "total_count_change",
                    &OfferedDeadlineMissedStatus::total_count_change,
                    "The delta in total_count since the last time the listener "
                    "was called or the status was read.")
            .def_property_readonly(
                    "last_instance_handle",
                    &OfferedDeadlineMissedStatus::last_instance_handle,
                    "Handle to the last instance in the DataWriter for which "
                    "an offered deadline was missed.");
}

template<>
void process_inits<OfferedDeadlineMissedStatus>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<OfferedDeadlineMissedStatus>(
                m,
                "OfferedDeadlineMissedStatus");
    });
}

}  // namespace pyrti
