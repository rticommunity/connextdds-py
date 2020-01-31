#include "PyConnext.hpp"
#include "PyLogger.hpp"
#include "PyLoggerOptions.hpp"
#include "PyLogLevel.hpp"
#include "PyMessageParams.hpp"


PYBIND11_MODULE(distlog, m) {
    pyrti::init_logger(m);
    pyrti::init_logger_options(m);
    pyrti::init_log_level(m);
    pyrti::init_message_params(m);
}