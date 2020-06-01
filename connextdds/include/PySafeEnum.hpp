#pragma once

#include <pybind11/pybind11.h>
#include "PyOpaqueTypes.hpp"
#include <pybind11/operators.h>
#include <dds/core/SafeEnumeration.hpp>
#include <functional>

namespace py = pybind11;

using namespace dds::core;

namespace pyrti {

template<typename T>
py::class_<safe_enum<T>> init_dds_safe_enum(py::module& m, const std::string& name, std::function<void(py::object&)> enum_init) {
    py::class_<safe_enum<T>> cls(m, name.c_str());
    enum_init(cls);
    cls.def(
            py::init<>(), 
            "Initializes enum to 0."
        )
        .def(
            py::init<
                typename safe_enum<T>::inner_enum
            >(),
            "Copy constructor."
        )
        .def_property_readonly(
            "underlying",
            &safe_enum<T>::underlying,
            "Retrieves the actual enumerated value."
        )
        .def(
            py::self == py::self,
            "Apply operator to underlying enumerated values."
        )
        .def(
            py::self != py::self,
            "Apply operator to underlying enumerated values."
        )
        .def(
            py::self < py::self,
            "Apply operator to underlying enumerated values."
        )
        .def(
            py::self <= py::self,
            "Apply operator to underlying enumerated values."
        )
        .def(
            py::self > py::self,
            "Apply operator to underlying enumerated values."
        )
        .def(
            py::self >= py::self,
            "Apply operator to underlying enumerated values."
        )
        .def(
            "__str__",
            [](const safe_enum<T>& s) {
                return py::str(py::cast(s.underlying()));
            },
            "String conversion."
        )
        .def(
            "__int__",
            [](const safe_enum<T>& k) {
                return k.underlying();
            },
            "Int conversion."
        );
    py::implicitly_convertible<typename safe_enum<T>::inner_enum, safe_enum<T>>();
    return cls;
}

}
