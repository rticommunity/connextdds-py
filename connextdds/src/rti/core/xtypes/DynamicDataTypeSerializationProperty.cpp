#include "PyConnext.hpp"
#include <rti/core/xtypes/DynamicDataProperty.hpp>

using namespace rti::core::xtypes;

template<>
void pyrti::init_class_defs(py::class_<DynamicDataTypeSerializationProperty>& cls) {
    cls
        .def(
            py::init<>(),
            "Construct with default settings."
        )
        .def(
            py::init<
                int32_t,
                int32_t,
                bool,
                bool
            >(),
            py::arg("max_serialized_size"),
            py::arg("min_serialized_size"),
            py::arg("trim_to_size"),
            py::arg("enable_fast_deserialization")
        )
        .def_property(
            "max_serialized_size",
            (uint32_t (DynamicDataTypeSerializationProperty::*)() const) &DynamicDataTypeSerializationProperty::max_size_serialized,
            (DynamicDataTypeSerializationProperty& (DynamicDataTypeSerializationProperty::*)(uint32_t)) &DynamicDataTypeSerializationProperty::max_size_serialized,
            "Initial buffer size."
        )
        .def_property(
            "min_serialized_size",
            (uint32_t (DynamicDataTypeSerializationProperty::*)() const) &DynamicDataTypeSerializationProperty::min_size_serialized,
            (DynamicDataTypeSerializationProperty& (DynamicDataTypeSerializationProperty::*)(uint32_t)) &DynamicDataTypeSerializationProperty::min_size_serialized,
            "Max buffer size."
        )
        .def_property(
            "trim_to_size",
            (bool (DynamicDataTypeSerializationProperty::*)() const) &DynamicDataTypeSerializationProperty::trim_to_size,
            (DynamicDataTypeSerializationProperty& (DynamicDataTypeSerializationProperty::*)(bool)) &DynamicDataTypeSerializationProperty::trim_to_size,
            "Max buffer size."
        )
        .def_property(
            "enable_fast_deserialization",
            (bool (DynamicDataTypeSerializationProperty::*)() const) &DynamicDataTypeSerializationProperty::enable_fast_deserialization,
            (DynamicDataTypeSerializationProperty& (DynamicDataTypeSerializationProperty::*)(bool)) &DynamicDataTypeSerializationProperty::enable_fast_deserialization,
            "Check buffer size."
        );
}

template<>
void pyrti::process_inits<DynamicDataTypeSerializationProperty>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<DynamicDataTypeSerializationProperty>(m, "DynamicDataTypeSerializationProperty");
        }
    );
}

