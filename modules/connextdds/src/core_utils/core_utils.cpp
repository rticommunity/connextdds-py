/*
 * (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */


#include "PyConnext.hpp"
#include "PyCoreUtils.hpp"
#include "IdlTypeSupport.hpp"

#include "osapi/osapi_heap.h"

namespace pyrti {

// Raw pointers are returned as Python int objects. To ensure that the pointer
// size is correct, we use size_t
using PyPointer = size_t;

static void memcpy_from_buffer_object(
        PyPointer dst,
        py::buffer src,
        size_t size)
{
    auto dst_ptr = reinterpret_cast<void*>(dst);
    auto src_info = src.request();
    if (src_info.size * src_info.itemsize != size) {
        throw dds::core::IllegalOperationError(
                "Source buffer size doesn't match destination buffer size");
    }

    memcpy(dst_ptr, src_info.ptr, size);
}

static void memcpy_to_buffer_object_slow(
        py::buffer dst,
        PyPointer src,
        size_t size)
{
    auto src_ptr = reinterpret_cast<void*>(src);
    auto dst_info = dst.request();
    if (dst_info.size * dst_info.itemsize != size) {
        throw dds::core::IllegalOperationError(
                "Source buffer size doesn't match destination buffer size");
    }

    memcpy(dst_info.ptr, src_ptr, size);
}

static void memcpy_to_buffer_object(py::buffer dst, PyPointer src, size_t size)
{
    auto src_ptr = reinterpret_cast<void*>(src);
    // auto dst_info = dst.request();
    pyrti::PyCTypesBuffer dst_info(dst);
    if (dst_info.py_buffer.len != size) {
        throw dds::core::IllegalOperationError(
                "Source buffer size doesn't match destination buffer size");
    }

    memcpy(dst_info.py_buffer.buf, src_ptr, size);
}

static size_t strcpy_from_buffer_object_no_null_char(char* dst, py::buffer src)
{
    auto src_info = src.request();
    size_t size = src_info.size * src_info.itemsize;
    memcpy(dst, src_info.ptr, size);
    return size;
}

static void strcpy_from_buffer_object(PyPointer dst, py::buffer src)
{
    // capacity of dst pointer must be checked before calling this function
    auto dst_ptr = reinterpret_cast<char*>(dst);
    size_t byte_count = strcpy_from_buffer_object_no_null_char(dst_ptr, src);
    dst_ptr[byte_count] = 0;
}

static void wstrcpy_from_buffer_object(PyPointer dst, py::buffer src)
{
    // capacity of dst pointer must be checked before calling this function
    auto dst_ptr = reinterpret_cast<char*>(dst);
    size_t byte_count = strcpy_from_buffer_object_no_null_char(dst_ptr, src);
    dst_ptr[byte_count] = 0;
    dst_ptr[byte_count + 1] = 0;
}

static PyPointer string_realloc(PyPointer str, size_t new_size)
{
    char *str_ptr = reinterpret_cast<char*>(str);
    RTIOsapiHeap_reallocateString(&str_ptr, new_size);
    if (str_ptr == nullptr) {
        throw std::bad_alloc();
    }

    return reinterpret_cast<PyPointer>(str_ptr);
}

static PyPointer wstring_realloc(PyPointer str, size_t new_size)
{
    RTIXCdrWchar* str_ptr = reinterpret_cast<RTIXCdrWchar*>(str);
    RTIOsapiHeap_reallocateArray(
            &str_ptr,
            new_size + sizeof(RTIXCdrWchar),
            RTIXCdrWchar);
    if (str_ptr == nullptr) {
        throw std::bad_alloc();
    }

    return reinterpret_cast<PyPointer>(str_ptr);
}

static py::memoryview get_memoryview_from_string(PyPointer ptr)
{
    size_t length = strlen(reinterpret_cast<char*>(ptr));
    return py::memoryview::from_memory(reinterpret_cast<void*>(ptr), length);
}

static py::memoryview get_memoryview_from_wstring(PyPointer ptr)
{
    size_t length =
            RTIXCdrWString_getLength(reinterpret_cast<RTIXCdrWchar*>(ptr));

    return py::memoryview::from_memory(
            reinterpret_cast<void*>(ptr),
            length * sizeof(RTIXCdrWchar));
}

} // namespace pyrti

void init_core_utils(py::module& m)
{
    using namespace pyrti;

    static_assert(
            sizeof(PyPointer) == sizeof(void*),
            "This platform's pointer size is not the same as size_t's size");

    m.def(
            "malloc",
            [](size_t size) {
                return reinterpret_cast<PyPointer>(RTIOsapiHeap_malloc(size));
            },
            py::arg("size"));

    m.def(
            "free",
            [](PyPointer ptr) {
                RTIOsapiHeap_free(reinterpret_cast<void*>(ptr));
            },
            py::arg("ptr"));

    m.def("memcpy_from_buffer_object", memcpy_from_buffer_object);
    m.def("memcpy_to_buffer_object_slow", memcpy_to_buffer_object_slow);
    m.def("memcpy_to_buffer_object", memcpy_to_buffer_object);
    m.def("strcpy_from_buffer_object", strcpy_from_buffer_object);
    m.def("wstrcpy_from_buffer_object", wstrcpy_from_buffer_object);
    m.def("get_memoryview_from_string", get_memoryview_from_string);
    m.def("get_memoryview_from_wstring", get_memoryview_from_wstring);
    m.def("string_realloc", string_realloc);
    m.def("wstring_realloc", wstring_realloc);
}
