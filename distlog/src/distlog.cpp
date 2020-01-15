#include "PyConnext.hpp"

void init_logger(py::module&);
void init_logger_options(py::module&);
void init_log_level(py::module&);
void init_message_params(py::module&);

PYBIND11_MODULE(distlog, m) {
    init_logger(m);
    init_logger_options(m);
    init_log_level(m);
    init_message_params(m);
}