#include "PyConnext.hpp"
#include <rti/topic/PrintFormat.hpp>
#include <PySafeEnum.hpp>

using namespace rti::topic;

namespace pyrti {

template<>
void init_class_defs(py::class_<PrintFormatProperty>& cls)
{
    cls.def(py::init<PrintFormatKind, bool, bool, bool>(),
            py::arg_v(
                    "kind",
                    PrintFormatKind::DEFAULT,
                    "PrintFormatKind.DEFAULT"),
            py::arg("is_pretty_print") = true,
            py::arg("is_enum_as_int") = false,
            py::arg("is_include_root_elements") = true,
            "Initalize PrintFormatProperty object.")
            .def_property(
                    "kind",
                    (PrintFormatKind(PrintFormatProperty::*)() const)
                            & PrintFormatProperty::kind,
                    (PrintFormatProperty
                     & (PrintFormatProperty::*) (PrintFormatKind))
                            & PrintFormatProperty::kind,
                    "Get/set the kind value.")
            .def_property(
                    "pretty_print",
                    (bool (PrintFormatProperty::*)() const)
                            & PrintFormatProperty::pretty_print,
                    (PrintFormatProperty & (PrintFormatProperty::*) (bool) )
                            & PrintFormatProperty::pretty_print,
                    "Get/set the value of pretty_print.")
            .def_property(
                    "enum_as_int",
                    (bool (PrintFormatProperty::*)() const)
                            & PrintFormatProperty::enum_as_int,
                    (PrintFormatProperty & (PrintFormatProperty::*) (bool) )
                            & PrintFormatProperty::enum_as_int,
                    "Get/set the value of enum_as_int.")
            .def_property(
                    "include_root_elements",
                    (bool (PrintFormatProperty::*)() const)
                            & PrintFormatProperty::include_root_elements,
                    (PrintFormatProperty & (PrintFormatProperty::*) (bool) )
                            & PrintFormatProperty::include_root_elements,
                    "Get/set the value of include_root_elements.")
            .def_static(
                    "default",
                    &PrintFormatProperty::Default,
                    "Create a new property with PrintFormatKind.DEFAULT.")
            .def_static(
                    "xml",
                    &PrintFormatProperty::Xml,
                    "Create a new property with PrintFormatKind.XML.")
            .def_static(
                    "json",
                    &PrintFormatProperty::Json,
                    "Create a new property with PrintFormatKind.JSON.");
}

template<>
void process_inits<PrintFormatProperty>(py::module& m, ClassInitList& l)
{
    init_dds_safe_enum<PrintFormatKind_def>(
            m,
            "PrintFormatKind",
            [](py::object& o) {
                py::enum_<PrintFormatKind::type>(o, "Enum")
                        .value("DEFAULT",
                               PrintFormatKind::type::DEFAULT,
                               "Use a default format specific to RTI Connext "
                               "to represent the data when converting a data "
                               "sample to a string.")
                        .value("XML",
                               PrintFormatKind::type::XML,
                               "Use an XML format to represent the data when "
                               "converting a data sample to a string.")
                        .value("JSON",
                               PrintFormatKind::type::JSON,
                               "Use an JSON format to represent the data when "
                               "converting a data sample to a string.")
                        .export_values();
            });

    l.push_back([m]() mutable {
        return init_class<PrintFormatProperty>(m, "PrintFormatProperty");
    });
}

}  // namespace pyrti
