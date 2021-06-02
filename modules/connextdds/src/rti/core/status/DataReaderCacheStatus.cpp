/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */

#include "PyConnext.hpp"
#include <rti/core/status/Status.hpp>

using namespace rti::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<DataReaderCacheStatus>& cls)
{
    cls.def_property_readonly(
               "sample_count",
               &DataReaderCacheStatus::sample_count,
               "The number of samples in the DataReader's queue.")
            .def_property_readonly(
                    "sample_count_peak",
                    &DataReaderCacheStatus::sample_count_peak,
                    "The highest number of samples in the DataReader's queue "
                    "over the "
                    "lifetime of the reader.");
#if rti_connext_version_lt(6, 1, 0)
#if rti_connext_version_gte(6, 0, 0)
        cls.def_property_readonly(
                    "instance_count",
                    [](const DataReaderCacheStatus& cs) {
                        return cs.native().instance_count;
                    },
                    "The number of instances tracked by this DataReader.")
            .def_property_readonly(
                    "instance_count_peak",
                    [](const DataReaderCacheStatus& cs) {
                        return cs.native().instance_count_peak;
                    },
                    "The highest number of instances for the DataReader "
                    "over the "
                    "lifetime of the reader.");
#endif
#else
    cls.def_property_readonly(
                    "old_source_timestamp_dropped_sample_count",
                    &DataReaderCacheStatus::old_source_timestamp_dropped_sample_count,
                    "The number of samples dropped as a result of receiving a sample "
                    "older than the last one, using DestinationOrderKind.BY_SOURCE_TIMESTAMP.")
            .def_property_readonly(
                    "tolerance_source_timestamp_dropped_sample_count",
                    &DataReaderCacheStatus::tolerance_source_timestamp_dropped_sample_count,
                    "The number of samples dropped as a result of receiving a sample "
                    "in the future, using DestinationOrderKind.BY_SOURCE_TIMESTAMP.")
            .def_property_readonly(
                    "ownership_dropped_sample_count",
                    &DataReaderCacheStatus::ownership_dropped_sample_count,
                    "The number of samples dropped as a result of receiving a sample from a "
                    "DataWriter with a lower strength, using Exclusive Ownership.")
            .def_property_readonly(
                    "content_filter_dropped_sample_count",
                    &DataReaderCacheStatus::content_filter_dropped_sample_count,
                    "The number of user samples filtered by the DataReader due to "
                    "Content-Filtered Topics.")
            .def_property_readonly(
                    "time_based_filter_dropped_sample_count",
                    &DataReaderCacheStatus::time_based_filter_dropped_sample_count,
                    "The number of user samples filtered by the DataReader due to "
                    "TimeBasedFilter QoS.")
            .def_property_readonly(
                    "expired_dropped_sample_count",
                    &DataReaderCacheStatus::expired_dropped_sample_count,
                    "The number of samples expired by the DataReader due to Lifespan "
                    "QoS or the autopurge sample delays.")
            .def_property_readonly(
                    "virtual_duplicate_dropped_sample_count",
                    &DataReaderCacheStatus::virtual_duplicate_dropped_sample_count,
                    "The number of virtual duplicate samples dropped by the DataReader. "
                    "A sample is a virtual duplicate if it has the same identity "
                    "(Virtual Writer GUID and Virtual Sequence Number) as a previously "
                    "received sample.")
            .def_property_readonly(
                    "replaced_dropped_sample_count",
                    &DataReaderCacheStatus::replaced_dropped_sample_count,
                    "The number of samples replaced by the DataReader due to "
                    "HistoryKind.KEEP_LAST replacement.")
            .def_property_readonly(
                    "writer_removed_batch_sample_dropped_sample_count",
                    &DataReaderCacheStatus::writer_removed_batch_sample_dropped_sample_count,
                    "The number of batch samples received by the DataReader that "
                    "were marked as removed by the DataWriter.")
            .def_property_readonly(
                    "total_samples_dropped_by_instance_replacement",
                    &DataReaderCacheStatus::total_samples_dropped_by_instance_replacement,
                    "The number of samples with sample state SampleState.NOT_READ "
                    "that were dropped when removing an instance due to instance "
                    "replacement. See DataReaderResourceLimits.instance_replacement "
                    "for more details about when instances are replaced.")
            .def_property_readonly(
                    "alive_instance_count",
                    &DataReaderCacheStatus::alive_instance_count,
                    "The number of instances in the DataReader's queue with an "
                    "instance state equal to InstanceState.ALIVE.")
            .def_property_readonly(
                    "alive_instance_count_peak",
                    &DataReaderCacheStatus::alive_instance_count_peak,
                    "The highest value of DataReaderCacheStatus.alive_instance_count "
                    "over the lifetime of the DataReader.")
            .def_property_readonly(
                    "no_writers_instance_count",
                    &DataReaderCacheStatus::no_writers_instance_count,
                    "The number of instances in the DataReader's queue with an "
                    "instance state equal to InstanceState.NOT_ALIVE_NO_WRITERS.")
            .def_property_readonly(
                    "no_writers_instance_count_peak",
                    &DataReaderCacheStatus::no_writers_instance_count_peak,
                    "The highest value of DataReaderCacheStatus.no_writers_instance_count "
                    "over the lifetime of the DataReader.")
            .def_property_readonly(
                    "disposed_instance_count",
                    &DataReaderCacheStatus::disposed_instance_count,
                    "The number of instances in the DataReader's queue with an instance "
                    "state equal to InstanceState.NOT_ALIVE_DISPOSED.")
            .def_property_readonly(
                    "disposed_instance_count_peak",
                    &DataReaderCacheStatus::disposed_instance_count_peak,
                    "The number of minimal instance states currently being maintained "
                    "in the DataReader's queue.")
            .def_property_readonly(
                    "detached_instance_count",
                    &DataReaderCacheStatus::detached_instance_count,
                    "The number of instances in the DataReader's queue with an instance "
                    "state equal to InstanceState.NOT_ALIVE_DISPOSED.")
            .def_property_readonly(
                    "detached_instance_count_peak",
                    &DataReaderCacheStatus::detached_instance_count_peak,
                    "The highest value of DataReaderCacheStatus.detached_instance_count over "
                    "the lifetime of the DataReader.")
            .def_property_readonly(
                    "compressed_sample_count",
                    &DataReaderCacheStatus::compressed_sample_count,
                    "The number of received compressed samples.");
#endif
}

template<>
void process_inits<DataReaderCacheStatus>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DataReaderCacheStatus>(m, "DataReaderCacheStatus");
    });
}

}  // namespace pyrti
