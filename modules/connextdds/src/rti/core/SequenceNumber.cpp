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
#include <rti/core/SequenceNumber.hpp>

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<SequenceNumber>& cls)
{
    cls.def(py::init<>(),
            "Create a default SequenceNumber, equal to "
            "SequenceNumber.unknown")
            .def(py::init<int64_t>(),
                 py::arg("value"),
                 "Creates a SequenceNumber from an integer.")
            .def_property(
                    "value",
                    (long long (SequenceNumber::*)() const)
                            & SequenceNumber::value,
                    (void (SequenceNumber::*)(long long))
                            & SequenceNumber::value,
                    "Get/set the SequenceNumber value.")
            .def(py::self == py::self, "Compare SequenceNumbers for equality.")
            .def(py::self != py::self,
                 "Compare SequenceNumbers for inequality.")
            .def(py::self + py::self, "Add two SequenceNumbers.")
            .def(py::self - py::self,
                 "Subtract one SequenceNumber from another.")
            .def(py::self += py::self,
                 "Compound assignment operator, assigning the result of the "
                 "addition to the current SequenceNumber object.")
            .def(py::self -= py::self,
                 "Compound assignment operator, assigning the result of the "
                 "subtraction to the current SequenceNumber object.")
            .def(py::self < py::self,
                 "Compare two SequenceNumbers for a less-than relationship.")
            .def(py::self <= py::self,
                 "Compare two SequenceNumbers for a less-than-or-equal "
                 "relationship.")
            .def(py::self > py::self,
                 "Compare two SequenceNumbers for a greater-than relationship.")
            .def(py::self >= py::self,
                 "Compare two SequenceNumbers for a greater-than-or-equal "
                 "relationship.")
            .def("__int__",
                 (long long (SequenceNumber::*)() const)
                         & SequenceNumber::value,
                 "Convert SequenceNumber to integer.")
            .def_property_readonly_static(
                    "zero",
                    [](py::object&) {
                        return SequenceNumber::zero();
                    },
                    "Create a SequenceNumber representing 0.")
            .def_property_readonly_static(
                    "unknown",
                    [](py::object&) {
                        return SequenceNumber::unknown();
                    },
                    "Create a SequenceNumber representing the unknown "
                    "SequenceNumber "
                    "value.")
            .def_property_readonly_static(
                    "maximum",
                    [](py::object&) {
                        return SequenceNumber::maximum();
                    },
                    "Create a SequenceNumber representing the highest, most "
                    "positive "
                    "value for the sequence number.")
            .def_property_readonly_static(
                    "automatic",
                    [](py::object&) {
                        return SequenceNumber::automatic();
                    },
                    "Create a SequenceNumber that will cause the value to be "
                    "internally determined by RTI Connext.");
    py::implicitly_convertible<int64_t, SequenceNumber>();
}

template<>
void process_inits<SequenceNumber>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<SequenceNumber>(m, "SequenceNumber");
    });
}

}  // namespace pyrti
