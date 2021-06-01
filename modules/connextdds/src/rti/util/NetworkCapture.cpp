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

#if rti_connext_version_gte(6, 1, 0)

#include <rti/util/util.hpp>
#include <PySafeEnum.hpp>

using namespace rti::util::network_capture;

namespace pyrti {

template<>
void init_class_defs(py::class_<ContentKindMask>& cls)
{
    cls.def_property_readonly_static(
                "ALL",
                [](py::object&) {
                    return ContentKindMask::all();
                },
                "The RTPS frames in the capture file will not include "
                "user data (either plain or encrypted).")
            .def_property_readonly_static(
                "NONE",
                [](py::object&) {
                    return ContentKindMask::none();
                },
                "The RTPS frames in the capture file will be saved "
                "as they are.")
            .def_property_readonly_static(
                "DEFAULT",
                [](py::object&) {
                    return ContentKindMask::default_mask();
                },
                "Default mask value: do not remove any content.")
            .def_property_readonly_static(
                "ENCRYPTED",
                [](py::object&) {
                    return ContentKindMask::encrypted();
                },
                "Encrypted user data.")
            .def_property_readonly_static(
                "USER",
                [](py::object&) {
                    return ContentKindMask::user();
                },
                "Serialized user data.");
}


template<>
void init_class_defs(py::class_<TrafficKindMask>& cls)
{
    cls.def_property_readonly_static(
                "ALL",
                [](py::object&) {
                    return TrafficKindMask::all();
                },
                "Capture all traffic (both inbound and outbound).")
            .def_property_readonly_static(
                "NONE",
                [](py::object&) {
                    return TrafficKindMask::none();
                },
                "Do not capture any traffic.")
            .def_property_readonly_static(
                "DEFAULT",
                [](py::object&) {
                    return TrafficKindMask::default_mask();
                },
                "Default mask.")
            .def_property_readonly_static(
                "IN",
                [](py::object&) {
                    return TrafficKindMask::in();
                },
                "Do not capture inbound traffic.")
            .def_property_readonly_static(
                "OUT",
                [](py::object&) {
                    return TrafficKindMask::out();
                },
                "Do not capture outbound traffic.");
}

template<>
void init_class_defs(py::class_<NetworkCaptureParams>& cls)
{
    cls.def(py::init<>(),
                "Create a default NetworkCaptureParams.")
            .def_property(
                "transports",
                (std::vector<std::string> (NetworkCaptureParams::*)() const)
                        & NetworkCaptureParams::transports,
                (NetworkCaptureParams& (NetworkCaptureParams::*) (const std::vector<std::string>&))
                        & NetworkCaptureParams::transports,
                "List of transports to capture.")
            .def_property(
                "dropped_content",
                (ContentKindMask (NetworkCaptureParams::*)() const)
                        & NetworkCaptureParams::dropped_content,
                (NetworkCaptureParams& (NetworkCaptureParams::*) (ContentKindMask) )
                        & NetworkCaptureParams::dropped_content,
                "The type of content excluded from capture files.")
            .def_property(
                "traffic",
                (TrafficKindMask (NetworkCaptureParams::*)() const)
                        & NetworkCaptureParams::traffic,
                (NetworkCaptureParams& (NetworkCaptureParams::*) (TrafficKindMask) )
                        & NetworkCaptureParams::traffic,
                "The traffic direction to capture.")
            .def_property(
                "parse_encrypted_content",
                (bool (NetworkCaptureParams::*)() const)
                        & NetworkCaptureParams::parse_encrypted_content,
                (NetworkCaptureParams& (NetworkCaptureParams::*) (bool) )
                        & NetworkCaptureParams::parse_encrypted_content,
                "Toggle for parsing encrypted contents.");
            .def_property(
                "checkpoint_thread_settings",
                (rti::core::ThreadSettings& (NetworkCaptureParams::*)() const)
                        & NetworkCaptureParams::checkpoint_thread_settings,
                (NetworkCaptureParams& (NetworkCaptureParams::*) (const rti::core::ThreadSettings&) )
                        & NetworkCaptureParams::checkpoint_thread_settings,
                "Checkpoint thread properties.")
            .def_property(
                "frame_queue_size",
                (int32_t (NetworkCaptureParams::*)() const)
                        & NetworkCaptureParams::frame_queue_size,
                (NetworkCaptureParams& (NetworkCaptureParams::*) (int32_t) )
                        & NetworkCaptureParams::frame_queue_size,
                "Set the size of the frame queue.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}


template<>
void process_inits<NetworkCaptureParams>(py::module& m, ClassInitList& l)
{
     l.push_back([m]() mutable {
        auto cls = init_mask_type<ContentKindMask, uint64_t>(
                m,
                "ContentKindMask",
                "Create a ContentKindMask with no bits set.");
        return [cls]() mutable {
            init_class_defs<ContentKindMask>(cls);
        };
    });

    l.push_back([m]() mutable {
        auto cls = init_mask_type<TrafficKindMask, uint64_t>(
                m,
                "TrafficKindMask",
                "Create a TrafficKindMask with no bits set.");
        return [cls]() mutable {
            init_class_defs<TrafficKindMask>(cls);
        };
    });

    l.push_back([m]() mutable {
        auto retval = init_class<NetworkCaptureParams>(m, "NetworkCaptureParams");

        m.def(
                "enable",
                &rti::util::network_capture::enable,
                "Enable network capture.\n\n"
                "Must be called before any using any other int the RTI Connext "
                "library.")
            .def(
                "disable",
                &rti::util::network_capture::disable,
                "Disable network capture. Must delete captured Domain Participants "
                "and stop capture before calling disable.")
            .def(
                "set_default_params",
                &rti::util::network_capture::set_default_params,
                py::arg("params"),
                "Set default network capture parameters.")
            .def(
                "start",
                (bool (rti::util::network_capture::*)(const std::string&))
                    &rti::util::network_capture::start,
                py::arg("filename"),
                "Start network capture.")
            .def(
                "start",
                (bool (rti::util::network_capture::*)(dds::domain::DomainParticipant, const std::string&))
                    &rti::util::network_capture::start,
                py::arg("participant"),
                py::arg("filename"),
                "Start network capture for a participant.")
            .def(
                "start",
                (bool (rti::util::network_capture::*)(const std::string&, const NetworkCaptureParams&))
                    &rti::util::network_capture::start,
                py::arg("filename"),
                py::arg("params"),
                "Start network capture with parameters.")
            .def(
                "start",
                (bool (rti::util::network_capture::*)(dds::domain::DomainParticipant, const std::string&, const NetworkCaptureParams&))
                    &rti::util::network_capture::start,
                py::arg("participant"),
                py::arg("filename"),
                py::arg("params"),
                "Start network capture with parameters for a participant.")
            .def(
                "stop",
                (bool (rti::util::network_capture::*)())
                    &rti::util::network_capture::stop,
                "Stop network capture.")
            .def(
                "stop",
                (bool (rti::util::network_capture::*)(dds::domain::DomainParticipant))
                    &rti::util::network_capture::start,
                py::arg("participant"),
                "Stop network capture.")
            .def(
                "pause",
                (bool (rti::util::network_capture::*)())
                    &rti::util::network_capture::pause,
                "Pause network capture.")
            .def(
                "pause",
                (bool (rti::util::network_capture::*)(dds::domain::DomainParticipant))
                    &rti::util::network_capture::pause,
                py::arg("participant"),
                "Pause network capture.")
            .def(
                "resume",
                (bool (rti::util::network_capture::*)())
                    &rti::util::network_capture::resume,
                "Resume network capture.")
            .def(
                "resume",
                (bool (rti::util::network_capture::*)(dds::domain::DomainParticipant))
                    &rti::util::network_capture::resume,
                py::arg("participant"),
                "Resume network capture.");

        return retval;
    });
}

void init_network_capture(py::module& m, ClassInitList& l) {
    auto netcap = m.def_submodule(
        "network_capture",
        "Save network traffic into a acpture file for further analysis.");

    process_inits<NetworkCaptureParams>(netcap, l);
}

}  // namespace pyrti

#endif