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
#include <dds/core/status/Status.hpp>
#include "PyMaskType.hpp"

using namespace dds::core::status;

namespace pyrti {

template<>
void init_class_defs(py::class_<SampleRejectedState>& cls)
{
    cls.def_static(
               "not_rejected",
               &SampleRejectedState::not_rejected,
               "Create a SampleRejectedState indicating that no samples were "
               "rejected.")
            .def_static(
                    "rejected_by_samples_limit",
                    &SampleRejectedState::rejected_by_samples_limit,
                    "Create a SampleRejectedState indicating that the sample "
                    "was "
                    "rejected because the DataReader's samples resource limit "
                    "was "
                    "reached.")
            .def_static(
                    "rejected_by_instances_limit",
                    &SampleRejectedState::rejected_by_instances_limit,
                    "Create a SampleRejectedState indicating that the sample "
                    "was "
                    "rejected because the DataReaders instances resource limit "
                    "was "
                    "reached.")
            .def_static(
                    "rejected_by_samples_per_instance_limit",
                    &SampleRejectedState::
                            rejected_by_samples_per_instance_limit,
                    "Create a SampleRejectedState indicating that the sample "
                    "was "
                    "rejected because the DataReader's samples per instance "
                    "resource "
                    "limit was reached.")
            .def_static(
                    "rejeced_by_remote_writers_limit",
                    &SampleRejectedState::rejected_by_remote_writers_limit,
                    "Create a SampleRejectedState indicating that the sample "
                    "was "
                    "rejected because the resource limit on the number of "
                    "remote "
                    "writers from which a DataReader may read was reached.")
            .def_static(
                    "rejected_by_remote_writers_per_instance_limit",
                    &SampleRejectedState::
                            rejected_by_remote_writers_per_instance_limit,
                    "Create a SampleRejectedState indicating that the sample "
                    "was "
                    "rejected because a resource limit on the number of remote "
                    "writers for a single instance from which a DataReader may "
                    "read "
                    "was reached.")
            .def_static(
                    "rejected_by_samples_per_remote_writer_limit",
                    &SampleRejectedState::
                            rejected_by_samples_per_remote_writer_limit,
                    "Create a SampleRejectedState indicating that the sample "
                    "was "
                    "rejected because the resource limit on the number of "
                    "samples "
                    "from a given remote writer that a DataReader may store "
                    "was "
                    "reached.")
            .def_static(
                    "rejected_by_virtual_writers_limit",
                    &SampleRejectedState::rejected_by_virtual_writers_limit,
                    "Create a SampleRejectedState indicating that the sample "
                    "was "
                    "rejected because the resource limit on the number of "
                    "virtual "
                    "writers from which a DataReader may read was reached.")
            .def_static(
                    "rejected_by_remote_writers_per_sample_limit",
                    &SampleRejectedState::
                            rejected_by_remote_writers_per_sample_limit,
                    "Create a SampleRejectedState indicating that the sample "
                    "was "
                    "rejected because the resource limit on the number of "
                    "remote "
                    "writers per sample was reached.")
            .def_static(
                    "rejected_by_remote_writers_per_virtual_queue_limit",
                    &SampleRejectedState::
                            rejected_by_remote_writers_per_virtual_queue_limit,
                    "Create a SampleRejectedState indicating that the sample "
                    "was "
                    "rejected because the resource limit on the number of "
                    "samples "
                    "published by a remote writer on behalf of a virtual "
                    "writer "
                    "that a DataReader may store was reached.");
}

template<>
void process_inits<SampleRejectedState>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        auto cls = init_mask_type_no_int_constructor<SampleRejectedState>(
                m,
                "SampleRejectedState",
                "Creates SampleRejectedState.not_rejected()");
        return [cls]() mutable { init_class_defs<SampleRejectedState>(cls); };
    });
}

}  // namespace pyrti
