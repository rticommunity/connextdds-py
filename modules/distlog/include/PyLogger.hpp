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
        static PyLogger& instance();
        static bool options(const PyLoggerOptions&);
        static void finalize();
        ~PyLogger();
        PyLogger& filter_level(PyLogLevel);
        PyLogger& print_format(const rti::config::PrintFormat&);
        PyLogger& verbosity(const rti::config::LogCategory&, const rti::config::Verbosity&);
        void log(PyLogLevel, const std::string&, const std::string&);
        void log(const PyMessageParams&);
        void fatal(const std::string&);
        void severe(const std::string&);
        void error(const std::string&);
        void warning(const std::string&);
        void notice(const std::string&);
        void info(const std::string&);
        void debug(const std::string&);
        void trace(const std::string&);
        void log(PyLogLevel, const std::string&);

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