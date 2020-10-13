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

#if rti_connext_version_gte(6, 0, 0)

using namespace rti::core;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<DataWriterShmemRefTransferModeSettings>& cls)
{
    cls.def(py::init<>(), "Creates an instance with the default settings.")
            .def_property(
                    "enable_data_consistency_check",
                    (bool (rti::core::
                                   DataWriterShmemRefTransferModeSettings::*)()
                             const)
                            & rti::core::
                                    DataWriterShmemRefTransferModeSettings::
                                            enable_data_consistency_check,
                    (rti::core::DataWriterShmemRefTransferModeSettings
                     & (rti::core::
                                DataWriterShmemRefTransferModeSettings::*) (bool) )
                            & rti::core::
                                    DataWriterShmemRefTransferModeSettings::
                                            enable_data_consistency_check,
                    "Controls if samples can be checked for consistency.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void init_class_defs(py::class_<DataWriterTransferMode>& cls)
{
    cls.def(py::init<>(),
            "Creates a DataWriterTransferMode qos policy with default values.")
            .def_property(
                    "shmem_ref_settings",
                    (rti::core::DataWriterShmemRefTransferModeSettings
                     & (DataWriterTransferMode::*) ())
                            & DataWriterTransferMode::shmem_ref_settings,
                    (DataWriterTransferMode
                     & (DataWriterTransferMode::*) (const rti::core::
                                                            DataWriterShmemRefTransferModeSettings&) )
                            & DataWriterTransferMode::shmem_ref_settings,
                    "Settings related to transferring data using shared memory "
                    "references.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<DataWriterTransferMode>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DataWriterShmemRefTransferModeSettings>(
                m,
                "DataWriterShmemRefTransferModeSettings");
    });

    l.push_back([m]() mutable {
        return init_class<DataWriterTransferMode>(m, "DataWriterTransferMode");
    });
}

}  // namespace pyrti

#endif