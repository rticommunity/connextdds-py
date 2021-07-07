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
#include <rti/core/CoherentSetInfo.hpp>

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<CoherentSetInfo>& cls)
{
    cls.def(py::init<>(),
                    "Create a default CoherentSetInfo.")
            .def(py::init<const Guid&,
                            const SequenceNumber&,
                            const SequenceNumber&,
                            bool>(),
                    py::arg("group_guid"),
                    py::arg("coherent_set_sequence_number"),
                    py::arg("group_coherent_set_sequence_number"),
                    py::arg("incomplete_coherent_set"),
                    "Create a CoherentSetInfo with the specified parameters.")
            .def_property(
                    "group_guid",
                    (Guid& (CoherentSetInfo::*)())
                            & CoherentSetInfo::group_guid,
                    (CoherentSetInfo& (CoherentSetInfo::*)(const Guid&))
                            & CoherentSetInfo::group_guid,
                    "Coherent set group.")
            .def_property(
                    "coherent_set_sequence_number",
                    (SequenceNumber& (CoherentSetInfo::*)())
                            & CoherentSetInfo::coherent_set_sequence_number,
                    (CoherentSetInfo& (CoherentSetInfo::*)(const SequenceNumber&))
                            & CoherentSetInfo::coherent_set_sequence_number,
                    "The coherent set sequence number that identifies a sample "
                    "as part of a DataWriter coherent set.")
            .def_property(
                    "group_coherent_set_sequence_number",
                    (SequenceNumber& (CoherentSetInfo::*)())
                            & CoherentSetInfo::group_coherent_set_sequence_number,
                    (CoherentSetInfo& (CoherentSetInfo::*)(const SequenceNumber&))
                            & CoherentSetInfo::group_coherent_set_sequence_number,
                    "The group coherent set sequence number that identifies a sample "
                    "as part of a group coherent set.")
            .def_property(
                    "incomplete_coherent_set",
                    (bool (CoherentSetInfo::*)() const)
                            & CoherentSetInfo::incomplete_coherent_set,
                    (CoherentSetInfo& (CoherentSetInfo::*)(bool))
                            & CoherentSetInfo::incomplete_coherent_set,
                    "The incomplete coherent set status.")
            .def_property_readonly_static(
                    "UNKNOWN",
                    [](py::object&) {
                        return CoherentSetInfo::unknown();
                    },
                    "An invalid or unknown coherent set info.")
            .def(
                    "__str__",
                    [](const CoherentSetInfo& csi) {
                        std::ostringstream oss;
                        oss << csi;
                        return oss.str();
                    },
                    "Convert CoherentSetInfo to str.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<CoherentSetInfo>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<CoherentSetInfo>(m, "CoherentSetInfo");
    });
}

}  // namespace pyrti
#endif