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
#include <rti/util/util.hpp>
#include <PySafeEnum.hpp>

using namespace rti::util::heap_monitoring;

namespace pyrti {

#if rti_connext_version_gte(6, 1, 0)
template<>
void init_class_defs(py::class_<HeapMonitoringParams>& cls)
{
    cls.def(py::init<SnapshotOutputFormat, SnapshotContentFormat>(),
            py::arg_v(
                    "snapshot_output_format",
                    SnapshotOutputFormat::STANDARD,
                    "SnapshotOutputFormat.STANDARD"),
            py::arg_v(
                    "snapshot_content_format",
                    SnapshotContentFormat::DEFAULT,
                    "SnapshotOutputFormat.DEFAULT"),
            "Create parameters for heap monitoring.")
            .def_property(
                    "snapshot_output_format",
                    (SnapshotOutputFormat(HeapMonitoringParams::*)() const)
                            & HeapMonitoringParams::snapshot_output_format,
                    (void (HeapMonitoringParams::*) (SnapshotOutputFormat))
                            & HeapMonitoringParams::snapshot_output_format,
                    "Format of the snapshot output.")
            .def_property(
                    "snapshot_content_format",
                    (SnapshotContentFormat (HeapMonitoringParams::*)() const)
                            & HeapMonitoringParams::snapshot_content_format,
                    (void (HeapMonitoringParams::*) (SnapshotContentFormat) )
                            & HeapMonitoringParams::snapshot_content_format,
                    "Information included in snapshot output.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}


template<>
void process_inits<HeapMonitoringParams>(py::module& m, ClassInitList& l)
{
    init_dds_safe_enum<SnapshotOutputFormat_def>(
            m,
            "SnapshotOutputFormat",
            [](py::object& o) {
                py::enum_<SnapshotOutputFormat::type>(o, "SnapshotOutputFormat")
                        .value("STANDARD",
                               SnapshotOutputFormat::type::STANDARD,
                               "The output of the snapshot will be in plain "
                               "text.")
                        .value("COMPRESSED",
                               SnapshotOutputFormat::type::COMPRESSED,
                               "The output of the snapshot will be compressed "
                               "using Zlib techonology.\n\n"
                               "The file can be uncompressed using zlib-flate.")
                        .export_values();
            });

    init_dds_safe_enum<SnapshotContentFormat_def>(
            m,
            "SnapshotContentFormat",
            [](py::object& o) {
                py::enum_<SnapshotContentFormat::type>(o, "SnapshotContentFormat")
                        .value("TOPIC",
                               SnapshotContentFormat::type::TOPIC,
                               "Add the topic to the snapshot of heap monitoring.")
                        .value("FUNCTION",
                               SnapshotContentFormat::type::FUNCTION,
                               "Add the function name to the snapshot of heap "
                               "monitoring.")
                        .value("ACTIVITY",
                               SnapshotContentFormat::type::ACTIVITY,
                               "Add the activity context to the snapshot of heap "
                               "monitoring. The user can select the information "
                               "that will be part of the activity context by using "
                               "the API activity_context.set_attribute_mask.")
                        .value("DEFAULT",
                               SnapshotContentFormat::type::DEFAULT,
                               "Add all the optional attributes to the snapshot "
                               "of heap monitoring.")
                        .value("MINIMAL",
                               SnapshotContentFormat::type::MINIMAL,
                               "Not add any optional attribute to the snapshot "
                               "of heap monitoring.")
                        .export_values();
            });

    l.push_back([m]() mutable {
        return init_class<HeapMonitoringParams>(m, "HeapMonitoringParams");
    });
}
#endif

}  // namespace pyrti

void init_heap_monitoring(py::module& m, pyrti::ClassInitList& l, pyrti::DefInitVector& v) {
    auto heapmon = m.def_submodule(
        "heap_monitoring",
        "Monitor memory allocations done by the middleware on "
        "the native heap.");

#if rti_connext_version_gte(6, 1, 0)
    pyrti::process_inits<HeapMonitoringParams>(heapmon, l);
#endif

    v.push_back(
        [heapmon]() mutable {
            heapmon.def(
                "enable",
                (bool (*)()) &rti::util::heap_monitoring::enable,
                "Start monitoring the heap memory used by RTI Connext. "
                "Must be called before any using any other int the RTI Connext "
                "library.")
#if rti_connext_version_gte(6, 1, 0)
            .def(
                "enable",
                (bool (*)(const HeapMonitoringParams&)) &rti::util::heap_monitoring::enable,
                py::arg("params"),
                "Start monitoring the heap memory used by RTI Connext with params. "
                "Must be called before any using any other in the RTI Connext "
                "library.")
#endif
            .def(
                "disable",
                &rti::util::heap_monitoring::disable,
                "Stop monitoring the heap memory used by RTI Connext.")
            .def(
                "pause",
                &rti::util::heap_monitoring::pause,
                "Pauses heap monitoring.")
            .def(
                "resume",
                &rti::util::heap_monitoring::resume,
                "Resumes heap monitoring.")
            .def(
                "take_snapshot",
                &rti::util::heap_monitoring::take_snapshot,
                py::arg("filename"),
                py::arg("print_details") = false,
                "Saves the current heap memory usage in a file.");
        }
    );
}