#include "PyConnext.hpp"
#include "PyMessageParams.hpp"

namespace pyrti {
    PyMessageParams::PyMessageParams(PyLogLevel level, const std::string& message, const std::string& category, const dds::core::Time& timestamp) {
        this->_message = message;
        this->_category = category;
        DDS_Time_t ts = {
            .sec = timestamp.sec(),
            .nanosec = timestamp.nanosec()
        };
        auto c = this->_category.empty() ? NULL : this->_category.c_str();
        this->_params = {
            .log_level = (int) level,
            .message = this->_message.c_str(),
            .category = c,
            .timestamp = ts
        };
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
        DDS_Time_t ts = {
            .sec = timestamp.sec(),
            .nanosec = timestamp.nanosec()
        };
        this->_params.timestamp = ts;
        return *this;
    }
}

void init_message_params(py::module& m) {
    py::class_<pyrti::PyMessageParams>(m, "MessageParams")
        .def(
            py::init<
                pyrti::PyLogLevel,
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
            (pyrti::PyLogLevel (pyrti::PyMessageParams::*)()) &pyrti::PyMessageParams::log_level,
            (pyrti::PyMessageParams& (pyrti::PyMessageParams::*)(pyrti::PyLogLevel)) &pyrti::PyMessageParams::log_level,
            "The message log level."
        )
        .def_property(
            "message",
            (std::string (pyrti::PyMessageParams::*)()) &pyrti::PyMessageParams::message,
            (pyrti::PyMessageParams& (pyrti::PyMessageParams::*)(const std::string&)) &pyrti::PyMessageParams::message,
            "The log message."
        )
        .def_property(
            "category",
            (std::string (pyrti::PyMessageParams::*)()) &pyrti::PyMessageParams::category,
            (pyrti::PyMessageParams& (pyrti::PyMessageParams::*)(const std::string&)) &pyrti::PyMessageParams::category,
            "The log message category."
        )
        .def_property(
            "timestamp",
            (dds::core::Time (pyrti::PyMessageParams::*)()) &pyrti::PyMessageParams::timestamp,
            (pyrti::PyMessageParams& (pyrti::PyMessageParams::*)(const dds::core::Time&)) &pyrti::PyMessageParams::timestamp,
            "The timestamp of the log message."
        );
}