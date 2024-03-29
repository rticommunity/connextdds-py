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
void init_class_defs(py::class_<SubscriptionMatchedStatus>& cls)
{
    cls.def_property_readonly(
               "total_count",
               &SubscriptionMatchedStatus::total_count,
               "Total count of times the DataReader matched with a DataWriter.")
            .def_property_readonly(
                    "total_count_change",
                    &SubscriptionMatchedStatus::total_count_change,
                    "The delta in the total count since the last time the "
                    "listener "
                    "fired or the status was read.")
            .def_property_readonly(
                    "current_count",
                    &SubscriptionMatchedStatus::current_count,
                    "The number of DataWriters that are currently matched with "
                    "this "
                    "DataReader.")
            .def_property_readonly(
                    "current_count_change",
                    &SubscriptionMatchedStatus::current_count_change,
                    "The delta for the current count since the last time the "
                    "listener "
                    "fired or the status was read.")
            .def_property_readonly(
                    "last_publication_handle",
                    &SubscriptionMatchedStatus::last_publication_handle,
                    "A handle to the DataWriter that caused the most recent "
                    "change to "
                    "to this status.")
            .def_property_readonly(
                    "current_count_peak",
                    [](SubscriptionMatchedStatus& status) {
                        return status->current_count_peak();
                    },
                    "The highest value that the current count has reached.");
}

template<>
void process_inits<SubscriptionMatchedStatus>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<SubscriptionMatchedStatus>(
                m,
                "SubscriptionMatchedStatus");
    });
}

}  // namespace pyrti
