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
#include "PyLogger.hpp"
#include <mutex>

namespace pyrti {

bool PyLogger::_options_set = false;
std::recursive_mutex PyLogger::_lock;
std::unique_ptr<PyLogger> PyLogger::_py_instance;
#if rti_connext_version_lt(6, 0, 0)
std::unique_ptr<PyLoggerOptions> PyLogger::_options;
#endif

PyLogger& PyLogger::instance() {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);

    if (!PyLogger::_py_instance) {
        if (!PyLogger::_options_set) {
            PyLogger::options(PyLoggerOptions());
            PyLogger::_options_set = true;
        }
        PyLogger::_py_instance.reset(new PyLogger());
    }
    return *PyLogger::_py_instance;
}

bool PyLogger::options(const PyLoggerOptions& options) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);

#if rti_connext_version_lt(6, 0, 0)
    if (PyLogger::_options_set) return false;
    PyLogger::_options.reset(new PyLoggerOptions(options));
    bool retval = (bool)RTI_DL_DistLogger_setOptions(PyLogger::_options->_options);
#else
    bool retval = (bool)RTI_DL_DistLogger_setOptions(options._options);
#endif

    if (retval) {
        PyLogger::_options_set = true;
    }

    return retval;
}

void PyLogger::finalize() {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);
    if (PyLogger::_py_instance == nullptr) {
        return;
    }

    auto ptr = PyLogger::_py_instance.release();
    delete ptr;
#if rti_connext_version_lt(6, 0, 0)
    auto opt_ptr = PyLogger::_options.release();
    delete opt_ptr;
#endif
}

PyLogger::PyLogger() {
    this->_instance = RTI_DL_DistLogger_getInstance();
}

PyLogger::~PyLogger() {
    RTI_DL_DistLogger_finalizeInstance();
}

PyLogger& PyLogger::filter_level(PyLogLevel level) {
    auto retval = RTI_DL_DistLogger_setFilterLevel(this->_instance, (int)level);
    if (retval != DDS_RETCODE_OK) throw dds::core::Error("Could not set Distributed Logger filter level");
    return *this;
}

PyLogger& PyLogger::print_format(const rti::config::PrintFormat& level) {
    auto retval = RTI_DL_DistLogger_setRTILoggerPrintFormat(this->_instance, (NDDS_Config_LogPrintFormat)level.underlying());
    if (retval != RTI_TRUE) throw dds::core::Error("Could not set Distributed Logger filter level");
    return *this;
}

PyLogger& PyLogger::verbosity(const rti::config::LogCategory& category, const rti::config::Verbosity& level) {
    RTI_DL_DistLogger_setRTILoggerVerbosityByCategory(this->_instance, (NDDS_Config_LogCategory)category.underlying(), (NDDS_Config_LogVerbosity)level.underlying());
    return *this;
}

void PyLogger::log(PyLogLevel level, const std::string& message, const std::string& category) {
    RTI_DL_DistLogger_logMessageWithLevelCategory(
        this->_instance,
        (int)level,
        message.c_str(),
        category.c_str());
}

void PyLogger::log(const PyMessageParams& params) {
    RTI_DL_DistLogger_logMessageWithParams(
        this->_instance,
        &params._params
    );
}

void PyLogger::fatal(const std::string& message) {
    RTI_DL_DistLogger_fatal(this->_instance, message.c_str());
}

void PyLogger::severe(const std::string& message) {
    RTI_DL_DistLogger_severe(this->_instance, message.c_str());
}

void PyLogger::error(const std::string& message) {
    RTI_DL_DistLogger_error(this->_instance, message.c_str());
}

void PyLogger::warning(const std::string& message) {
    RTI_DL_DistLogger_warning(this->_instance, message.c_str());
}

void PyLogger::notice(const std::string& message) {
    RTI_DL_DistLogger_notice(this->_instance, message.c_str());
}

void PyLogger::info(const std::string& message) {
    RTI_DL_DistLogger_info(this->_instance, message.c_str());
}

void PyLogger::debug(const std::string& message) {
    RTI_DL_DistLogger_debug(this->_instance, message.c_str());
}

void PyLogger::trace(const std::string& message) {
    RTI_DL_DistLogger_trace(this->_instance, message.c_str());
}

void PyLogger::log(PyLogLevel level, const std::string& message) {
    RTI_DL_DistLogger_log(this->_instance, (int)level, message.c_str());
}

void init_logger(py::module& m) {
    py::class_<PyLogger>(m, "Logger")
        .def_property_readonly(
            "filter_level",
            &PyLogger::filter_level,
            "The logger filter level."
        )
        .def_property_readonly(
            "print_format",
            &PyLogger::print_format,
            "The logger print format."
        )
        .def_property_readonly(
            "verbosity",
            &PyLogger::verbosity,
            "The logger's verbosity."
        )
        .def(
            "log",
            (void (PyLogger::*)(PyLogLevel, const std::string&)) &PyLogger::log,
            py::arg("log_level"),
            py::arg("message"),
            "Log a message with the given log level"
        )
        .def(
            "log",
            (void (PyLogger::*)(PyLogLevel, const std::string&, const std::string&)) &PyLogger::log,
            py::arg("log_level"),
            py::arg("message"),
            py::arg("category"),
            "Log a message with the given log level and category"
        )
        .def(
            "log",
            (void (PyLogger::*)(const PyMessageParams&)) &PyLogger::log,
            py::arg("message_params"),
            "Log a message with the given log level and category"
        )
        .def(
            "fatal",
            &PyLogger::fatal,
            "Log a fatal message."
        )
        .def(
            "severe",
            &PyLogger::severe,
            "Log a severe message."
        )
        .def(
            "error",
            &PyLogger::error,
            "Log an error message."
        )
        .def(
            "warning",
            &PyLogger::warning,
            "Log a warning message."
        )
        .def(
            "notice",
            &PyLogger::notice,
            "Log a notice message."
        )
        .def(
            "info",
            &PyLogger::info,
            "Log an info message."
        )
        .def(
            "debug",
            &PyLogger::debug,
            "Log a debug message."
        )
        .def(
            "trace",
            &PyLogger::trace,
            "Log a trace message."
        )
        .def_property_readonly_static(
            "instance",
            [](py::object&) -> PyLogger& {
                return PyLogger::instance();
            },
            py::call_guard<py::gil_scoped_release>(),
            "Get the Logger instance."
        )
        .def_static(
            "options",
            &PyLogger::options,
            py::arg("options"),
            py::call_guard<py::gil_scoped_release>(),
            "Set the options for the Logger instance (must be set prior to "
            "accessing the instance."
        )
        .def_static(
            "finalize",
            &PyLogger::finalize,
            py::call_guard<py::gil_scoped_release>(),
            "Destroy the Logger. It should not be accessed after this call."
        );
}

}