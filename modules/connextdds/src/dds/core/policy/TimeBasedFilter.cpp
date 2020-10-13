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
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<TimeBasedFilter>& cls)
{
    cls.def(py::init<>(), "Creates the default time-based filter.")
            .def(py::init<const dds::core::Duration&>(),
                 py::arg("min_separation"),
                 "Creates a policy with the specified minimum separation.")
            .def_property(
                    "minimum_separation",
                    (const dds::core::Duration (TimeBasedFilter::*)() const)
                            & TimeBasedFilter::minimum_separation,
                    (TimeBasedFilter
                     & (TimeBasedFilter::*) (const dds::core::Duration&) )
                            & TimeBasedFilter::minimum_separation,
                    "The minimum separation between subsequent samples.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<TimeBasedFilter>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<TimeBasedFilter>(m, "TimeBasedFilter");
    });
}

}  // namespace pyrti
