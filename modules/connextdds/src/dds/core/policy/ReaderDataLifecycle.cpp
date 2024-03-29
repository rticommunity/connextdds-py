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
void init_class_defs(py::class_<ReaderDataLifecycle>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<
                         const dds::core::Duration&,
                         const dds::core::Duration&>(),
                 py::arg("nowriter_delay"),
                 py::arg("disposed_samples_delay"),
                 "Creates an instance with the specified nowriter and "
                 "disposed-samples purge delays.")
            .def_property(
                    "autopurge_nowriter_samples_delay",
                    (dds::core::Duration(ReaderDataLifecycle::*)() const)
                            & ReaderDataLifecycle::
                                    autopurge_nowriter_samples_delay,
                    (ReaderDataLifecycle
                     & (ReaderDataLifecycle::*) (const dds::core::Duration&) )
                            & ReaderDataLifecycle::
                                    autopurge_nowriter_samples_delay,
                    "Minimum duration for which the DataReader will maintain "
                    "information regarding an instance once its instance_state "
                    "becomes InstanceStateKind.NOT_ALIVE_NO_WRITERS."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property(
                    "autopurge_disposed_samples_delay",
                    (dds::core::Duration(ReaderDataLifecycle::*)() const)
                            & ReaderDataLifecycle::
                                    autopurge_disposed_samples_delay,
                    (ReaderDataLifecycle
                     & (ReaderDataLifecycle::*) (const dds::core::Duration&) )
                            & ReaderDataLifecycle::
                                    autopurge_disposed_samples_delay,
                    "Minimum duration for which the DataReader will maintain "
                    "information regarding an instance once its instance_state "
                    "becomes InstanceStateKind.NOT_ALIVE_DISPOSED."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property(
                    "autopurge_disposed_instances_delay",
                    [](const ReaderDataLifecycle& rdl) {
                        return rdl->autopurge_disposed_instances_delay();
                    },
                    [](ReaderDataLifecycle& rdl, const dds::core::Duration& d) {
                        rdl->autopurge_disposed_instances_delay(d);
                    },
                    "Minimum duration for which the DataReader will maintain "
                    "an "
                    "instance once its instance_state becomes "
                    "InstanceStateKind.NOT_ALIVE_DISPOSED."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property(
                    "autopurge_nowriter_instances_delay",
                    [](const ReaderDataLifecycle& rdl) {
                        return rdl->autopurge_nowriter_instances_delay();
                    },
                    [](ReaderDataLifecycle& rdl, const dds::core::Duration& d) {
                        rdl->autopurge_nowriter_instances_delay(d);
                    },
                    "Minimum duration for which the DDSDataReader will " 
                    "maintain information about a received instance once its "
                    "instance_state becomes "
                    "DDS_NOT_ALIVE_NO_WRITERS_INSTANCE_STATE and there are no "
                    "samples for the instance in the DataReader queue.")
            .def_property_readonly_static(
                    "no_auto_purge",
                    [](py::object&) {
                        return ReaderDataLifecycle::NoAutoPurge();
                    },
                    "Returns a policy where all purge delays are disabled (the "
                    "default)")
            .def_static(
                    "auto_purge_disposed_samples",
                    &ReaderDataLifecycle::AutoPurgeDisposedSamples,
                    py::arg("delay"),
                    "Returns a policy where only the disposed-samples purge "
                    "delay is "
                    "enabled with a specified duration.")
            .def_static(
                    "auto_purge_no_writer_samples",
                    &ReaderDataLifecycle::AutoPurgeNoWriterSamples,
                    py::arg("delay"),
                    "Returns a policy where only the no-writer purge delay is "
                    "enabled "
                    "with a specified duration.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<ReaderDataLifecycle>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<ReaderDataLifecycle>(m, "ReaderDataLifecycle");
    });
}

}  // namespace pyrti