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

#include <sstream>

#include "PyConnext.hpp"
#include <rti/core/SampleIdentity.hpp>

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<SampleIdentity>& cls)
{
    cls.def(py::init<>(), "Create a default SampleIdentity object.")
            .def(py::init<const Guid&, const SequenceNumber&>(),
                 py::arg("writer_guid"),
                 py::arg("sequence_number"),
                 "Create a SampleIdentity with the provided Guid and "
                 "SequenceNumber.")
            .def_property_readonly(
                    "writer_guid",
                    (Guid & (SampleIdentity::*) ())
                            & SampleIdentity::writer_guid,
                    "Get the 16-byte identifier identifying the virtual Guid.")
            .def_property_readonly(
                    "sequence_number",
                    (SequenceNumber & (SampleIdentity::*) ())
                            & SampleIdentity::sequence_number,
                    "Get the monotonically increasing 64-bit SequenceNumber "
                    "that "
                    "identifies the sample in the data source.")
            .def("__str__",
                 [](const SequenceNumber& sn) {
                     std::ostringstream oss;
                     oss << sn;
                     return oss.str();
                 })
            .def_property_readonly_static(
                    "automatic",
                    [](py::object&) {
                        return SampleIdentity::automatic();
                    },
                    "Special value to indicate that RTI Connext will "
                    "automatically "
                    "assign the identity of a sample.")
            .def_property_readonly_static(
                    "unknown",
                    [](py::object&) {
                        return SampleIdentity::unknown();
                    },
                    "An invalid or unknown sample identity.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<SampleIdentity>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<SampleIdentity>(m, "SampleIdentity");
    });
}

}  // namespace pyrti
