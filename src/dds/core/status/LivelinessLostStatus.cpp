#include "PyConnext.hpp"
#include <dds/core/status/Status.hpp>

using namespace dds::core::status;

template<>
void pyrti::init_class_defs(py::class_<LivelinessLostStatus>& cls) {
    cls
        .def_property_readonly(
            "total_count",
            &LivelinessLostStatus::total_count,
            "Total count of times that a previously alive DataWriter became "
            "not alive due to a failure to assert its liveliness signal "
            "within the agreed lease duration."
        )
        .def_property_readonly(
            "total_count_change",
            &LivelinessLostStatus::total_count_change,
            "The delta number of times liveliness has been lost since the "
            "last time the listener callback fired or this status was "
            "checked."
        );
}

template<>
void pyrti::process_inits<LivelinessLostStatus>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<LivelinessLostStatus>(m, "LivelinessLostStatus");
        }
    ); 
}