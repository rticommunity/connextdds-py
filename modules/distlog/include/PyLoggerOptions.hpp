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
        PyLoggerOptions(const PyLoggerOptions&);
        dds::core::optional<PyDomainParticipant> participant();
        PyLoggerOptions& participant(const PyDomainParticipant&);
        bool log_infrastructure_messages();
        PyLoggerOptions& log_infrastructure_messages(bool);
        bool remote_administration_enabled();
        PyLoggerOptions& remote_administration_enabled(bool);
        PyLogLevel filter_level();
        PyLoggerOptions& filter_level(PyLogLevel);
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
        friend class PyLogger;
    };

    void init_logger_options(py::module&);
}