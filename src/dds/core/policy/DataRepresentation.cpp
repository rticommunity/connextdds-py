#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

template<>
void pyrti::init_class_defs(py::class_<DataRepresentation>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates an instance with one element, AUTO_ID."
        )
        .def(
            py::init<
                const std::vector<int16_t>&
            >(),
            py::arg("representations"),
            "Creates an instance with a sequence of Data Representation IDs."
        )
        .def_property(
            "value",
            (std::vector<int16_t> (DataRepresentation::*)() const) &DataRepresentation::value,
            [](DataRepresentation& dr, std::vector<int16_t>& values) {
                return dr.value(values.begin(), values.end());
            },
            "A sequence of IDs representing the allowed data representations. "
            "\n\n"
            "This property uses copy semantics. Changes to the sequence are "
            "not reflected in the policy unless the sequence is commited back "
            "to the policy object via the property's setter."
        )
        .def_property_readonly_static(
            "XCDR",
            [](py::object&) {
                return DataRepresentation::xcdr();
            },
            "Extended Common Data Representation ID for encoding version 1."
        )
        .def_property_readonly_static(
            "XML",
            [](py::object&) {
                return DataRepresentation::xml();
            },
            "XML Data Representation ID (unsupported)."
        )
        .def_property_readonly_static(
            "XCDR2",
            [](py::object&) {
                return DataRepresentation::xcdr2();
            },
            "Extended Common Data Representation ID for encoding version 2."
        )
        .def_property_readonly_static(
            "AUTO_ID",
            [](py::object&) {
                return DataRepresentation::auto_id();
            },
            "ID that makes representation automatically chosen based on the "
            "type."
        )
        .def(
            py::self == py::self
        )
        .def(
            py::self != py::self
        );
}

template<>
void pyrti::process_inits<DataRepresentation>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<DataRepresentation>(
                m, "DataRepresentation");
        }
    );
}