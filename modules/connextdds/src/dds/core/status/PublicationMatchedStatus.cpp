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
void init_class_defs(py::class_<PublicationMatchedStatus>& cls)
{
    cls.def_property_readonly(
               "total_count",
               &PublicationMatchedStatus::total_count,
               "Total count of times the DataWriter matched with a DataReader.")
            .def_property_readonly(
                    "total_count_change",
                    &PublicationMatchedStatus::total_count_change,
                    "The delta in the total count since the last time the "
                    "listener "
                    "fired or the status was read.")
            .def_property_readonly(
                    "current_count",
                    &PublicationMatchedStatus::current_count,
                    "The number of DataReaders that are currently matched with "
                    "this "
                    "DataWriter.")
            .def_property_readonly(
                    "current_count_change",
                    &PublicationMatchedStatus::current_count_change,
                    "The delta for the current count since the last time the "
                    "listener "
                    "fired or the status was read.")
            .def_property_readonly(
                    "last_subscription_handle",
                    &PublicationMatchedStatus::last_subscription_handle,
                    "A handle to the DataReader that caused the most recent "
                    "change to "
                    "to this status.")
            .def_property_readonly(
                    "current_count_peak",
                    [](PublicationMatchedStatus& status) {
                        return status->current_count_peak();
                    },
                    "The highest value that the current count has reached.");
}

template<>
void process_inits<PublicationMatchedStatus>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<PublicationMatchedStatus>(
                m,
                "PublicationMatchedStatus");
    });
}

}  // namespace pyrti
