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
#include "PyLoggerOptions.hpp"

namespace pyrti {

PyLoggerOptions::PyLoggerOptions() {
    this->_options = RTI_DL_Options_createDefault();
}

PyLoggerOptions::~PyLoggerOptions() {
    RTI_DL_Options_delete(this->_options);
}

PyLoggerOptions::PyLoggerOptions(const PyLoggerOptions& other) {
    this->_options = RTI_DL_Options_createDefault();
    RTI_DL_Options_copy(this->_options, other._options);
}

dds::core::optional<PyDomainParticipant> PyLoggerOptions::participant() {
    dds::core::optional<PyDomainParticipant> retval;
    auto dp = RTI_DL_Options_getDomainParticipant(this->_options);
    if (dp != NULL) {
        retval = PyDomainParticipant(
            dds::core::construct_from_native_tag_t(),
            new rti::domain::DomainParticipantImpl(dp));
    }
    return retval;
}

PyLoggerOptions& PyLoggerOptions::participant(const PyDomainParticipant& dp) {
    RTI_DL_Options_setDomainParticipant(this->_options, const_cast<DDS_DomainParticipant*>(dp->native_participant()));
    return *this;
}

bool PyLoggerOptions::log_infrastructure_messages() {
    return RTI_DL_Options_getLogInfrastructureMessages(this->_options) == RTI_TRUE;
}

PyLoggerOptions& PyLoggerOptions::log_infrastructure_messages(bool enabled) {
    RTIBool value = enabled ? RTI_TRUE : RTI_FALSE;
    if (DDS_RETCODE_OK != RTI_DL_Options_setRemoteAdministrationEnabled(this->_options, value)) {
        throw dds::core::Error("Could not enable infrastructre message logging.");
    }
    return *this;
}

bool PyLoggerOptions::remote_administration_enabled() {
    return RTI_DL_Options_isRemoteAdministrationEnabled(this->_options) == RTI_TRUE;
}

PyLoggerOptions& PyLoggerOptions::remote_administration_enabled(bool enabled) {
    RTIBool value = enabled ? RTI_TRUE : RTI_FALSE;
    if (DDS_RETCODE_OK != RTI_DL_Options_setRemoteAdministrationEnabled(this->_options, value)) {
        throw dds::core::Error("Could not enable remote administration.");
    }
    return *this;
}

PyLogLevel PyLoggerOptions::filter_level() {
    return PyLogLevel(RTI_DL_Options_getFilterLevel(this->_options));
}

PyLoggerOptions& PyLoggerOptions::filter_level(PyLogLevel level) {
    if (DDS_RETCODE_OK != RTI_DL_Options_setFilterLevel(this->_options, (int)level)) {
        throw dds::core::Error("Could not set logger filter level.");
    }
    return *this;
}

int32_t PyLoggerOptions::domain_id() {
    return RTI_DL_Options_getDomainId(this->_options);
}

PyLoggerOptions& PyLoggerOptions::domain_id(int32_t id) {
    if (DDS_RETCODE_OK != RTI_DL_Options_setDomainId(this->_options, id)) {
        throw dds::core::Error("Could not set logger domain ID.");
    }
    return *this;
}

std::string PyLoggerOptions::qos_library() {
    return std::string(RTI_DL_Options_getQosLibrary(this->_options));
}

PyLoggerOptions& PyLoggerOptions::qos_library(const std::string& name) {
    if (DDS_RETCODE_OK != RTI_DL_Options_setQosLibrary(this->_options, name.c_str())) {
        throw dds::core::Error("Could not set logger QoS library name.");
    }
    return *this;
}

std::string PyLoggerOptions::qos_profile() {
    return std::string(RTI_DL_Options_getQosProfile(this->_options));
}

PyLoggerOptions& PyLoggerOptions::qos_profile(const std::string& name) {
    if (DDS_RETCODE_OK != RTI_DL_Options_setQosProfile(this->_options, name.c_str())) {
        throw dds::core::Error("Could not set logger QoS profile name.");
    }
    return *this;
}

int32_t PyLoggerOptions::queue_size() {
    return RTI_DL_Options_getQueueSize(this->_options);
}

PyLoggerOptions& PyLoggerOptions::queue_size(int32_t size) {
    if (DDS_RETCODE_OK != RTI_DL_Options_setQueueSize(this->_options, size)) {
        throw dds::core::Error("Could not set logger queue size.");
    }
    return *this;
}

rti::core::ThreadSettings PyLoggerOptions::thread_settings() {
    struct DDS_ThreadSettings_t* ts;
    ts = RTI_DL_Options_getThreadSettings(this->_options);
    return rti::core::ThreadSettings(*ts);
}

PyLoggerOptions& PyLoggerOptions::thread_settings(const rti::core::ThreadSettings& settings) {
    if (DDS_RETCODE_OK != RTI_DL_Options_setThreadSettings(this->_options, &settings.native())) {
        throw dds::core::Error("Could not set logger thread settings.");
    }
    return *this;
}

std::string PyLoggerOptions::application_kind() {
    return std::string(RTI_DL_Options_getApplicationKind(this->_options));
}

PyLoggerOptions& PyLoggerOptions::application_kind(const std::string& kind) {
    if (DDS_RETCODE_OK != RTI_DL_Options_setApplicationKind(this->_options, kind.c_str())) {
        throw dds::core::Error("Could not set logger application kind.");
    }
    return *this;
}

bool PyLoggerOptions::echo_to_stdout() {
    return RTI_DL_Options_getEchoToStdout(this->_options) == RTI_TRUE;
}

PyLoggerOptions& PyLoggerOptions::echo_to_stdout(bool enabled) {
    RTIBool value = enabled ? RTI_TRUE : RTI_FALSE;
    if (DDS_RETCODE_OK != RTI_DL_Options_setEchoToStdout(this->_options, value)) {
        throw dds::core::Error("Could not enable echo to stdout.");
    }
    return *this;
}

void init_logger_options(py::module& m) {
    py::class_<PyLoggerOptions>(m, "LoggerOptions")
        .def(
            py::init<>(),
            "Create a LoggerOptions instance with default settings."
        )
        .def_property(
            "participant",
            (dds::core::optional<PyDomainParticipant> (PyLoggerOptions::*)()) &PyLoggerOptions::participant,
            (PyLoggerOptions& (PyLoggerOptions::*)(const PyDomainParticipant&)) &PyLoggerOptions::participant,
            "The DomainParticipant to use for the logger."
        )
        .def_property(
            "log_infrastructure_messages",
            (bool (PyLoggerOptions::*)()) &PyLoggerOptions::log_infrastructure_messages,
            (PyLoggerOptions& (PyLoggerOptions::*)(bool)) &PyLoggerOptions::log_infrastructure_messages,
            "Toggle for logging infrastructure messages."
        )
        .def_property(
            "remote_administration_enabled",
            (bool (PyLoggerOptions::*)()) &PyLoggerOptions::remote_administration_enabled,
            (PyLoggerOptions& (PyLoggerOptions::*)(bool)) &PyLoggerOptions::remote_administration_enabled,
            "Toggle for remote administration."
        )
        .def_property(
            "filter_level",
            (PyLogLevel (PyLoggerOptions::*)()) &PyLoggerOptions::filter_level,
            (PyLoggerOptions& (PyLoggerOptions::*)(PyLogLevel)) &PyLoggerOptions::filter_level,
            "Toggle for log filter level."
        )
        .def_property(
            "domain_id",
            (int32_t (PyLoggerOptions::*)()) &PyLoggerOptions::domain_id,
            (PyLoggerOptions& (PyLoggerOptions::*)(int32_t)) &PyLoggerOptions::domain_id,
            "The domain ID for logging."
        )
        .def_property(
            "qos_library",
            (std::string (PyLoggerOptions::*)()) &PyLoggerOptions::qos_library,
            (PyLoggerOptions& (PyLoggerOptions::*)(const std::string&)) &PyLoggerOptions::qos_library,
            "The QoS library name."
        )
        .def_property(
            "qos_profile",
            (std::string (PyLoggerOptions::*)()) &PyLoggerOptions::qos_profile,
            (PyLoggerOptions& (PyLoggerOptions::*)(const std::string&)) &PyLoggerOptions::qos_profile,
            "The QoS profile name."
        )
        .def_property(
            "queue_size",
            (int32_t (PyLoggerOptions::*)()) &PyLoggerOptions::queue_size,
            (PyLoggerOptions& (PyLoggerOptions::*)(int32_t)) &PyLoggerOptions::queue_size,
            "The logger's queue size."
        )
        .def_property(
            "thread_settings",
            (rti::core::ThreadSettings (PyLoggerOptions::*)()) &PyLoggerOptions::thread_settings,
            (PyLoggerOptions& (PyLoggerOptions::*)(const rti::core::ThreadSettings&)) &PyLoggerOptions::thread_settings,
            "The settings for the thread handling logging."
        )
        .def_property(
            "application_kind",
            (std::string (PyLoggerOptions::*)()) &PyLoggerOptions::application_kind,
            (PyLoggerOptions& (PyLoggerOptions::*)(const std::string&)) &PyLoggerOptions::application_kind,
            "The application_kind."
        )
        .def_property(
            "echo_to_stdout",
            (bool (PyLoggerOptions::*)()) &PyLoggerOptions::echo_to_stdout,
            (PyLoggerOptions& (PyLoggerOptions::*)(bool)) &PyLoggerOptions::echo_to_stdout,
            "Toggle for echo to stdout."
        );
}

}
