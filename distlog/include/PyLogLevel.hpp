#pragma once

#include <dds/core/SafeEnumeration.hpp>
#include <rti_dl/rti_dl_c.h>

namespace pyrti {
    enum PyLogLevel {
        PY_DISTLOG_SILENT      = 0,
        PY_DISTLOG_FATAL       = 100,
        PY_DISTLOG_SEVERE      = 200,   
        PY_DISTLOG_ERROR       = 300,
        PY_DISTLOG_WARNING     = 400,
        PY_DISTLOG_NOTICE      = 500,
        PY_DISTLOG_INFO        = 600,
        PY_DISTLOG_DEBUG       = 700,
        PY_DISTLOG_TRACE       = 800
    };

    void init_log_level(py::module&);
}
