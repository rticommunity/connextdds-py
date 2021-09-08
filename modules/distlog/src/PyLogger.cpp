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
#if rti_connext_version_lt(6, 0, 0, 0)
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
        {
            py::gil_scoped_acquire acquire;
            auto atexit = py::module::import("atexit");
            atexit.attr("register")(py::cpp_function([]() {
                PyLogger::finalize();
            }));
        }
    }
    return *PyLogger::_py_instance;
}

bool PyLogger::options(const PyLoggerOptions& options) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);

#if rti_connext_version_lt(6, 0, 0, 0)
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
#if rti_connext_version_lt(6, 0, 0, 0)
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

void PyLogger::filter_level(PyLogLevel level) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);
    auto retval = RTI_DL_DistLogger_setFilterLevel(PyLogger::instance()._instance, (int)level);
    if (retval != DDS_RETCODE_OK) throw dds::core::Error("Could not set Distributed Logger filter level");
}

void PyLogger::print_format(const rti::config::PrintFormat& format) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);
    auto retval = RTI_DL_DistLogger_setRTILoggerPrintFormat(PyLogger::instance()._instance, (NDDS_Config_LogPrintFormat)format.underlying());
    if (retval != RTI_TRUE) throw dds::core::Error("Could not set Distributed Logger print format");
}

void PyLogger::verbosity(const rti::config::LogCategory& category, const rti::config::Verbosity& level) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);
    RTI_DL_DistLogger_setRTILoggerVerbosityByCategory(
        PyLogger::instance()._instance,
        (NDDS_Config_LogCategory)category.underlying(),
        (NDDS_Config_LogVerbosity)level.underlying());
}

void PyLogger::log(PyLogLevel level, const std::string& message, const std::string& category) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);
    RTI_DL_DistLogger_logMessageWithLevelCategory(
        PyLogger::instance()._instance,
        (int)level,
        message.c_str(),
        category.c_str());
}

void PyLogger::log(const PyMessageParams& params) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);
    RTI_DL_DistLogger_logMessageWithParams(
        PyLogger::instance()._instance,
        &params._params
    );
}

void PyLogger::fatal(const std::string& message) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);
    RTI_DL_DistLogger_fatal(PyLogger::instance()._instance, message.c_str());
}

void PyLogger::severe(const std::string& message) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);
    RTI_DL_DistLogger_severe(PyLogger::instance()._instance, message.c_str());
}

void PyLogger::error(const std::string& message) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);
    RTI_DL_DistLogger_error(PyLogger::instance()._instance, message.c_str());
}

void PyLogger::warning(const std::string& message) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);
    RTI_DL_DistLogger_warning(PyLogger::instance()._instance, message.c_str());
}

void PyLogger::notice(const std::string& message) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);
    RTI_DL_DistLogger_notice(PyLogger::instance()._instance, message.c_str());
}

void PyLogger::info(const std::string& message) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);
    RTI_DL_DistLogger_info(PyLogger::instance()._instance, message.c_str());
}

void PyLogger::debug(const std::string& message) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);
    RTI_DL_DistLogger_debug(PyLogger::instance()._instance, message.c_str());
}

void PyLogger::trace(const std::string& message) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);
    RTI_DL_DistLogger_trace(PyLogger::instance()._instance, message.c_str());
}

void PyLogger::log(PyLogLevel level, const std::string& message) {
    std::lock_guard<std::recursive_mutex> lock(PyLogger::_lock);
    RTI_DL_DistLogger_log(PyLogger::instance()._instance, (int)level, message.c_str());
}

void init_logger(py::module& m) {
    py::class_<PyLogger> cls(m, "Logger");
    cls
        .def_static(
            "init",
            [cls](dds::core::optional<PyLoggerOptions*>& options) {
                if (has_value(options)) {
                    if (!PyLogger::options(*get_value(options))) {
                        throw dds::core::Error("Could not set logging options while initializing logger");
                    }
                }
                PyLogger::instance();
            },
            py::arg("options") = py::none(),
            py::call_guard<py::gil_scoped_release>(),
            "Initializes the distributed logger"
        )
        .def_static(
            "filter_level",
            &PyLogger::filter_level,
            py::arg("level"),
            py::call_guard<py::gil_scoped_release>(),
            "The logger filter level."
        )
        .def_static(
            "print_format",
            &PyLogger::print_format,
            py::arg("format"),
            py::call_guard<py::gil_scoped_release>(),
            "The logger print format."
            "NOTE: This will affect the print format of the associated"
            "DomainParticipant's logger as well."
        )
        .def_static(
            "verbosity",
            &PyLogger::verbosity,
            py::arg("category"),
            py::arg("level"),
            py::call_guard<py::gil_scoped_release>(),
            "The logger's verbosity."
            "NOTE: This will affect the verbosity of the associated"
            "DomainParticipant's logger as well."
        )
        .def_static(
            "log",
            (void (*)(PyLogLevel, const std::string&)) &PyLogger::log,
            py::arg("log_level"),
            py::arg("message"),
            py::call_guard<py::gil_scoped_release>(),
            "Log a message with the given log level."
        )
        .def_static(
            "log",
            (void (*)(PyLogLevel level, const std::string& message, const std::string& category)) &PyLogger::log,
            py::arg("log_level"),
            py::arg("message"),
            py::arg("category"),
            py::call_guard<py::gil_scoped_release>(),
            "Log a message with the given log level and category."
        )
        .def_static(
            "log",
            (void (*)(const PyMessageParams& params)) &PyLogger::log,
            py::arg("message_params"),
            py::call_guard<py::gil_scoped_release>(),
            "Log a message with the given message parameters."
        )
        .def_static(
            "fatal",
            &PyLogger::fatal,
            py::arg("message"),
            py::call_guard<py::gil_scoped_release>(),
            "Log a fatal message."
        )
        .def_static(
            "severe",
            &PyLogger::severe,
            py::arg("message"),
            py::call_guard<py::gil_scoped_release>(),
            "Log a severe message."
        )
        .def_static(
            "error",
            &PyLogger::error,
            py::arg("message"),
            py::call_guard<py::gil_scoped_release>(),
            "Log an error message."
        )
        .def_static(
            "warning",
            &PyLogger::warning,
            py::arg("message"),
            py::call_guard<py::gil_scoped_release>(),
            "Log a warning message."
        )
        .def_static(
            "notice",
            &PyLogger::notice,
            py::arg("message"),
            py::call_guard<py::gil_scoped_release>(),
            "Log a notice message."
        )
        .def_static(
            "info",
            &PyLogger::info,
            py::arg("message"),
            py::call_guard<py::gil_scoped_release>(),
            "Log an info message."
        )
        .def_static(
            "debug",
            &PyLogger::debug,
            py::arg("message"),
            py::call_guard<py::gil_scoped_release>(),
            "Log a debug message."
        )
        .def_static(
            "trace",
            &PyLogger::trace,
            py::arg("message"),
            py::call_guard<py::gil_scoped_release>(),
            "Log a trace message."
        )
        .def_static(
            "finalize",
            &PyLogger::finalize,
            py::call_guard<py::gil_scoped_release>(),
            "Destroy the Logger. It should not be accessed after this call."
        );
}

}