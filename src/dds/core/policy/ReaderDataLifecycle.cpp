#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

template<>
void pyrti::init_class_defs(py::class_<ReaderDataLifecycle>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates the default policy."
        )
        .def(
            py::init<
                const dds::core::Duration&,
                const dds::core::Duration&
            >(),
            py::arg("nowriter_delay"),
            py::arg("disposed_samples_delay"),
            "Creates an instance with the specified nowriter and "
            "disposed-samples purge delays."
        )
        .def_property(
            "autopurge_unregistered_instances_delay",
            (dds::core::Duration (ReaderDataLifecycle::*)() const) &ReaderDataLifecycle::autopurge_nowriter_samples_delay,
            (ReaderDataLifecycle& (ReaderDataLifecycle::*)(const dds::core::Duration&)) &ReaderDataLifecycle::autopurge_nowriter_samples_delay,
            "Minimum duration for which the DataReader will maintain "
            "information regarding an instance once its instance_state "
            "becomes InstanceStateKind.NOT_ALIVE_NO_WRITERS."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "autopurge_disposed_samples_delay",
            (dds::core::Duration (ReaderDataLifecycle::*)() const) &ReaderDataLifecycle::autopurge_disposed_samples_delay,
            (ReaderDataLifecycle& (ReaderDataLifecycle::*)(const dds::core::Duration&)) &ReaderDataLifecycle::autopurge_disposed_samples_delay,
            "Minimum duration for which the DataReader will maintain "
            "information regarding an instance once its instance_state "
            "becomes InstanceStateKind.NOT_ALIVE_DISPOSED."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property(
            "autopurge_disposed_instances_delay",
            [](const ReaderDataLifecycle& rdl) {
                return rdl->autopurge_disposed_instances_delay();
            },
            [](ReaderDataLifecycle& rdl, const dds::core::Duration& d) {
                rdl->autopurge_disposed_instances_delay(d);
            },
            "Minimum duration for which the DataReader will maintain an "
            "instance once its instance_state becomes "
            "InstanceStateKind.NOT_ALIVE_DISPOSED."
            "\n\n"
            "This property uses value semantics."
        )
        .def_static(
            "no_auto_purge",
            &ReaderDataLifecycle::NoAutoPurge,
            "Returns a policy where all purge delays are disabled (the "
            "default)"
        )
        .def_static(
            "auto_purge_disposed_samples",
            &ReaderDataLifecycle::AutoPurgeDisposedSamples,
            "Returns a policy where only the disposed-samples purge delay is "
            "enabled with a specified duration."
        )
        .def_static(
            "auto_purge_no_writer_samples",
            &ReaderDataLifecycle::AutoPurgeNoWriterSamples,
            "Returns a policy where only the no-writer purge delay is enabled "
            "with a specified duration."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

template<>
void pyrti::process_inits<ReaderDataLifecycle>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<ReaderDataLifecycle>(m, "ReaderDataLifecycle");
        }
    );
}