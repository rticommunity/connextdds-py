#include "PyConnext.hpp"
#include <rti/config/Logger.hpp>
#include "PySafeEnum.hpp"

using namespace rti::config;

namespace pyrti {

template<>
void init_class_defs(py::class_<Logger>& cls) {
    cls
        .def_static(
            "get_instance",
            &Logger::instance,
            "Get the singleton instance of this type."
        )
        .def_property(
            "verbosity",
            (Verbosity (Logger::*)()) &Logger::verbosity,
            (void (Logger::*)(Verbosity)) &Logger::verbosity,
            "The verbosity at which RTI Connext is currently logging "
            "diagnostic information."
        )
        .def_property(
            "print_format",
            (PrintFormat (Logger::*)()) &Logger::print_format,
            (void (Logger::*)(PrintFormat)) &Logger::print_format,
            "The message format that RTI Connext will use to log diagnostic "
            "information."
        )
        .def(
            "verbosity_by_category",
            (Verbosity (Logger::*)(LogCategory)) &Logger::verbosity_by_category,
            "Get the verbosity at which RTI Connext is currently logging "
            "diagnostic information in the given category."           
        )
        .def(
            "verbosity_by_category",
            (void (Logger::*)(LogCategory, Verbosity)) &Logger::verbosity_by_category,
            "Set the verbosity at which RTI Connext will log diagnostic "
            "information in the given category."
        )
        .def(
            "output_file",
            [](Logger& l, const std::string& filename) {
                l.output_file(filename.c_str());
            },
            "Set the name of the file to which the logged output is "
            "redirected."
        )
        .def(
            "output_file_set",
            [](Logger& l, const std::string& file_prefix, const std::string& file_suffix, int max_bytes, int max_files) {
                l.output_file_set(
                    file_prefix.c_str(),
                    file_suffix.c_str(),
                    max_bytes,
                    max_files);
            },
            py::arg("file_prefix"),
            py::arg("file_suffix"),
            py::arg("max_bytes"),
            py::arg_v("max_files", dds::core::LENGTH_UNLIMITED, "LENGTH_UNLIMITED"),
            "Set the name of the file to which the logged output is "
            "redirected."
        );
}

template<>
void process_inits<Logger>(py::module& m, ClassInitList& l) {
    auto verbosity = init_dds_safe_enum<Verbosity_def>(m, "Verbosity");

    py::enum_<Verbosity::type>(verbosity, "Verbosity")
        .value(
            "SILENT",
            Verbosity::type::SILENT,
            "No further output will be logged."
        )
        .value(
            "EXCEPTION",
            Verbosity::type::EXCEPTION,
            "Only error messages will be logged."
            "\n\n"
            "An error indicates something wrong in the functioning of RTI "
            "Connext. The most common cause of errors is incorrect "
            "configuration."
        )
        .value(
            "WARNING",
            Verbosity::type::WARNING,
            "Both error and warning messages will be logged."
            "\n\n"
            "A warning indicates that RTI Connext is taking an action that "
            "may or may not be what you intended. Some configuration "
            "information is also logged at this verbosity to aid in debugging."
        )
        .value(
            "STATUS_LOCAL",
            Verbosity::type::STATUS_LOCAL,
            "Errors, warnings, and verbose information about the lifecycles "
            "of local RTI Connext objects will be logged."
        )
        .value(
            "STATUS_REMOTE",
            Verbosity::type::STATUS_REMOTE,
            "Errors, warnings, and verbose information about the lifecycles "
            "of remote RTI Connext objects will be logged."
        )
        .value(
            "STATUS_ALL",
            Verbosity::type::STATUS_ALL,
            "Errors, warnings, verbose information about the lifecycles of "
            "local and remote RTI Connext objects, and periodic information "
            "about RTI Connext threads will be logged."
        )
        .export_values();

    auto log_category = init_dds_safe_enum<LogCategory_def>(m, "LogCategory");

    py::enum_<LogCategory::type>(log_category, "LogCategory")
        .value(
            "PLATFORM",
            LogCategory::type::PLATFORM,
            "Log messages pertaining to the underlying platform (hardware and "
            "OS) on which RTI Connext is running are in this category."
        )
        .value(
            "COMMUNICATION",
            LogCategory::type::COMMUNICATION,
            "Log messages pertaining to data serialization and "
            "deserialization and network traffic are in this category."
        )
        .value(
            "DATABASE",
            LogCategory::type::DATABASE,
            "Log messages pertaining to the internal database in which RTI "
            "Connext objects are stored are in this category."
        )
        .value(
            "ENTITIES",
            LogCategory::type::ENTITIES,
            "Log messages pertaining to local and remote entities and to the "
            "discovery process are in this category."
        )
        .value(
            "API",
            LogCategory::type::API,
            "Log messages pertaining to the API layer of RTI Connext (such as "
            "method argument validation) are in this category."
        )
        .value(
            "ALL_CATEGORIES",
            LogCategory::type::ALL_CATEGORIES,
            "Log messages pertaining to all category of RTI Connext."
        )
        .export_values();

    auto print_format = init_dds_safe_enum<PrintFormat_def>(m, "PrintFormat");

    py::enum_<PrintFormat::type>(print_format, "PrintFormat")
        .value(
            "DEFAULT",
            PrintFormat::type::DEFAULT,
            "Print message, method name, and activity context (default)."
        )
        .value(
            "TIMESTAMPED",
            PrintFormat::type::TIMESTAMPED,
            "Print message, method name, activity context, and timestamp."
        )
        .value(
            "VERBOSE",
            PrintFormat::type::VERBOSE,
            "Print message with all available context information (includes "
            "thread identifier, activity context)."
        )
        .value(
            "VERBOSE_TIMESTAMPED",
            PrintFormat::type::VERBOSE_TIMESTAMPED,
            "Print message with all available context information, and "
            "timestamp."
        )
        .value(
            "DEBUG",
            PrintFormat::type::DEBUG,
            "Print a set of field that may be useful for internal debug."
        )
        .value(
            "MINIMAL",
            PrintFormat::type::MINIMAL,
            "Print only message number and method name."
        )
        .value(
            "MAXIMAL",
            PrintFormat::type::MAXIMAL,
            "Print all available fields."
        )
        .export_values();

    l.push_back(
        [m]() mutable {
            return init_class<Logger>(m, "Logger");
        }
    );
}

}
