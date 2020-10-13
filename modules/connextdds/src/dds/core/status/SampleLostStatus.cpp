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
void init_class_defs(py::class_<SampleLostStatus>& cls)
{
    cls.def_property_readonly(
               "total_count",
               &SampleLostStatus::total_count,
               "Total count of all samples lost across all instances of data "
               "published under the Topic.")
            .def_property_readonly(
                    "total_count_change",
                    &SampleLostStatus::total_count_change,
                    "The incremental number of samples lost since the last "
                    "time the listener was called or the status was read.")
            .def_property_readonly(
                    "last_reason",
                    [](const SampleLostStatus& sls) {
                        return sls->last_reason();
                    },
                    "The reason for the most recent sample loss.");
}

template<>
void process_inits<SampleLostStatus>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<SampleLostStatus>(m, "SampleLostStatus");
    });
}

}  // namespace pyrti
