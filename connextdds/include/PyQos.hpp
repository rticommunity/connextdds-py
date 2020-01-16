#pragma once

#include <pybind11/pybind11.h>
#include "PyOpaqueTypes.hpp"
#include <pybind11/operators.h>

namespace py = pybind11;

namespace pyrti {
    template<typename T, typename U>
    void add_qos_property(py::class_<T>& cls, const std::string& field_name, const std::string& property_name) {
        cls
            .def_property(
                field_name.c_str(),
//Requred for MSVC version before VS2017
#if defined(_MS_VER) && _MS_VER <= 1900
                (U& (T::*)()) &T::policy<U>,
                (T& (T::*)(const U&)) &T::policy<U>,
#else
                (U& (T::*)()) &T::template policy<U>,
                (T& (T::*)(const U&)) &T::template policy<U>,
#endif
                ("Get/set " + property_name + " QoS.").c_str()
            )
            .def(
                "__lshift__",
#if defined(_MS_VER) && _MS_VER <= 1900
                (T& (T::*)(const U&)) &T::operator<U><<,
#else
                (T& (T::*)(const U&)) &T::template operator<<<U>,
#endif
                py::is_operator(),
                ("Set the " + property_name + "QoS.").c_str()
            )
            .def(
                "__rshift__",
#if defined(_MS_VER) && _MS_VER <= 1900
                (const T& (T::*)(U&) const) &T::operator<U>>>,
#else
                (const T& (T::*)(U&) const) &T::template operator>><U>,
#endif
                py::is_operator(),
                ("Get the " + property_name + "QoS.").c_str()
            );
    }
}
