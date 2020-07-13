#include "PyConnext.hpp"
#include "PyMaskType.hpp"
#include <rti/core/status/Status.hpp>

using namespace rti::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<SampleLostState>& cls)
{
    cls.def_static(
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
                "Creates SampleLostState.not_lost()");
        return [cls]() mutable { init_class_defs<SampleLostState>(cls); };
    });
}

}  // namespace pyrti
