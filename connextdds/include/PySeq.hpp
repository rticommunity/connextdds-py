#pragma once
#include "PyConnext.hpp"
#include <pybind11/stl_bind.h>
#include "PyBindVector.hpp"

namespace pyrti {

template<typename T, typename... Bases>
DefInitFunc init_class_with_seq(py::object& parent, const std::string& cls_name)
{
    py::class_<T, Bases...> cls(parent, cls_name.c_str());
    pyrti::bind_vector<T>(parent, (cls_name + "Seq").c_str());
    py::implicitly_convertible<py::iterable, std::vector<T>>();

    return ([cls]() mutable { init_class_defs<T>(cls); });
}

template<typename T, typename... Bases>
DefInitFunc init_class_with_ptr_seq(
        py::object& parent,
        const std::string& cls_name)
{
    py::class_<T, Bases...> cls(parent, cls_name.c_str());
    pyrti::bind_vector<T*>(parent, (cls_name + "Seq").c_str());
    py::implicitly_convertible<py::iterable, std::vector<T*>>();

    return ([cls]() mutable { init_class_defs<T>(cls); });
}

}  // namespace pyrti