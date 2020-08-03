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
