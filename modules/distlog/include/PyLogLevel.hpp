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
