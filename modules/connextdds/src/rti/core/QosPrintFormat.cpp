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

#include <rti/core/QosPrintFormat.hpp>

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<QosPrintFormat>& cls)
{
    cls.def(py::init<>(), "Initializes the properties with the default values.")
            .def(py::init<uint32_t, bool, bool>(),
                 py::arg("indent"),
                 py::arg("private"),
                 py::arg("is_standalone"),
                 "Initializes the properties.")
            .def_property(
                    "indent",
                    (uint32_t (QosPrintFormat::*)() const) &QosPrintFormat::indent,
                    (QosPrintFormat& (QosPrintFormat::*)(uint32_t)) &QosPrintFormat::indent,
                    "The value of indent.")
            .def_property(
                    "print_private",
                    (bool (QosPrintFormat::*)() const) &QosPrintFormat::print_private,
                    (QosPrintFormat& (QosPrintFormat::*)(bool)) &QosPrintFormat::print_private,
                    "Print private QoS field toggle.")
            .def_property(
                    "is_standalone",
                    (bool (QosPrintFormat::*)() const) &QosPrintFormat::is_standalone,
                    (QosPrintFormat& (QosPrintFormat::*)(bool)) &QosPrintFormat::is_standalone,
                    "Print XML preample toggle.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<QosPrintFormat>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<QosPrintFormat>(m, "QosPrintFormat");
    });
}

}  // namespace pyrti

#endif