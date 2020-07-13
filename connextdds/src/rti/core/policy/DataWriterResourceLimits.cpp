#include "PyConnext.hpp"
#include "PySafeEnum.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<DataWriterResourceLimits>& cls)
{
    cls.def(py::init<>(), "Create a default DataWriterResourceLimits policy.")
            .def_property(
                    "initial_concurrent_blocking_threads",
                    (int32_t(DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::
                                    initial_concurrent_blocking_threads,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (int32_t))
                            & DataWriterResourceLimits::
                                    initial_concurrent_blocking_threads,
                    "The initial number of threads that are allowed to "
                    "concurrently "
                    "block on write call on the same DataWriter.")
            .def_property(
                    "max_concurrent_blocking_threads",
                    (int32_t(DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::
                                    max_concurrent_blocking_threads,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (int32_t))
                            & DataWriterResourceLimits::
                                    max_concurrent_blocking_threads,
                    "The maximum number of threads that are allowed to "
                    "concurrently "
                    "block on write call on the same DataWriter.")
            .def_property(
                    "max_remote_reader_filters",
                    (int32_t(DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::
                                    max_remote_reader_filters,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (int32_t))
                            & DataWriterResourceLimits::
                                    max_remote_reader_filters,
                    "The maximum number of remote DataReaders for which the "
                    "DataWriter will perform content-based filtering.")
            .def_property(
                    "initial_batches",
                    (int32_t(DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::initial_batches,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (int32_t))
                            & DataWriterResourceLimits::initial_batches,
                    "Represents the initial number of batches a DataWriter "
                    "will "
                    "manage.")
            .def_property(
                    "max_batches",
                    (int32_t(DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::max_batches,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (int32_t))
                            & DataWriterResourceLimits::max_batches,
                    "Represents the maximum number of batches a DataWriter "
                    "will "
                    "manage.")
            .def_property(
                    "cookie_max_length",
                    (int32_t(DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::cookie_max_length,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (int32_t))
                            & DataWriterResourceLimits::cookie_max_length,
                    "Represents the maximum length in bytes of a Cookie.")
            .def_property(
                    "instance_replacement",
                    (DataWriterResourceLimitsInstanceReplacementKind(
                            DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::instance_replacement,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (DataWriterResourceLimitsInstanceReplacementKind))
                            & DataWriterResourceLimits::instance_replacement,
                    "Sets the kinds of instances allowed to be replaced when "
                    "instance "
                    "resource limits are reached.")
            .def_property(
                    "replace_empty_instances",
                    (bool (DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::replace_empty_instances,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (bool) )
                            & DataWriterResourceLimits::replace_empty_instances,
                    "Whether or not to replace empty instances during instance "
                    "replacement.")
            .def_property(
                    "autoregister_instances",
                    (bool (DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::autoregister_instances,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (bool) )
                            & DataWriterResourceLimits::autoregister_instances,
                    "Whether or not to automatically register new instances.")
            .def_property(
                    "initial_virtual_writers",
                    (int32_t(DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::initial_virtual_writers,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (int32_t))
                            & DataWriterResourceLimits::initial_virtual_writers,
                    "The initial number of virtual writers supported by a "
                    "DataWriter.")
            .def_property(
                    "max_virtual_writers",
                    (int32_t(DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::max_virtual_writers,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (int32_t))
                            & DataWriterResourceLimits::max_virtual_writers,
                    "The maximum number of virtual writers supported by a "
                    "DataWriter.")
            .def_property(
                    "max_remote_readers",
                    (int32_t(DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::max_remote_readers,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (int32_t))
                            & DataWriterResourceLimits::max_remote_readers,
                    "The maximum number of remote readers supported by a "
                    "DataWriter.")
            .def_property(
                    "max_app_ack_remote_readers",
                    (int32_t(DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::
                                    max_app_ack_remote_readers,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (int32_t))
                            & DataWriterResourceLimits::
                                    max_app_ack_remote_readers,
                    "The maximum number of application-level acknowledging "
                    "remote "
                    "readers supported by a DataWriter.")
            .def_property(
                    "initial_active_topic_queries",
                    (int32_t(DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::
                                    initial_active_topic_queries,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (int32_t))
                            & DataWriterResourceLimits::
                                    initial_active_topic_queries,
                    "Represents the initial number of active topic queries a "
                    "DataWriter will manage.")
            .def_property(
                    "max_active_topic_queries",
                    (int32_t(DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::
                                    max_active_topic_queries,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (int32_t))
                            & DataWriterResourceLimits::
                                    max_active_topic_queries,
                    "Represents the maximum number of active topic queries a "
                    "DataWriter will manage.")
#if rti_connext_version_gte(6, 0, 0)
            .def_property(
                    "writer_loaned_sample_allocation",
                    (rti::core::AllocationSettings
                     & (DataWriterResourceLimits::*) ())
                            & DataWriterResourceLimits::
                                    writer_loaned_sample_allocation,
                    [](DataWriterResourceLimits& dwrl,
                       rti::core::AllocationSettings& as) {
                        dwrl.writer_loaned_sample_allocation() = as;
                    },
                    "Represents the allocation settings of loaned samples "
                    "managed by "
                    "a DataWriter.")
            .def_property(
                    "initialize_writer_loaned_sample",
                    (bool (DataWriterResourceLimits::*)() const)
                            & DataWriterResourceLimits::
                                    initialize_writer_loaned_sample,
                    (DataWriterResourceLimits
                     & (DataWriterResourceLimits::*) (bool) )
                            & DataWriterResourceLimits::
                                    initialize_writer_loaned_sample,
                    "Whether or not to initialize loaned samples returned by a "
                    "DataWriter.")
#endif
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<DataWriterResourceLimits>(py::module& m, ClassInitList& l)
{
    init_dds_safe_enum<DataWriterResourceLimitsInstanceReplacementKind_def>(
            m,
            "DataWriterResourceLimitsInstaceReplacementKind",
            [](py::object& o) {
                py::enum_<
                        DataWriterResourceLimitsInstanceReplacementKind::type>(
                        o,
                        "Enum")
                        .value("UNREGISTERED",
                               DataWriterResourceLimitsInstanceReplacementKind::
                                       type::UNREGISTERED,
                               "Allows a DataWriter to reclaim unregistered "
                               "acknowledged "
                               "instances."
                               "\n\n"
                               "By default, all instance replacement kinds "
                               "first attempt to "
                               "reclaim an unregistered, acknowledged "
                               "instance. Used in "
                               "DataWriterResourceLimits.instance_replacement "
                               "[default]")
                        .value("ALIVE",
                               DataWriterResourceLimitsInstanceReplacementKind::
                                       type::ALIVE,
                               "Allows a DataWriter to reclaim alive, "
                               "acknowledged instances."
                               "\n\n"
                               "When an unregistered, acknowledged instance is "
                               "not available to "
                               "reclaim, this kind allows a DataWriter to "
                               "reclaim an alive, "
                               "acknowledged instance, where an alive instance "
                               "is a registered, "
                               "non-disposed instance. The least recently "
                               "registered or written "
                               "alive instance will be reclaimed.")
                        .value("DISPOSED",
                               DataWriterResourceLimitsInstanceReplacementKind::
                                       type::DISPOSED,
                               "Allows a DataWriter to reclaim disposed "
                               "acknowledged instances."
                               "\n\n"
                               "When an unregistered, acknowledged instance is "
                               "not available to "
                               "reclaim, this kind allows a DataWriter to "
                               "reclaim a disposed, "
                               "acknowledged instance. The least recently "
                               "disposed instance will "
                               "be reclaimed.")
                        .value("ALIVE_THEN_DISPOSED",
                               DataWriterResourceLimitsInstanceReplacementKind::
                                       type::ALIVE_THEN_DISPOSED,
                               "Allows a DataWriter first to reclaim an alive, "
                               "acknowledged "
                               "instance, and then, if necessary, a disposed, "
                               "acknowledged "
                               "instance."
                               "\n\n"
                               "When an unregistered, acknowledged instance is "
                               "not available "
                               "to reclaim, this kind allows a DataWriter to "
                               "first try "
                               "reclaiming an alive, acknowledged instance. If "
                               "no instance is "
                               "reclaimable, then it tries reclaiming a "
                               "disposed, acknowledged "
                               "instance. The least recently used (i.e., "
                               "registered, written, or "
                               "disposed) instance will be reclaimed.")
                        .value("DISPOSED_THEN_ALIVE",
                               DataWriterResourceLimitsInstanceReplacementKind::
                                       type::DISPOSED_THEN_ALIVE,
                               "Allows a DataWriter first to reclaim a "
                               "disposed, acknowledged "
                               "instance, and then, if necessary, an alive, "
                               "acknowledged "
                               "instance."
                               "\n\n"
                               "When an unregistered, acknowledged instance is "
                               "not available "
                               "to reclaim, this kind allows a DataWriter to "
                               "first "
                               "try reclaiming a disposed, acknowledged "
                               "instance. If no instance "
                               "is reclaimable, then it tries reclaiming an "
                               "alive, acknowledged "
                               "instance. The least recently used (i.e., "
                               "disposed, registered, "
                               "or written) instance will be reclaimed.")
                        .value("ALIVE_OR_DISPOSED",
                               DataWriterResourceLimitsInstanceReplacementKind::
                                       type::ALIVE_OR_DISPOSED,
                               "Allows a DataWriter to reclaim a either an "
                               "alive acknowledged "
                               "instance or a disposed acknowledged instance."
                               "\n\n"
                               "When an unregistered acknowledged instance is "
                               "not available to "
                               "reclaim, this kind allows a DataWriter to "
                               "reclaim "
                               "either an alive, acknowledged instance or a "
                               "disposed, "
                               "acknowledged instance. If both instance kinds "
                               "are available to "
                               "reclaim, the DataWriter will reclaim the least "
                               "recently used "
                               "(i.e. disposed, registered, or written) "
                               "instance.")
                        .export_values();
            });

    l.push_back([m]() mutable {
        return init_class<DataWriterResourceLimits>(
                m,
                "DataWriterResourceLimits");
    });
}

}  // namespace pyrti
