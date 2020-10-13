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
#include "PyMessageParams.hpp"

namespace pyrti {

PyMessageParams::PyMessageParams(PyLogLevel level, const std::string& message, const std::string& category, const dds::core::Time& timestamp) {
    this->_message = message;
    this->_category = category;

    DDS_Time_t ts;
    ts.sec = timestamp.sec();
    ts.nanosec = timestamp.nanosec();

    auto c = this->_category.empty() ? NULL : this->_category.c_str();

    RTI_DL_DistLogger_MessageParams params;
    params.log_level = (int) level;
    params.message = this->_message.c_str();
    params.category = c;
    params.timestamp = ts;
    this->_params = params;
}

PyLogLevel PyMessageParams::log_level() {
    return PyLogLevel(this->_params.log_level);
}

PyMessageParams& PyMessageParams::log_level(PyLogLevel level) {
    this->_params.log_level = (int) level;
    return *this;
}

std::string PyMessageParams::message() {
    return this->_message;
}

PyMessageParams& PyMessageParams::message(const std::string& message) {
    this->_message = message;
    this->_params.message = this->_message.c_str();
    return *this;
}

std::string PyMessageParams::category() {
    return this->_category;
}

PyMessageParams& PyMessageParams::category(const std::string& category) {
    this->_category = category;
    this->_params.category = category.empty() ? NULL : this->_category.c_str();
    return *this;
}

dds::core::Time PyMessageParams::timestamp() {
    return dds::core::Time(this->_params.timestamp.sec, this->_params.timestamp.nanosec);
}

PyMessageParams& PyMessageParams::timestamp(const dds::core::Time& timestamp) {
    DDS_Time_t ts;
    ts.sec = timestamp.sec(),
    ts.nanosec = timestamp.nanosec();
    this->_params.timestamp = ts;
    return *this;
}

void init_message_params(py::module& m) {
    py::class_<PyMessageParams>(m, "MessageParams")
        .def(
            py::init<
                PyLogLevel,
                const std::string&,
                const std::string&,
                const dds::core::Time&
            >(),
            py::arg("log_level"),
            py::arg("message"),
            py::arg("category"),
            py::arg("timestamp"),
            "Create MessageParams."
        )
        .def_property(
            "log_level",
            (PyLogLevel (PyMessageParams::*)()) &PyMessageParams::log_level,
            (PyMessageParams& (PyMessageParams::*)(PyLogLevel)) &PyMessageParams::log_level,
            "The message log level."
        )
        .def_property(
            "message",
            (std::string (PyMessageParams::*)()) &PyMessageParams::message,
            (PyMessageParams& (PyMessageParams::*)(const std::string&)) &PyMessageParams::message,
            "The log message."
        )
        .def_property(
            "category",
            (std::string (PyMessageParams::*)()) &PyMessageParams::category,
            (PyMessageParams& (PyMessageParams::*)(const std::string&)) &PyMessageParams::category,
            "The log message category."
        )
        .def_property(
            "timestamp",
            (dds::core::Time (PyMessageParams::*)()) &PyMessageParams::timestamp,
            (PyMessageParams& (PyMessageParams::*)(const dds::core::Time&)) &PyMessageParams::timestamp,
            "The timestamp of the log message."
        );
}

}
