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
            .def_property_readonly_static(
                    "default",
                    [](py::object&) {
                        return PrintFormatProperty::Default();
                    },
                    "Create a new property with PrintFormatKind.DEFAULT.")
            .def_property_readonly_static(
                    "xml",
                    [](py::object&) {
                        return PrintFormatProperty::Xml();
                    },
                    "Create a new property with PrintFormatKind.XML.")
            .def_property_readonly_static(
                    "json",
                    [](py::object&) {
                        return PrintFormatProperty::Json();
                    },
                    "Create a new property with PrintFormatKind.JSON.");
}

template<>
void process_inits<PrintFormatProperty>(py::module& m, ClassInitList& l)
{
    init_dds_safe_enum<PrintFormatKind_def>(
            m,
            "PrintFormatKind",
            [](py::object& o) {
                py::enum_<PrintFormatKind::type>(o, "PrintFormatKind")
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
