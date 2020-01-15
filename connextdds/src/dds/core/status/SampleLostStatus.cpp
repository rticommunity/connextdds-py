#include "PyConnext.hpp"
#include <dds/core/status/Status.hpp>

using namespace dds::core::status;

template<>
void pyrti::init_class_defs(py::class_<SampleLostStatus>& cls) {
    cls
        .def_property_readonly(
            "total_count",
            &SampleLostStatus::total_count,
            "Total count of all samples lost across all instances of data published under the Topic."
        )
        .def_property_readonly(
            "total_count_change",
            &SampleLostStatus::total_count_change,
            "The incremental number of samples lost since the last time the listener was called or the status was read."
        )
        .def_property_readonly(
            "last_reason",
            [](const SampleLostStatus& sls) {
                return sls->last_reason();
            },
            "The reason for the most recent sample loss."
        );
}

template<>
void pyrti::process_inits<SampleLostStatus>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<SampleLostStatus>(m, "SampleLostStatus");
        }
    ); 
}