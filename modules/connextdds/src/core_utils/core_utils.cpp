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

// Raw pointers are returned as Python int objects. To ensure that the pointer
// size is correct, we use size_t
using PyPointer = size_t;

void init_core_utils(py::module& m)
{
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


    m.def(
            "memcpy_from_buffer_object",
            [](PyPointer dst, py::buffer src, size_t size) {
                auto dst_ptr = reinterpret_cast<void*>(dst);
                auto src_info = src.request();
                if (src_info.size * src_info.itemsize != size) {
                    throw dds::core::IllegalOperationError(
                            "Source buffer size doesn't match destination buffer size");
                }

                memcpy(dst_ptr, src_info.ptr, size);
            });

    m.def(
            "memcpy_to_buffer_object_slow",
            [](py::buffer dst, PyPointer src, size_t size) {
                auto src_ptr = reinterpret_cast<void*>(src);
                auto dst_info = dst.request();
                if (dst_info.size * dst_info.itemsize != size) {
                    throw dds::core::IllegalOperationError(
                            "Source buffer size doesn't match destination buffer size");
                }

                memcpy(dst_info.ptr, src_ptr, size);
            });


    m.def(
            "memcpy_to_buffer_object",
            [](py::buffer dst, PyPointer src, size_t size) {
                auto src_ptr = reinterpret_cast<void*>(src);
                // auto dst_info = dst.request();
                pyrti::PyCTypesBuffer dst_info(dst);
                if (dst_info.py_buffer.len != size) {
                    throw dds::core::IllegalOperationError(
                            "Source buffer size doesn't match destination buffer size");
                }

                memcpy(dst_info.py_buffer.buf, src_ptr, size);
            });
}
