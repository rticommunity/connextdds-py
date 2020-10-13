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

#include "PyConnext.hpp"
#include "PyLogger.hpp"
#include "PyLoggerOptions.hpp"
#include "PyLogLevel.hpp"
#include "PyMessageParams.hpp"


PYBIND11_MODULE(distlog, m) {
    pyrti::init_log_level(m);
    pyrti::init_logger_options(m);
    pyrti::init_message_params(m);
    pyrti::init_logger(m);
}