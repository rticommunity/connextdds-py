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
void init_class_defs(py::class_<LivelinessLostStatus>& cls)
{
    cls.def_property_readonly(
               "total_count",
               &LivelinessLostStatus::total_count,
               "Total count of times that a previously alive DataWriter became "
               "not alive due to a failure to assert its liveliness signal "
               "within the agreed lease duration.")
            .def_property_readonly(
                    "total_count_change",
                    &LivelinessLostStatus::total_count_change,
                    "The delta number of times liveliness has been lost since "
                    "the "
                    "last time the listener callback fired or this status was "
                    "checked.");
}

template<>
void process_inits<LivelinessLostStatus>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<LivelinessLostStatus>(m, "LivelinessLostStatus");
    });
}

}  // namespace pyrti
