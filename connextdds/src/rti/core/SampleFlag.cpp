#include "PyConnext.hpp"
#include "PyMaskType.hpp"
#include <rti/core/SampleFlag.hpp>

using namespace rti::core;

namespace pyrti {

template <> void init_class_defs(py::class_<SampleFlag> &cls) {
  cls.def_static("redelivered", &SampleFlag::redelivered,
                 "Indicates that a sample has been redelivered by RTI Queuing "
                 "Service.")
      .def_static(
          "intermediate_reply_sequence",
          &SampleFlag::intermediate_reply_sequence,
          "Indicates that a response sample is not the last response sample "
          "for a given request. This bit is usually set by Connext Repliers "
          "sending multiple responses for a request.")
      .def_static("replicate", &SampleFlag::replicate,
                  "Indicates if a sample must be broadcast by one RTI Queuing "
                  "Service replica to other replicas.")
      .def_static("last_shared_reader_queue",
                  &SampleFlag::last_shared_reader_queue,
                  "Indicates that a sample is the last sample in a "
                  "SharedReaderQueue for a QueueConsumer DataReader.")
      .def_static(
          "intermediate_topic_query_sample",
          &SampleFlag::intermediate_topic_query_sample,
          "Indicates that a sample for a TopicQuery will be followed by "
          "more samples.");
}

template <> void process_inits<SampleFlag>(py::module &m, ClassInitList &l) {
  l.push_back([m]() mutable {
    auto cls = init_mask_type<SampleFlag, uint64_t>(
        m, "SampleFlag", "Construct an empty SampleFlag with no bits set.");
    return [cls]() mutable { init_class_defs<SampleFlag>(cls); };
  });
}

} // namespace pyrti
