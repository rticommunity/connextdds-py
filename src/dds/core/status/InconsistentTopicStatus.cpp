#include "PyConnext.hpp"
#include <dds/core/status/Status.hpp>

using namespace dds::core::status;

template<>
void pyrti::init_class_defs(py::class_<InconsistentTopicStatus>& cls) {
    cls
        .def_property_readonly(
            "total_count",
            &InconsistentTopicStatus::total_count,
            "Get the total count of pairs of DataReaders/DataWriters whose "
            "Topic names match but data types are inconsistent according to "
            " the current type consistency enforcement policy."
        )
        .def_property_readonly(
            "total_count_change",
            &InconsistentTopicStatus::total_count_change,
            "The delta number of inconsistent pairs of DataReaders/DataWriters "
            "for the Topic that have been discovered since the last time this "
            "status was read."
        );
}

template<>
void pyrti::process_inits<InconsistentTopicStatus>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<InconsistentTopicStatus>(m, "InconsistentTopicStatus");
        }
    ); 
}