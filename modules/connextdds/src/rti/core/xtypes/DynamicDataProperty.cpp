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
#include <rti/core/xtypes/DynamicDataProperty.hpp>

using namespace rti::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<DynamicDataProperty>& cls)
{
    cls.def(py::init<>(), "Construct with default settings.")
            .def(py::init<int32_t, int32_t, int32_t, bool>(),
                 py::arg("initial_buffer_size"),
                 py::arg("max_buffer_size"),
                 py::arg("max_buffer_size_increment"),
                 py::arg("check_buffer_size"))
            .def_property(
                    "initial_buffer_size",
                    (int32_t(DynamicDataProperty::*)() const)
                            & DynamicDataProperty::buffer_initial_size,
                    (DynamicDataProperty & (DynamicDataProperty::*) (int32_t))
                            & DynamicDataProperty::buffer_initial_size,
                    "Initial buffer size.")
            .def_property(
                    "max_buffer_size",
                    (int32_t(DynamicDataProperty::*)() const)
                            & DynamicDataProperty::buffer_max_size,
                    (DynamicDataProperty & (DynamicDataProperty::*) (int32_t))
                            & DynamicDataProperty::buffer_max_size,
                    "Max buffer size.")
            .def_property(
                    "max_buffer_size_increment",
                    (int32_t(DynamicDataProperty::*)() const)
                            & DynamicDataProperty::buffer_max_size_increment,
                    (DynamicDataProperty & (DynamicDataProperty::*) (int32_t))
                            & DynamicDataProperty::buffer_max_size_increment,
                    "Max buffer size.")
            .def_property(
                    "check_buffer_size",
                    (bool (DynamicDataProperty::*)() const)
                            & DynamicDataProperty::buffer_check_size,
                    (DynamicDataProperty & (DynamicDataProperty::*) (bool) )
                            & DynamicDataProperty::buffer_check_size,
                    "Check buffer size.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<DynamicDataProperty>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DynamicDataProperty>(m, "DynamicDataProperty");
    });
}

}  // namespace pyrti
