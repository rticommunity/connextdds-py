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
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<SystemResourceLimits>& cls)
{
    cls.def(py::init<>(),
            "Creates a SystemResourceLimits qos policy with default values.")
            .def(py::init<int32_t>(),
                 py::arg("max_objects_per_thread"),
                 "Creates a SystemResourceLimits qos policy with the provided "
                 "max_objects_per_thread.")
            .def_property(
                    "max_objects_per_thread",
                    (int32_t(SystemResourceLimits::*)() const)
                            & SystemResourceLimits::max_objects_per_thread,
                    (SystemResourceLimits & (SystemResourceLimits::*) (int32_t))
                            & SystemResourceLimits::max_objects_per_thread,
                    "The maximum number of objects that can be stored per "
                    "thread.")
            .def_property(
                    "initial_objects_per_thread",
                    (int32_t(SystemResourceLimits::*)() const)
                            & SystemResourceLimits::initial_objects_per_thread,
                    (SystemResourceLimits & (SystemResourceLimits::*) (int32_t))
                            & SystemResourceLimits::initial_objects_per_thread,
                    "The number of objects per thread for a "
                    "DomainParticipantFactory for which infrastructure will "
                    "initially be allocated.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<SystemResourceLimits>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<SystemResourceLimits>(m, "SystemResourceLimits");
    });
}

}  // namespace pyrti
