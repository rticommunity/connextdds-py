#include "PyConnext.hpp"
#include <rti/core/AllocationSettings.hpp>
#include <rti/xcdr/Interpreter.hpp>

using namespace rti::core;

template<>
void pyrti::init_class_defs(py::class_<AllocationSettings>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates an instance with an initial, max and incremental count "
            "set to zero."
        )
        .def(
            py::init<
                int32_t,
                int32_t,
                int32_t
            >(),
            py::arg("initial_count"),
            py::arg("max_count"),
            py::arg("incremental_count"),
            "Creates an instance with the given initial, maximum and "
            "incremental values."
        )
        .def_property(
            "initial_count",
            (int32_t (AllocationSettings::*)() const) &AllocationSettings::initial_count,
            (AllocationSettings& (AllocationSettings::*)(int32_t)) &AllocationSettings::initial_count,
            "Initial count of resources."
        )
        .def_property(
            "max_count",
            (int32_t (AllocationSettings::*)() const) &AllocationSettings::max_count,
            (AllocationSettings& (AllocationSettings::*)(int32_t)) &AllocationSettings::max_count,
            "Max count of resources."
        )
        .def_property(
            "incremental_count",
            (int32_t (AllocationSettings::*)() const) &AllocationSettings::incremental_count,
            (AllocationSettings& (AllocationSettings::*)(int32_t)) &AllocationSettings::incremental_count,
            "Incremental count of resources."
        )
        .def_property_readonly_static(
            "AUTO_COUNT",
            []() {
                return AllocationSettings::AUTO_COUNT;
            },
            "A special value indicating that the quantity is derived from a "
            "different Qos value."
        );
}

template<>
void pyrti::process_inits<AllocationSettings>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<AllocationSettings>(m, "AllocationSettings");
        }
    );
}
