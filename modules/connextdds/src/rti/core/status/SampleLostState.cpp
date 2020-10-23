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
#include "PyMaskType.hpp"

using namespace rti::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<SampleLostState>& cls)
{
    cls.def(
            "__str__",
            [](const SampleLostState& s) {
                switch(s.to_ullong()) {
                case DDS_NOT_LOST:
                    return std::string("SampleLostState.NOT_LOST");
                case DDS_LOST_BY_WRITER:
                    return std::string("SampleLostState.LOST_BY_WRITER");
                case DDS_LOST_BY_INSTANCES_LIMIT:
                    return std::string("SampleLostState.LOST_BY_INSTANCES_LIMIT");
                case DDS_LOST_BY_REMOTE_WRITERS_PER_INSTANCE_LIMIT:
                    return std::string("SampleLostState.LOST_BY_REMOTE_WRITERS_PER_INSTANCE_LIMIT");
                case DDS_LOST_BY_INCOMPLETE_COHERENT_SET:
                    return std::string("SampleLostState.LOST_BY_INCOMPLETE_COHERENT_SET");
                case DDS_LOST_BY_LARGE_COHERENT_SET:
                    return std::string("SampleLostState.LOST_BY_LARGE_COHERENT_SET");
                case DDS_LOST_BY_SAMPLES_PER_REMOTE_WRITER_LIMIT:
                    return std::string("SampleLostState.LOST_BY_SAMPLES_PER_REMOTE_WRITER_LIMIT");
                case DDS_LOST_BY_VIRTUAL_WRITERS_LIMIT:
                    return std::string("SampleLostState.LOST_BY_VIRTUAL_WRITERS_LIMIT");
                case DDS_LOST_BY_REMOTE_WRITERS_PER_SAMPLE_LIMIT:
                    return std::string("SampleLostState.LOST_BY_REMOTE_WRITERS_PER_SAMPLE_LIMIT");
                case DDS_LOST_BY_AVAILABILITY_WAITING_TIME:
                    return std::string("SampleLostState.LOST_BY_AVAILABILITY_WAITING_TIME");
                case DDS_LOST_BY_REMOTE_WRITER_SAMPLES_PER_VIRTUAL_QUEUE_LIMIT:
                    return std::string("SampleLostState.LOST_BY_REMOTE_WRITER_SAMPLES_PER_VIRTUAL_QUEUE_LIMIT");
                case DDS_LOST_BY_OUT_OF_MEMORY:
                    return std::string("SampleLostState.LOST_BY_OUT_OF_MEMORY");
#if rti_connext_version_gte(6, 0, 0)
                case DDS_LOST_BY_UNKNOWN_INSTANCE:
                    return std::string("SampleLostState.LOST_BY_UNKNOWN_INSTANCE");
#endif
                default:
                    return std::string("SampleLostState.INVALID");
                }
            })
            .def_static(
                    "not_lost",
                    &SampleLostState::not_lost,
                    "Create a SampleLostState indicating that the sample was not "
                    "lost.")
            .def_static(
                    "lost_by_writer",
                    &SampleLostState::lost_by_writer,
                    "Create a SampleLostState indicating that a DataWriter "
                    "removed "
                    "the sample before being received by the DataReader.")
            .def_static(
                    "lost_by_instances_limit",
                    &SampleLostState::lost_by_instances_limit,
                    "Create a SampleLostState indicating that the sample was "
                    "lost "
                    "because the resource limit on the number of instances was "
                    "reached.")
            .def_static(
                    "lost_by_remote_writers_per_instance_limit",
                    &SampleLostState::lost_by_remote_writers_per_instance_limit,
                    "Create a SampleLostState indicating that the sample was "
                    "lost "
                    "because the resource limit on the number of remote "
                    "writers for "
                    "a single instance from which a DataReader may read was "
                    "reached.")
            .def_static(
                    "lost_by_incomplete_coherent_set",
                    &SampleLostState::lost_by_incomplete_coherent_set,
                    "Create a SampleLostState indicating that the sample was "
                    "lost "
                    "because it is part of an incomplete coherent set.")
            .def_static(
                    "lost_by_large_coherent_set",
                    &SampleLostState::lost_by_large_coherent_set,
                    "Create a SampleLostState indicating that the sample was "
                    "lost "
                    "because it is part of a large coherent set.")
            .def_static(
                    "lost_by_samples_per_remote_writer_limit",
                    &SampleLostState::lost_by_samples_per_remote_writer_limit,
                    "Create a SampleLostState indicating that the sample was "
                    "lost "
                    "because the resource limit on the number of samples from "
                    "a given "
                    "remote writer that a DataReader may store was reached.")
            .def_static(
                    "lost_by_virtual_writers_limit",
                    &SampleLostState::lost_by_virtual_writers_limit,
                    "Create a SampleLostState indicating that the resource "
                    "limit on "
                    "the number of virtual writers from which a DataReader may "
                    "read "
                    "was reached.")
            .def_static(
                    "lost_by_remote_writers_per_sample_limit",
                    &SampleLostState::lost_by_remote_writers_per_sample_limit,
                    "Create a SampleLostState indicating that the resource "
                    "limit on "
                    "the number of remote writers per sample was reached.")
            .def_static(
                    "lost_by_availability_waiting_time",
                    &SampleLostState::lost_by_availability_waiting_time,
                    "Create a SampleLostState indicating that the sample was "
                    "lost "
                    "because the max_data_availability_waiting_time expired.")
            .def_static(
                    "lost_by_remote_writers_per_virtual_queue_limit",
                    &SampleLostState::
                            lost_by_remote_writers_per_virtual_queue_limit,
                    "Create a SampleLostState indicating that the sample was "
                    "lost "
                    "because the resource limit on the number of samples "
                    "published "
                    "by a remote writer on behalf of a virtual writer that a "
                    "DataReader may store was reached.")
#if rti_connext_version_gte(6, 0, 0)
            .def_static(
                    "lost_by_unknown_instance",
                    []() {
                        auto retval = SampleLostState::not_lost();
                        retval |= DDS_LOST_BY_UNKNOWN_INSTANCE;
                        return retval;
                    },
                    "Create a SampleLostState indicating that the sample was "
                    "lost because the instance is unknown")
#endif
            .def_static(
                    "lost_by_out_of_memory",
                    &SampleLostState::lost_by_out_of_memory,
                    "Create a SampleLostState indicating that the sample was "
                    "lost "
                    "because there was insufficent memory available to store "
                    "the "
                    "sample data.");
}

template<>
void process_inits<SampleLostState>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        auto cls = init_mask_type_no_int_constructor<SampleLostState>(
                m,
                "SampleLostState",
                "Creates SampleLostState.not_lost()",
                false);
        return [cls]() mutable { init_class_defs<SampleLostState>(cls); };
    });
}

}  // namespace pyrti
