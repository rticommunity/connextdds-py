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

}  // namespace pyrti
