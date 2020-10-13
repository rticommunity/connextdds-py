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

#if rti_connext_version_gte(6, 0, 0)

namespace pyrti {

template<>
void init_class_defs(py::class_<DataRepresentation>& cls)
{
    cls.def(py::init<>(), "Creates an instance with one element, AUTO_ID.")
            .def(py::init<const std::vector<int16_t>&>(),
                 py::arg("representations"),
                 "Creates an instance with a sequence of Data Representation "
                 "IDs.")
            .def_property(
                    "value",
                    (std::vector<int16_t>(DataRepresentation::*)() const)
                            & DataRepresentation::value,
                    [](DataRepresentation& dr, std::vector<int16_t>& values) {
                        return dr.value(values.begin(), values.end());
                    },
                    "A sequence of IDs representing the allowed data "
                    "representations. "
                    "\n\n"
                    "This property uses copy semantics. Changes to the "
                    "sequence are "
                    "not reflected in the policy unless the sequence is "
                    "commited back "
                    "to the policy object via the property's setter.")
            .def_property_readonly_static(
                    "XCDR",
                    [](py::object&) { return DataRepresentation::xcdr(); },
                    "Extended Common Data Representation ID for encoding "
                    "version 1.")
            .def_property_readonly_static(
                    "XML",
                    [](py::object&) { return DataRepresentation::xml(); },
                    "XML Data Representation ID (unsupported).")
            .def_property_readonly_static(
                    "XCDR2",
                    [](py::object&) { return DataRepresentation::xcdr2(); },
                    "Extended Common Data Representation ID for encoding "
                    "version 2.")
            .def_property_readonly_static(
                    "AUTO_ID",
                    [](py::object&) { return DataRepresentation::auto_id(); },
                    "ID that makes representation automatically chosen based "
                    "on the "
                    "type.")
            .def(py::self == py::self)
            .def(py::self != py::self);
}

template<>
void process_inits<DataRepresentation>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DataRepresentation>(m, "DataRepresentation");
    });
}

}  // namespace pyrti

#endif