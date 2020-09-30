#pragma once
#include "PyConnext.hpp"
#include <pybind11/stl_bind.h>


namespace pyrti {


template<typename T>
std::vector<T> vector_replicate(std::vector<T>& v, size_t count)
{
    if (1 == v.size()) {
        return std::vector<T>(count, v[0]);
    } else {
        std::vector<T> retval;
        retval.reserve(v.size() * count);
        auto begin = v.begin();
        auto end = v.end();
        for (int i = 0; i < count; ++i) {
            retval.insert(retval.end(), begin, end);
        }
        return retval;
    }
}


template<typename T>
std::vector<T>& vector_replicate_inplace(std::vector<T>& v, size_t count)
{
    if (1 == v.size()) {
        v.resize(count, v[0]);
        return v;
    } else {
        v.reserve(v.size() * count);
        auto begin = v.begin();
        auto end = v.end();
        // Start at 1 because we already have the first iteration
        for (int i = 1; i < count; ++i) {
            v.insert(v.end(), begin, end);
        }
        return v;
    }
}


template<typename T>
std::vector<T> vector_concat(std::vector<T>& v, std::vector<T>& other)
{
    std::vector<T> retval(v);
    retval.insert(retval.end(), other.begin(), other.end());
    return retval;
}


template<typename T>
std::vector<T>& vector_concat_inplace(std::vector<T>& v, std::vector<T>& other)
{
    v.reserve(v.size() + other.size());
    v.insert(v.end(), other.begin(), other.end());
    return v;
}


template<typename T>
void bind_buffer_vector(const py::object& m, const char* name, const char* alias = nullptr)
{
    auto cls = py::bind_vector<std::vector<T>>(m, name, py::buffer_protocol());
    cls.def("__mul__",
            &pyrti::vector_replicate<T>,
            py::return_value_policy::move)
            .def("__rmul__",
                 &pyrti::vector_replicate<T>,
                 py::return_value_policy::move)
            .def("__imul__",
                 &pyrti::vector_replicate_inplace<T>,
                 py::return_value_policy::reference)
            .def("__add__",
                 &pyrti::vector_concat<T>,
                 py::return_value_policy::move)
            .def("__iadd__",
                 &pyrti::vector_concat_inplace<T>,
                 py::return_value_policy::reference);

    if (nullptr != alias) {
        m.attr(alias) = cls;
    }
}

template<typename T>
void bind_vector(const py::object& m, const char* name, const char* alias = nullptr)
{
    auto cls = py::bind_vector<std::vector<T>>(m, name);
    cls.def("__mul__",
            &pyrti::vector_replicate<T>,
            py::return_value_policy::move)
            .def("__rmul__",
                 &pyrti::vector_replicate<T>,
                 py::return_value_policy::move)
            .def("__imul__",
                 &pyrti::vector_replicate_inplace<T>,
                 py::return_value_policy::move)
            .def("__add__",
                 &pyrti::vector_concat<T>,
                 py::return_value_policy::move)
            .def("__iadd__",
                 &pyrti::vector_concat_inplace<T>,
                 py::return_value_policy::reference);

    if (nullptr != alias) {
        m.attr(alias) = cls;
    }
}

}  // namespace pyrti
