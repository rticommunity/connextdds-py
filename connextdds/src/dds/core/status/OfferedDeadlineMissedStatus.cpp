#include "PyConnext.hpp"
#include <dds/core/status/Status.hpp>

using namespace dds::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<OfferedDeadlineMissedStatus>& cls) {
    cls
        .def_property_readonly(
            "total_count",
            &OfferedDeadlineMissedStatus::total_count,
            "The count of a DataWriter's failures to meet write deadlines."
        )
        .def_property_readonly(
            "total_change_count",
            &OfferedDeadlineMissedStatus::total_count_change,
            "The delta in total_count since the last time the listener was called or the status was read."
        )
        .def_property_readonly(
            "last_instance_handle",
            &OfferedDeadlineMissedStatus::last_instance_handle,
            "Handle to the last instance in the DataWriter for which an offered deadline was missed."
        );
}

template<>
void process_inits<OfferedDeadlineMissedStatus>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<OfferedDeadlineMissedStatus>(m, "OfferedDeadlineMissedStatus");
        }
    ); 
}

}
