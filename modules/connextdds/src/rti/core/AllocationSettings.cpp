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
#if rti_connext_version_lt(6, 1, 0)
#include <rti/core/AllocationSettings.hpp>
#else
#include <rti/core/PolicySettings.hpp>
#endif

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<AllocationSettings>& cls)
{
    cls.def(py::init<>(),
            "Creates an instance with an initial, max and incremental count "
            "set to zero.")
            .def(py::init<int32_t, int32_t, int32_t>(),
                 py::arg("initial_count"),
                 py::arg("max_count"),
                 py::arg("incremental_count"),
                 "Creates an instance with the given initial, maximum and "
                 "incremental values.")
            .def_property(
                    "initial_count",
                    (int32_t(AllocationSettings::*)() const)
                            & AllocationSettings::initial_count,
                    (AllocationSettings & (AllocationSettings::*) (int32_t))
                            & AllocationSettings::initial_count,
                    "Initial count of resources.")
            .def_property(
                    "max_count",
                    (int32_t(AllocationSettings::*)() const)
                            & AllocationSettings::max_count,
                    (AllocationSettings & (AllocationSettings::*) (int32_t))
                            & AllocationSettings::max_count,
                    "Max count of resources.")
            .def_property(
                    "incremental_count",
                    (int32_t(AllocationSettings::*)() const)
                            & AllocationSettings::incremental_count,
                    (AllocationSettings & (AllocationSettings::*) (int32_t))
                            & AllocationSettings::incremental_count,
                    "Incremental count of resources.")
#if rti_connext_version_gte(6, 0, 0)
            .def_property_readonly_static(
                    "AUTO_COUNT",
                    [](py::object&) { return AllocationSettings::AUTO_COUNT; },
                    "A special value indicating that the quantity is derived "
                    "from a "
                    "different Qos value.")
#endif
            ;
}

template<>
void process_inits<AllocationSettings>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<AllocationSettings>(m, "AllocationSettings");
    });
}

}  // namespace pyrti
