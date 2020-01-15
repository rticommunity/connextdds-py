#pragma once

#include <dds/core/SafeEnumeration.hpp>
#include <rti_dl/rti_dl_c.h>

namespace pyrti {
    enum PyLogLevel {
        SILENT      = 0,
        FATAL       = 100,
        SEVERE      = 200,   
        ERROR       = 300,
        WARNING     = 400,
        NOTICE      = 500,
        INFO        = 600,
        DEBUG       = 700,
        TRACE       = 800
    };
}
