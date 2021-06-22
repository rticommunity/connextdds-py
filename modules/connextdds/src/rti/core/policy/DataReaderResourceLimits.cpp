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
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<DataReaderResourceLimits>& cls)
{
    cls.def(py::init<>(),
            "Create a default DataReaderResourceLimits policy with default "
            "settings.")
            .def_property(
                    "max_remote_writers",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::max_remote_writers,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::max_remote_writers,
                    "The maximum number of remote writers from which a "
                    "DataReader "
                    "may read, including all instances.")
            .def_property(
                    "max_remote_writers_per_instance",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    max_remote_writers_per_instance,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::
                                    max_remote_writers_per_instance,
                    "The maximum number of remote writers from which a "
                    "DataReader"
                    "may read a single instance.")
            .def_property(
                    "max_samples_per_remote_writer",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    max_samples_per_remote_writer,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::
                                    max_samples_per_remote_writer,
                    "The maximum number of out-of-order samples from a given "
                    "remote "
                    "DataWriter that a DataReader may store when maintaining a "
                    "reliable connection to the DataWriter.")
            .def_property(
                    "max_infos",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::max_infos,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::max_infos,
                    "The maximum number of info units that a DataReader can "
                    "use to "
                    "store SampleInfo.")
            .def_property(
                    "initial_remote_writers_per_instance",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    initial_remote_writers_per_instance,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::
                                    initial_remote_writers_per_instance,
                    "The initial number of remote writers from which a "
                    "DataReader "
                    "may read a single instance.")
            .def_property(
                    "initial_remote_writers",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::initial_remote_writers,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::initial_remote_writers,
                    "The initial number of remote writers from which a "
                    "DataReader "
                    "may read, including all instances.")
            .def_property(
                    "initial_infos",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::initial_infos,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::initial_infos,
                    "The initial number of info units that a DataReader can "
                    "use to "
                    "store SampleInfo.")
            .def_property(
                    "initial_outstanding_reads",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    initial_outstanding_reads,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::
                                    initial_outstanding_reads,
                    "The initial number of outstanding calls to read/take (or "
                    "one of "
                    "their variants) on the same DataReader for which memory "
                    "has not "
                    "been returned by calling LoanedSamples.return_loan().")
            .def_property(
                    "max_outstanding_reads",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::max_outstanding_reads,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::max_outstanding_reads,
                    "The max number of outstanding calls to read/take (or one "
                    "of "
                    "their variants) on the same DataReader for which memory "
                    "has not "
                    "been returned by calling LoanedSamples.return_loan().")
            .def_property(
                    "max_samples_per_read",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::max_samples_per_read,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::max_samples_per_read,
                    "The maximum number of data samples that the application "
                    "can "
                    "receive from the middleware in a single call to "
                    "DataReader.read() or DataReader.take(). If more data "
                    "exists in "
                    "the middleware, the application will need to issue "
                    "multiple "
                    "read/take calls.")
            .def_property(
                    "disable_fragmentation_support",
                    (bool (DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    disable_fragmentation_support,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (bool) )
                            & DataReaderResourceLimits::
                                    disable_fragmentation_support,
                    "Determines whether the DataReader can receive fragmented "
                    "samples.")
            .def_property(
                    "max_fragmented_samples",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::max_fragmented_samples,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::max_fragmented_samples,
                    "The maximum number of samples for which the DataReader "
                    "may "
                    "store fragments at a given point in time.")
            .def_property(
                    "initial_fragmented_samples",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    initial_fragmented_samples,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::
                                    initial_fragmented_samples,
                    "The initial number of samples for which a DataReader may "
                    "store "
                    "fragments.")
            .def_property(
                    "max_fragmented_samples_per_remote_writer",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    max_fragmented_samples_per_remote_writer,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::
                                    max_fragmented_samples_per_remote_writer,
                    "The maximum number of samples per remote writer for which "
                    "a "
                    "DataReader may store fragments.")
            .def_property(
                    "max_fragments_per_sample",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    max_fragments_per_sample,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::
                                    max_fragments_per_sample,
                    "Maximum number of fragments for a single sample.")
            .def_property(
                    "dynamically_allocate_fragmented_samples",
                    (bool (DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    dynamically_allocate_fragmented_samples,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (bool) )
                            & DataReaderResourceLimits::
                                    dynamically_allocate_fragmented_samples,
                    "Determines whether the DataReader pre-allocates storage "
                    "for "
                    "storing fragmented samples.")
            .def_property(
                    "max_total_instances",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::max_total_instances,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::max_total_instances,
                    "Maximum number of instances for which a DataReader will "
                    "keep "
                    "state.")
            .def_property(
                    "max_remote_virtual_writers",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    max_remote_virtual_writers,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::
                                    max_remote_virtual_writers,
                    "The maximum number of remote virtual writers from which a "
                    "DataReader may read, including all instances.")
            .def_property(
                    "initial_remote_virtual_writers",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    initial_remote_virtual_writers,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::
                                    initial_remote_virtual_writers,
                    "The initial number of remote virtual writers from which a "
                    "DataReader may read, including all instances.")
            .def_property(
                    "max_remote_virtual_writers_per_instance",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    max_remote_virtual_writers_per_instance,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::
                                    max_remote_virtual_writers_per_instance,
                    "The maximum number of virtual remote writers that can be "
                    "associated with an instance.")
            .def_property(
                    "initial_remote_virtual_writers_per_instance",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    initial_remote_virtual_writers_per_instance,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::
                                    initial_remote_virtual_writers_per_instance,
                    "The initial number of virtual remote writers that can be "
                    "associated with an instance.")
            .def_property(
                    "max_remote_writers_per_sample",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    max_remote_writers_per_sample,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::
                                    max_remote_writers_per_sample,
                    "The maximum number of remote writers allowed to write the "
                    "same "
                    "sample.")
            .def_property(
                    "max_query_condition_filters",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    max_query_condition_filters,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::
                                    max_query_condition_filters,
                    "The maximum number of query condition filters a reader is "
                    "allowed.")
            .def_property(
                    "max_app_ack_response_length",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    max_app_ack_response_length,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::
                                    max_app_ack_response_length,
                    "Maximum length of application-level acknowledgment "
                    "response data.")
            .def_property(
                    "keep_minimum_state_for_instances",
                    (bool (DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::
                                    keep_minimum_state_for_instances,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (bool) )
                            & DataReaderResourceLimits::
                                    keep_minimum_state_for_instances,
                    "Whether or not keep a minimum instance state for up to "
                    "DataReaderResourceLimits.max_total_instances.")
            .def_property(
                    "initial_topic_queries",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::initial_topic_queries,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::initial_topic_queries,
                    "The initial number of TopicQueries allocated by a "
                    "DataReader.")
            .def_property(
                    "max_topic_queries",
                    (int32_t(DataReaderResourceLimits::*)() const)
                            & DataReaderResourceLimits::max_topic_queries,
                    (DataReaderResourceLimits
                     & (DataReaderResourceLimits::*) (int32_t))
                            & DataReaderResourceLimits::max_topic_queries,
                    "The maximum number of TopicQueries allocated by a "
                    "DataReader.")
#if rti_connext_version_gte(6, 0, 0)
            .def_property(
                    "shmem_ref_transfer_mode_attached_segment_allocation",
                    (AllocationSettings & (DataReaderResourceLimits::*) ())
                            & DataReaderResourceLimits::
                                    shmem_ref_transfer_mode_attached_segment_allocation,
                    [](DataReaderResourceLimits& drrl, AllocationSettings& as) {
                        drrl.shmem_ref_transfer_mode_attached_segment_allocation() =
                                as;
                    },
                    "The initial number of TopicQueries allocated by a "
                    "DataReader.")
#endif
#if rti_connext_version_gte(6, 1, 0)
#ifndef _MSC_VER
            .def_property(
                    "instance_replacement",
                    (DataReaderResourceLimitsInstanceReplacementSettings& (DataReaderResourceLimits::*) ())
                            & DataReaderResourceLimits::
                                    instance_replacement,
                    [](DataReaderResourceLimits& drrl, DataReaderResourceLimitsInstanceReplacementSettings& ir) {
                        drrl.instance_replacement() = ir;
                    },
                    "The instance replacement policy.")
#endif
#endif
            .def(py::self == py::self)
            .def(py::self != py::self);
}

template<>
void process_inits<DataReaderResourceLimits>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DataReaderResourceLimits>(
                m,
                "DataReaderResourceLimits");
    });
}

}  // namespace pyrti
