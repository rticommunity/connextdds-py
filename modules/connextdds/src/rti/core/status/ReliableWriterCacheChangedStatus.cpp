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
void init_class_defs(py::class_<ReliableWriterCacheChangedStatus>& cls)
{
    cls.def_property_readonly(
               "empty_reliable_writer_cache",
               &ReliableWriterCacheChangedStatus::empty_reliable_writer_cache,
               "The number of times the reliable DataWriters's cache of "
               "unacknowledged samples has become empty.")
            .def_property_readonly(
                    "full_reliable_writer_cache",
                    &ReliableWriterCacheChangedStatus::
                            full_reliable_writer_cache,
                    "The number of times the reliable DataWriters's cache (or "
                    "send "
                    "window) of unacknowledged samples has become full.")
            .def_property_readonly(
                    "low_watermark_reliable_writer_cache",
                    &ReliableWriterCacheChangedStatus::
                            low_watermark_reliable_writer_cache,
                    "The number of times the reliable DataWriter's cache of "
                    "unacknowledged samples has fallen to the low watermark.")
            .def_property_readonly(
                    "high_watermark_reliable_writer_cache",
                    &ReliableWriterCacheChangedStatus::
                            high_watermark_reliable_writer_cache,
                    "The number of times the reliable DataWriter's cache of "
                    "unacknowledged samples has risen to the high watermark.")
            .def_property_readonly(
                    "unacknowledged_sample_count",
                    &ReliableWriterCacheChangedStatus::
                            unacknowledged_sample_count,
                    "The current number of unacknowledged samples in the "
                    "DataWriter's "
                    "cache.")
            .def_property_readonly(
                    "unacknowledged_sample_count_peak",
                    &ReliableWriterCacheChangedStatus::
                            unacknowledged_sample_count_peak,
                    "The highest value that unacknowledged_sample_count has "
                    "reached "
                    "until now.")
#if rti_connext_version_gte(6, 1, 0, 0)
            .def_property_readonly(
                    "replaced_unacknowledged_sample_count",
                    &ReliableWriterCacheChangedStatus::
                            replaced_unacknowledged_sample_count,
                    "The number of unacknowledged samples that have been "
                    "replaced in the writer's cache.")
#endif
            ;
}

template<>
void process_inits<ReliableWriterCacheChangedStatus>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<ReliableWriterCacheChangedStatus>(
                m,
                "ReliableWriterCacheChangedStatus");
    });
}

}  // namespace pyrti
