#include "PyConnext.hpp"
#include "PyLogLevel.hpp"

void init_log_level(py::module& m) {
    py::enum_<pyrti::PyLogLevel>(m, "LogLevel")
        .value(
            "SILENT",
            pyrti::PyLogLevel::SILENT
        )
        .value(
            "FATAL",
            pyrti::PyLogLevel::FATAL
        )
        .value(
            "SEVERE",
            pyrti::PyLogLevel::SEVERE
        )
        .value(
            "ERROR",
            pyrti::PyLogLevel::ERROR
        )
        .value(
            "WARNING",
            pyrti::PyLogLevel::WARNING
        )
        .value(
            "NOTICE",
            pyrti::PyLogLevel::NOTICE
        )
        .value(
            "INFO",
            pyrti::PyLogLevel::INFO
        )
        .value(
            "DEBUG",
            pyrti::PyLogLevel::DEBUG
        )
        .value(
            "TRACE",
            pyrti::PyLogLevel::TRACE
        );
}