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
void init_class_defs(py::class_<LivelinessChangedStatus>& cls)
{
    cls.def_property_readonly(
               "alive_count",
               &LivelinessChangedStatus::alive_count,
               "The number of currently alive DataWriters that write to the "
               "Topic of the DataReader.")
            .def_property_readonly(
                    "not_alive_count",
                    &LivelinessChangedStatus::not_alive_count,
                    "The number of currently NOT_ALIVE DataWriters that write "
                    "to "
                    "the Topic of the DataReader.")
            .def_property_readonly(
                    "alive_count_change",
                    &LivelinessChangedStatus::alive_count_change,
                    "The delta in the alive count since the last time the "
                    "listener "
                    "fired or the status was read.")
            .def_property_readonly(
                    "not_alive_count_change",
                    &LivelinessChangedStatus::not_alive_count_change,
                    "The delta in the NOT_ALIVE count since the last time the "
                    "listener fired or the status was read.")
            .def_property_readonly(
                    "last_publication_handle",
                    &LivelinessChangedStatus::last_publication_handle,
                    "The instance handle of the DataWriter with the most "
                    "recent "
                    "change in liveliness.");
}

template<>
void process_inits<LivelinessChangedStatus>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<LivelinessChangedStatus>(
                m,
                "LivelinessChangedStatus");
    });
}

}  // namespace pyrti
