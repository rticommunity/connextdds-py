#include "PyConnext.hpp"
#include <dds/core/status/Status.hpp>

using namespace dds::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<RequestedDeadlineMissedStatus>& cls)
{
    cls.def_property_readonly(
               "total_count",
               &RequestedDeadlineMissedStatus::total_count,
               "Total count of the missed deadlines detected for any instance "
               "read by the DataReader.")
            .def_property_readonly(
                    "total_count_change",
                    &RequestedDeadlineMissedStatus::total_count_change,
                    "The delta in missed deadlines detected since the last "
                    "time the listener was called or the status was read.")
            .def_property_readonly(
                    "last_instance_handle",
                    &RequestedDeadlineMissedStatus::last_instance_handle,
                    "Handle to the last instance in the DataReader for which a "
                    "missed deadline was detected.");
}

template<>
void process_inits<RequestedDeadlineMissedStatus>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<RequestedDeadlineMissedStatus>(
                m,
                "RequestedDeadlineMissedStatus");
    });
}

}  // namespace pyrti
