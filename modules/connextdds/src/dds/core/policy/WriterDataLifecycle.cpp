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
void init_class_defs(py::class_<WriterDataLifecycle>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<bool>(),
                 py::arg("autodispose"),
                 "Creates an instance with a value for auto-dispose "
                 "unregistered "
                 "instances and default values for the rest of parameters.")
            .def_property(
                    "autodispose_unregistered_instances",
                    (bool (WriterDataLifecycle::*)() const)
                            & WriterDataLifecycle::
                                    autodispose_unregistered_instances,
                    (WriterDataLifecycle & (WriterDataLifecycle::*) (bool) )
                            & WriterDataLifecycle::
                                    autodispose_unregistered_instances,
                    "Indicates whether the DataWriter should automatically "
                    "dispose "
                    "an instance when it unregisters it.")
            .def_property(
                    "autopurge_unregistered_instances_delay",
                    [](const WriterDataLifecycle& wdl) {
                        return wdl->autopurge_unregistered_instances_delay();
                    },
                    [](WriterDataLifecycle& wdl, const dds::core::Duration& d) {
                        wdl->autopurge_unregistered_instances_delay(d);
                    },
                    "Maximum duration for which the DataWriter will maintain "
                    "information regarding an instance once it has "
                    "unregistered the "
                    "instance."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property(
                    "autopurge_disposed_instances_delay",
                    [](const WriterDataLifecycle& wdl) {
                        return wdl->autopurge_disposed_instances_delay();
                    },
                    [](WriterDataLifecycle& wdl, const dds::core::Duration& d) {
                        wdl->autopurge_disposed_instances_delay(d);
                    },
                    "Maximum duration for which the DataWriter will maintain "
                    "information regarding an instance once it has disposed "
                    "the "
                    "instance."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_static(
                    "auto_dispose_unregistered_instances",
                    &WriterDataLifecycle::AutoDisposeUnregisteredInstances,
                    "Creates WriterDataLifecycle(true).")
            .def_static(
                    "manually_dispose_unregistered_instances",
                    &WriterDataLifecycle::ManuallyDisposeUnregisteredInstances,
                    "Creates WriterDataLifecycle(false).")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<WriterDataLifecycle>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<WriterDataLifecycle>(m, "WriterDataLifecycle");
    });
}

}  // namespace pyrti
