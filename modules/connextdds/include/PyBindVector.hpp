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
        for (size_t i = 0; i < count; ++i) {
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
        for (size_t i = 1; i < count; ++i) {
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
static void vector_resize_no_init(std::vector<T>& sequence, size_t size)
{
    // For std::vector<T> such that T is a POD (a primitive type in our
    // case), avoid initializing the elements on resize().

    // Declare a struct with the same size as T but a no-op default
    // constructor
    struct no_init {
        T v;
        no_init()
        {
            // empty
            // ROBUSTNESS-253
            // coverity[uninit_member]
        }
    };

    static_assert(
            sizeof(no_init[4]) == sizeof(T[4]),
            "T and no_init do not have the same alignment");

    // Recast std::vector<T> as an equivalent vector whose allocator doesn't
    // initialize its members.
    using allocator = typename std::vector<T>::allocator_type;
    using no_init_allocator = typename std::allocator_traits<
            allocator>::template rebind_alloc<no_init>;
    using no_init_vector = std::vector<no_init, no_init_allocator>;

    reinterpret_cast<no_init_vector&>(sequence).resize(size);
}

template <typename T>
py::bytes create_py_bytes_from_vector(const std::vector<T>& v)
{
    return py::bytes(
            reinterpret_cast<const char*>(v.data()),
            v.size() * sizeof(T));
}

template <typename T>
std::vector<T> create_vector_from_py_bytes(py::bytes bytes)
{
    char* buffer = nullptr;
    py::ssize_t length = 0;
    if (PYBIND11_BYTES_AS_STRING_AND_SIZE(bytes.ptr(), &buffer, &length)) {
        throw std::runtime_error("Failed to get bytes from py::bytes");
    }

    if (length % sizeof(T) != 0) {
        throw std::runtime_error("Bytes length is not a multiple of sizeof(T)");
    }

    return std::vector<T>(
            reinterpret_cast<const T*>(buffer),
            reinterpret_cast<const T*>(buffer) + (length / sizeof(T)));
}


template<typename T>
void bind_buffer_vector(const py::object& m, const char* name, const char* alias = nullptr)
{
    auto cls = py::bind_vector<std::vector<T>>(m, name, py::buffer_protocol());
    cls.def(py::init<typename std::vector<T>::size_type>());
    cls.def("__mul__",
            &pyrti::vector_replicate<T>,
            py::return_value_policy::move);
    cls.def("__rmul__",
            &pyrti::vector_replicate<T>,
            py::return_value_policy::move);
    cls.def("__imul__",
            &pyrti::vector_replicate_inplace<T>,
            py::return_value_policy::reference);
    cls.def("__add__", &pyrti::vector_concat<T>, py::return_value_policy::move);
    cls.def("__iadd__",
            &pyrti::vector_concat_inplace<T>,
            py::return_value_policy::reference);

    cls.def("resize",
            vector_resize_no_init<T>,
            "resizes the vector to the given size");

    // Add pickle support, required by copy.deepcopy, which is supported by
    // IDL types where dds.Int32Seq, etc may be used.
    cls.def(py::pickle(
            [](const std::vector<T>& p) {  // __getstate__
                return create_py_bytes_from_vector(p);
            },
            [](py::bytes bytes) {  // __setstate__
                return create_vector_from_py_bytes<T>(bytes);
            }));

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
