#include "PyConnext.hpp"
#include <rti/core/status/Status.hpp>

using namespace rti::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<DataWriterCacheStatus>& cls) {
    cls
        .def_property_readonly(
            "sample_count",
            &DataWriterCacheStatus::sample_count,
            "Number of samples in the DataWriter's queue, including "
            "unregister and dispose samples."
        )
        .def_property_readonly(
            "sample_count_peak",
            &DataWriterCacheStatus::sample_count_peak,
            "Highest number of samples in the writer's queue over the "
            "lifetime of the writer."
        );
}

template<>
void process_inits<DataWriterCacheStatus>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<DataWriterCacheStatus>(m, "DataWriterCacheStatus");
        }
    );
}

}
