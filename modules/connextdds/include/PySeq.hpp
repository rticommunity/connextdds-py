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