#include "PyConnext.hpp"
#include <dds/topic/Filter.hpp>

using namespace dds::topic;

template<>
void pyrti::init_class_defs(py::class_<Filter>& cls) {
    cls
        .def(
            py::init<const std::string&>(),
            py::arg("expression"),
            "Create a Filter with the specified expression."
        )
        .def(
            py::init<const std::string&, const std::vector<std::string>&>(),
            py::arg("expression"),
            py::arg("parameters"),
            "Create a Filter with the specified expression and parameters."
        )
        .def_property_readonly(
            "expression",
            &Filter::expression,
            "Get the Filter's expression."
        )
        .def(
            "__iter__",
            [](Filter& f) {
                return py::make_iterator(f.begin(), f.end());
            },
            py::keep_alive<0, 1>()
        )
        .def(
            "parameters",
            [](Filter& f, std::vector<std::string>& params) {
                f.parameters(params.begin(), params.end());
            },
            "Set the parameters for the Filter."
        )
        .def(
            "add_parameters",
            &Filter::add_parameter,
            py::arg("parameter"),
            "Append a parameter to the Filter."
        )
        .def_property_readonly(
            "parameter_count",
            &Filter::parameters_length,
            "Get the number of parameters."
        )
        .def_property(
            "name",
            [](const Filter& f) {
                return f->name();
            },
            [](Filter& f, std::string& name) {
                return f->name(name);
            },
            "The name of the type of Filter."
        )
        .def_property_readonly_static(
            "sql_filter_name",
            &rti::topic::sql_filter_name,
            "The name of the SQL filter."
        )
        .def_property_readonly_static(
            "stringmatch_filter_name",
            &rti::topic::stringmatch_filter_name,
            "The name of the string match filter."
        );
}

template<>
void pyrti::process_inits<Filter>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<Filter>(m, "Filter");
        }
    );
}
