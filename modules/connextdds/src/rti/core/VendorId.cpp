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
#include <rti/core/VendorId.hpp>

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<VendorId>& cls)
{
    cls.def(py::init<>(), "Creates unknown vendor ID.")
            .def_property_readonly(
                    "value",
                    &VendorId::value,
                    "Provides access to the bytes that represent the vendor "
                    "id.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.")
            .def_property_readonly_static(
                    "unknown",
                    [](py::object&) {
                        return VendorId::unknown();
                    },
                    "Create an unknown vendor ID.");
}

template<>
void process_inits<VendorId>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable { return init_class<VendorId>(m, "VendorId"); });
}

}  // namespace pyrti
