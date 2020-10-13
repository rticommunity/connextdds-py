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
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<DurabilityService>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<
                         const dds::core::Duration&,
                         HistoryKind,
                         int32_t,
                         int32_t,
                         int32_t,
                         int32_t>(),
                 py::arg("service_cleanup_delay"),
                 py::arg("history_kind"),
                 py::arg("history_depth"),
                 py::arg("max_samples"),
                 py::arg("max_instances"),
                 py::arg("max_samples_per_instance"),
                 "Creates an instance with all the parameters set.")
            .def_property(
                    "service_cleanup_delay",
                    (dds::core::Duration(DurabilityService::*)() const)
                            & DurabilityService::service_cleanup_delay,
                    (DurabilityService
                     & (DurabilityService::*) (const dds::core::Duration&) )
                            & DurabilityService::service_cleanup_delay,
                    "Controls when the service is able to remove all "
                    "information "
                    "regarding a data instances.")
            .def_property(
                    "history_kind",
                    (HistoryKind(DurabilityService::*)() const)
                            & DurabilityService::history_kind,
                    (DurabilityService & (DurabilityService::*) (HistoryKind))
                            & DurabilityService::history_kind,
                    "Controls when the service is able to remove all "
                    "information "
                    "regarding a data instances.")
            .def_property(
                    "history_depth",
                    (int32_t(DurabilityService::*)() const)
                            & DurabilityService::history_depth,
                    (DurabilityService & (DurabilityService::*) (int32_t))
                            & DurabilityService::history_depth,
                    "Controls when the service is able to remove all "
                    "information "
                    "regarding a data instances.")
            .def_property(
                    "max_samples",
                    (int32_t(DurabilityService::*)() const)
                            & DurabilityService::max_samples,
                    (DurabilityService & (DurabilityService::*) (int32_t))
                            & DurabilityService::max_samples,
                    "Controls when the service is able to remove all "
                    "information "
                    "regarding a data instances.")
            .def_property(
                    "max_instances",
                    (int32_t(DurabilityService::*)() const)
                            & DurabilityService::max_instances,
                    (DurabilityService & (DurabilityService::*) (int32_t))
                            & DurabilityService::max_instances,
                    "Controls when the service is able to remove all "
                    "information "
                    "regarding a data instances.")
            .def_property(
                    "max_samples_per_instance",
                    (int32_t(DurabilityService::*)() const)
                            & DurabilityService::max_samples_per_instance,
                    (DurabilityService & (DurabilityService::*) (int32_t))
                            & DurabilityService::max_samples_per_instance,
                    "Controls when the service is able to remove all "
                    "information "
                    "regarding a data instances.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<DurabilityService>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DurabilityService>(m, "DurabilityService");
    });
}

}  // namespace pyrti
