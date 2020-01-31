#include "PyConnext.hpp"
#include "PyLogLevel.hpp"

namespace pyrti {

void init_log_level(py::module& m) {
    py::enum_<PyLogLevel>(m, "LogLevel")
        .value(
            "SILENT",
            PyLogLevel::SILENT
        )
        .value(
            "FATAL",
            PyLogLevel::FATAL
        )
        .value(
            "SEVERE",
            PyLogLevel::SEVERE
        )
        .value(
            "ERROR",
            PyLogLevel::ERROR
        )
        .value(
            "WARNING",
            PyLogLevel::WARNING
        )
        .value(
            "NOTICE",
            PyLogLevel::NOTICE
        )
        .value(
            "INFO",
            PyLogLevel::INFO
        )
        .value(
            "DEBUG",
            PyLogLevel::DEBUG
        )
        .value(
            "TRACE",
            PyLogLevel::TRACE
        );
}

}
