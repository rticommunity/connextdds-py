#pragma once

#include <rti_dl/rti_dl_c.h>
#include "PyLogLevel.hpp"
#include <dds/core/Time.hpp>

namespace pyrti {
    class PyLogger;

    class PyMessageParams {
    public:
        PyMessageParams(PyLogLevel, const std::string&, const std::string&, const dds::core::Time&);
        PyLogLevel log_level();
        PyMessageParams& log_level(PyLogLevel);
        std::string message();
        PyMessageParams& message(const std::string&);
        std::string category();
        PyMessageParams& category(const std::string&);
        dds::core::Time timestamp();
        PyMessageParams& timestamp(const dds::core::Time&);
    private:
        std::string _message;
        std::string _category;
        RTI_DL_DistLogger_MessageParams _params;
        friend class PyLogger;
    };

    void init_message_params(py::module&);
}