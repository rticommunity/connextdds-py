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
#include "PyLogLevel.hpp"

namespace pyrti {

void init_log_level(py::module& m) {
    py::enum_<PyLogLevel>(m, "LogLevel")
        .value(
            "SILENT",
            PyLogLevel::PY_DISTLOG_SILENT
        )
        .value(
            "FATAL",
            PyLogLevel::PY_DISTLOG_FATAL
        )
        .value(
            "SEVERE",
            PyLogLevel::PY_DISTLOG_SEVERE
        )
        .value(
            "ERROR",
            PyLogLevel::PY_DISTLOG_ERROR
        )
        .value(
            "WARNING",
            PyLogLevel::PY_DISTLOG_WARNING
        )
        .value(
            "NOTICE",
            PyLogLevel::PY_DISTLOG_NOTICE
        )
        .value(
            "INFO",
            PyLogLevel::PY_DISTLOG_INFO
        )
        .value(
            "DEBUG",
            PyLogLevel::PY_DISTLOG_DEBUG
        )
        .value(
            "TRACE",
            PyLogLevel::PY_DISTLOG_TRACE
        );
}

}
