#include "PyConnext.hpp"
#include <rti/core/cond/WaitSetImpl.hpp>
#include <dds/core/Duration.hpp>

using namespace rti::core::cond;

namespace pyrti {

template<>
void init_class_defs(py::class_<WaitSetProperty>& cls) {
    cls
        .def(
            py::init<>(),
            "Constructs a WaitSetProperty with default values."
        )
        .def(
            py::init<int32_t, const dds::core::Duration&>(),
            py::arg("event_count"),
            py::arg("event_delay"),
            "Construct a WaitSetProperty with the given settings."
        )
        .def_property(
            "event_count",
            (int32_t (WaitSetProperty::*)() const) &WaitSetProperty::max_event_count,
            (WaitSetProperty& (WaitSetProperty::*)(int32_t)) &WaitSetProperty::max_event_count,
            "Get/set the event count that will wake the WaitSet."
        )
        .def_property(
            "event_delay",
            (dds::core::Duration (WaitSetProperty::*)() const) &WaitSetProperty::max_event_delay,
            (WaitSetProperty& (WaitSetProperty::*)(const dds::core::Duration&)) &WaitSetProperty::max_event_delay,
            "Get/set the delay that will cause the WaitSet to wake even if the event count is not hit."
        );
}

template<>
void process_inits<WaitSetProperty>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<WaitSetProperty>(m, "WaitSetProperty");
        }
    );
}

}
