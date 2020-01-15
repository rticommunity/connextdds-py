#include "PyConnext.hpp"
#include <rti/core/xtypes/DynamicDataProperty.hpp>

using namespace rti::core::xtypes;

template<>
void pyrti::init_class_defs(py::class_<DynamicDataProperty>& cls) {
    cls
        .def(
            py::init<>(),
            "Construct with default settings."
        )
        .def(
            py::init<
                int32_t,
                int32_t,
                int32_t,
                bool
            >(),
            py::arg("initial_buffer_size"),
            py::arg("max_buffer_size"),
            py::arg("max_buffer_size_increment"),
            py::arg("check_buffer_size")
        )
        .def_property(
            "initial_buffer_size",
            (int32_t (DynamicDataProperty::*)() const) &DynamicDataProperty::buffer_initial_size,
            (DynamicDataProperty& (DynamicDataProperty::*)(int32_t)) &DynamicDataProperty::buffer_initial_size,
            "Initial buffer size."
        )
        .def_property(
            "max_buffer_size",
            (int32_t (DynamicDataProperty::*)() const) &DynamicDataProperty::buffer_max_size,
            (DynamicDataProperty& (DynamicDataProperty::*)(int32_t)) &DynamicDataProperty::buffer_max_size,
            "Max buffer size."
        )
        .def_property(
            "max_buffer_size_increment",
            (int32_t (DynamicDataProperty::*)() const) &DynamicDataProperty::buffer_max_size_increment,
            (DynamicDataProperty& (DynamicDataProperty::*)(int32_t)) &DynamicDataProperty::buffer_max_size_increment,
            "Max buffer size."
        )
        .def_property(
            "check_buffer_size",
            (bool (DynamicDataProperty::*)() const) &DynamicDataProperty::buffer_check_size,
            (DynamicDataProperty& (DynamicDataProperty::*)(bool)) &DynamicDataProperty::buffer_check_size,
            "Check buffer size."
        );
}

template<>
void pyrti::process_inits<DynamicDataProperty>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<DynamicDataProperty>(m, "DynamicDataProperty");
        }
    );
}

