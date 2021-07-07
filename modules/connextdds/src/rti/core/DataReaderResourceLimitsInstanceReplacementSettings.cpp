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

#if rti_connext_version_gte(6, 1, 0)

#ifndef _MSC_VER

#include <rti/core/PolicySettings.hpp>

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<DataReaderResourceLimitsInstanceReplacementSettings>& cls)
{
    cls.def(py::init<>(), "Creates an instance with the default removal kind for each instance state.")
            .def(py::init<policy::DataReaderInstanceRemovalKind, 
                    policy::DataReaderInstanceRemovalKind,
                    policy::DataReaderInstanceRemovalKind>(),
                 py::arg("alive_instance_removal"),
                 py::arg("disposed_instance_removal"),
                 py::arg("no_writers_instance_removal"),
                 "Creates an instance with the given DataReaderInstanceRemovalKind "
                 "for alive, disposed and no_writers instances, respectively.")
            .def_property(
                    "alive_instance_removal",
                    (policy::DataReaderInstanceRemovalKind (DataReaderResourceLimitsInstanceReplacementSettings::*)() const) &DataReaderResourceLimitsInstanceReplacementSettings::alive_instance_removal,
                    (DataReaderResourceLimitsInstanceReplacementSettings& (DataReaderResourceLimitsInstanceReplacementSettings::*)(policy::DataReaderInstanceRemovalKind)) &DataReaderResourceLimitsInstanceReplacementSettings::alive_instance_removal,
                    "The instance replacment policy for alive instances.")
            .def_property(
                    "disposed_instance_removal",
                    (policy::DataReaderInstanceRemovalKind (DataReaderResourceLimitsInstanceReplacementSettings::*)() const) &DataReaderResourceLimitsInstanceReplacementSettings::disposed_instance_removal,
                    (DataReaderResourceLimitsInstanceReplacementSettings& (DataReaderResourceLimitsInstanceReplacementSettings::*)(policy::DataReaderInstanceRemovalKind)) &DataReaderResourceLimitsInstanceReplacementSettings::disposed_instance_removal,
                    "The instance replacment policy for disposed instances.")
            .def_property(
                    "no_writers_instance_removal",
                    (policy::DataReaderInstanceRemovalKind (DataReaderResourceLimitsInstanceReplacementSettings::*)() const) &DataReaderResourceLimitsInstanceReplacementSettings::no_writers_instance_removal,
                    (DataReaderResourceLimitsInstanceReplacementSettings& (DataReaderResourceLimitsInstanceReplacementSettings::*)(policy::DataReaderInstanceRemovalKind)) &DataReaderResourceLimitsInstanceReplacementSettings::no_writers_instance_removal,
                    "The instance replacment policy for not-alive-no-writer instances.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<DataReaderResourceLimitsInstanceReplacementSettings>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DataReaderResourceLimitsInstanceReplacementSettings>(m, "DataReaderResourceLimitsInstanceReplacementSettings");
    });
}

}  // namespace pyrti

#endif

#endif