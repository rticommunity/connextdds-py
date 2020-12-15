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

#include "PyEntity.hpp"
#include <rti_dl/rti_dl_c.h>
#include <rti/config/Logger.hpp>
#include "PyLoggerOptions.hpp"
#include "PyLogLevel.hpp"
#include "PyMessageParams.hpp"
#include <mutex>

namespace pyrti {
    class PyLoggerOptions;

    class PyLogger {
    public:
        ~PyLogger();
        static PyLogger& instance();
        static bool options(const PyLoggerOptions&);
        static void finalize();
        static void filter_level(PyLogLevel);
        static void print_format(const rti::config::PrintFormat&);
        static void verbosity(const rti::config::LogCategory&, const rti::config::Verbosity&);
        static void log(PyLogLevel, const std::string&, const std::string&);
        static void log(const PyMessageParams&);
        static void fatal(const std::string&);
        static void severe(const std::string&);
        static void error(const std::string&);
        static void warning(const std::string&);
        static void notice(const std::string&);
        static void info(const std::string&);
        static void debug(const std::string&);
        static void trace(const std::string&);
        static void log(PyLogLevel, const std::string&);

    private:
        PyLogger();
        RTI_DL_DistLogger* _instance;
        static std::unique_ptr<PyLogger> _py_instance;
        static bool _options_set;
        static std::recursive_mutex _lock;
#if rti_connext_version_lt(6, 0, 0)
        static std::unique_ptr<PyLoggerOptions> _options;
#endif
    };

    void init_logger(py::module&);
}