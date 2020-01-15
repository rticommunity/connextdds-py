#include "PyConnext.hpp"
#include <rti/core/status/Status.hpp>

using namespace rti::core::status;

template<>
void pyrti::init_class_defs(py::class_<DataReaderCacheStatus>& cls) {
    cls
        .def_property_readonly(
            "sample_count",
            &DataReaderCacheStatus::sample_count,
            "The number of samples in the DataReader's queue."
        )
        .def_property_readonly(
            "sample_count_peak",
            &DataReaderCacheStatus::sample_count_peak,
            "The highest number of samples in the DataReader's queue over the "
            "lifetime of the reader."
        );
}

template<>
void pyrti::process_inits<DataReaderCacheStatus>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<DataReaderCacheStatus>(m, "DataReaderCacheStatus");
        }
    );
}