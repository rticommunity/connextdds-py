#pragma once

#include "PyConnext.hpp"
#include <rti_dl/rti_dl_c.h>
#include <rti/core/ThreadSettings.hpp>
#include "PyLogLevel.hpp"

namespace pyrti {
    class PyLogger;

    class PyLoggerOptions {
    public:
        PyLoggerOptions();
        ~PyLoggerOptions();
        PyLoggerOptions(const pyrti::PyLoggerOptions&);
        py::object participant();
        pyrti::PyLoggerOptions& participant(const pyrti::PyDomainParticipant&);
        bool log_infrastructure_messages();
        PyLoggerOptions& log_infrastructure_messages(bool);
        bool remote_administration_enabled();
        PyLoggerOptions& remote_administration_enabled(bool);
        pyrti::PyLogLevel filter_level();
        PyLoggerOptions& filter_level(pyrti::PyLogLevel);
        int32_t domain_id();
        PyLoggerOptions& domain_id(int32_t);
        std::string qos_library();
        PyLoggerOptions& qos_library(const std::string&);
        std::string qos_profile();
        PyLoggerOptions& qos_profile(const std::string&);
        int32_t queue_size();
        PyLoggerOptions& queue_size(int32_t);
        rti::core::ThreadSettings thread_settings();
        PyLoggerOptions& thread_settings(const rti::core::ThreadSettings&);
        std::string application_kind();
        PyLoggerOptions& application_kind(const std::string&);
        bool echo_to_stdout();
        PyLoggerOptions& echo_to_stdout(bool);

    private:
        RTI_DL_Options* _options;
        friend class pyrti::PyLogger;
    };
}