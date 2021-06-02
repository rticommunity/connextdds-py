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

#pragma once

#include <pybind11/pybind11.h>
#include "PyOpaqueTypes.hpp"
#include <pybind11/operators.h>

namespace py = pybind11;

namespace pyrti {

template<typename T, typename U>
void add_qos_property(
        py::class_<T>& cls,
        const std::string& field_name,
        const std::string& property_name)
{
    cls.def_property(
               field_name.c_str(),
// Requred for MSVC version before VS2017
#if defined(_MS_VER) && _MS_VER <= 1900
               (U & (T::*) ()) & T::policy<U>,
               (T & (T::*) (const U&) ) & T::policy<U>,
#else
               (U & (T::*) ()) & T::template policy<U>,
               (T & (T::*) (const U&) ) & T::template policy<U>,
#endif
               ("Get/set " + property_name + " QoS.").c_str())
            .def("__lshift__",
#if defined(_MS_VER) && _MS_VER <= 1900
                 (T & (T::*) (const U&) ) & T::operator<U> <<,
#else
                 (T & (T::*) (const U&) ) & T::template operator<<<U>,
#endif
                 py::is_operator(),
                 ("Set the " + property_name + "QoS.").c_str())
            .def("__rshift__",
#if defined(_MS_VER) && _MS_VER <= 1900
                 (const T& (T::*) (U&) const) & T::operator<U> >>,
#else
                 (const T& (T::*) (U&) const) & T::template operator>><U>,
#endif
                 py::is_operator(),
                 ("Get the " + property_name + "QoS.").c_str());
}

#if rti_connext_version_gte(6, 1, 0)
template<typename T>
void add_qos_string_conversions(py::class_<T>& cls) {
    cls.def(
            "__str__",
            [](const T& qos) {
                return dds::sub::qos::to_string(qos);
            })
        .def(
            "to_string",
            [](const T& qos,
                    const rti::core::QosPrintFormat& format,
                    dds::core::optional<T> base,
                    bool print_all) {
                if (base.has_value() && print_all) {
                    throw dds::core::InvalidArgumentError("Cannot print all with base profile.");
                }
                if (base.has_value()) {
                    return dds::sub::qos::to_string(qos, base, format);
                }
                else if (print_all) {
                    return dds::sub::qos::to_string(qos, rti::core::qos_print_all, format);
                }
                else {
                    return dds::sub::qos::to_string(qos, format);
                }
            },
            py::arg("qos"),
            py::arg_v("format", rti::core::QosPrintFormat(), "QosPrintFormat()"),
            py::arg("base") = py::none(),
            py::arg("print_all") = false,
            "Convert QoS to string based on params.");
}
#endif

}  // namespace pyrti
