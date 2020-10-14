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
#include <rti/core/SampleFlag.hpp>
#include "PyMaskType.hpp"

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<SampleFlag>& cls)
{
    cls.def(
            "__str__",
            [](const SampleFlag& s) {
                switch(s.to_ullong()) {
                case DDS_REDELIVERED_SAMPLE:
                    return std::string("SampleFlag.REDELIVERED");
                case DDS_INTERMEDIATE_REPLY_SEQUENCE_SAMPLE:
                    return std::string("SampleFlag.INTERMEDIATE_REPLY_SEQUENCE");
                case DDS_REPLICATE_SAMPLE:
                    return std::string("SampleFlag.REPLICATE");
                case DDS_LAST_SHARED_READER_QUEUE_SAMPLE:
                    return std::string("SampleFlag.LAST_SHARED_READER_QUEUE");
                case DDS_INTERMEDIATE_TOPIC_QUERY_SAMPLE:
                    return std::string("SampleFlag.INTERMEDIATE_TOPIC_QUERY");
                default:
                    return std::string("SampleFlag.INVALID");
                }
            })
            .def_static(
                    "redelivered",
                    &SampleFlag::redelivered,
                    "Indicates that a sample has been redelivered by RTI Queuing "
                    "Service.")
            .def_static(
                    "intermediate_reply_sequence",
                    &SampleFlag::intermediate_reply_sequence,
                    "Indicates that a response sample is not the last response "
                    "sample "
                    "for a given request. This bit is usually set by Connext "
                    "Repliers "
                    "sending multiple responses for a request.")
            .def_static(
                    "replicate",
                    &SampleFlag::replicate,
                    "Indicates if a sample must be broadcast by one RTI "
                    "Queuing "
                    "Service replica to other replicas.")
            .def_static(
                    "last_shared_reader_queue",
                    &SampleFlag::last_shared_reader_queue,
                    "Indicates that a sample is the last sample in a "
                    "SharedReaderQueue for a QueueConsumer DataReader.")
            .def_static(
                    "intermediate_topic_query_sample",
                    &SampleFlag::intermediate_topic_query_sample,
                    "Indicates that a sample for a TopicQuery will be followed "
                    "by "
                    "more samples.");
}

template<>
void process_inits<SampleFlag>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        auto cls = init_mask_type<SampleFlag, uint64_t>(
                m,
                "SampleFlag",
                "Construct an empty SampleFlag with no bits set.",
                false);
        return [cls]() mutable { init_class_defs<SampleFlag>(cls); };
    });
}

}  // namespace pyrti
