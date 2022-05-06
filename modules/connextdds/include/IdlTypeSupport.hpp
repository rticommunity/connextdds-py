/*
 * (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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
#include <rti/core/xtypes/DynamicTypeImpl.hpp>
#include <rti/topic/cdr/GenericTypePluginFactory.hpp>

namespace pyrti {

// Holds a pointer to a TypePlugin and its associated DynamicType created from
// a GenericTypePluginFactory. This struct is used to pass around these pointers
// between Python and C++. The pointers are owned by the factory and are valid
// until the factory is destroyed.
struct TypePlugin {
    rti::core::xtypes::DynamicTypeImpl* type;
    rti::topic::cdr::CTypePlugin* type_plugin;
};

//
// Methods to get the type support for a given type
//

template<typename PyObjectType>
static void assert_valid_type_support(PyObjectType& type_support)
{
    if (!py::hasattr(type_support, "_plugin_dynamic_type")) {
        throw py::type_error(
                "Incompatible 'type' argument: not a valid an @idl.struct or "
                "@idl.union");
    }
}

// From an @idl_type-decorated python dataclass we get its DynamicType,
// which must be cached in the associated type_support
//
// @pre The GIL must be held
// @throw TypeError if idl_type is not a valid @idl_type-annotated type
inline
static py::object get_type_support_from_idl_type(py::object& type)
{
    // type must be a type
    if (!py::isinstance<py::type>(type)) {
        throw py::type_error(
                "Incompatible 'type' argument: an @idl.struct or @idl.union is "
                "required");
    }

    // type must have a type_support attribute (which is added by the decorator)
    if (!py::hasattr(type, "type_support")) {
        throw py::type_error(
                "Incompatible 'type' argument: an @idl.struct or @idl.union is "
                "required");
    }

    // the type_support must have a dynamic_type attribute
    auto type_support = type.attr("type_support");
    assert_valid_type_support(type_support);

    return type_support;
}

inline static rti::topic::cdr::CTypePlugin* get_type_plugin_from_type_support(
        py::object& type_support)
{
    // get the struct holding the DynamicType and the associated Type
    // Plugin
    auto py_type_plugin_holder = type_support.attr("_plugin_dynamic_type");

    // pybind11 magic allows casting the python object to the C++ one
    TypePlugin type_plugin_holder =
            py::cast<TypePlugin>(py_type_plugin_holder);

#ifndef NDEBUG
    if (type_plugin_holder.type_plugin == nullptr) {
        throw py::type_error(
                "Invalid type_support: type_plugin is not set");
    }
#endif

    return type_plugin_holder.type_plugin;
}


//
// Conversion from Python to C++ type
//

struct PyCTypesBuffer {
    Py_buffer py_buffer;

    explicit PyCTypesBuffer(const py::object& ctypes_sample)
    {
        PyObject* py_object_ptr = ctypes_sample.ptr();
        if (PyObject_GetBuffer(py_object_ptr, &py_buffer, PyBUF_SIMPLE) != 0) {
            throw py::error_already_set();
        }
    }

    // move-only struct
    PyCTypesBuffer(const PyCTypesBuffer&) = delete;
    PyCTypesBuffer& operator=(const PyCTypesBuffer&) = delete;

    PyCTypesBuffer(PyCTypesBuffer&& other)
    {
        py_buffer = other.py_buffer;
        other.py_buffer.buf = nullptr;
    }

    PyCTypesBuffer& operator=(PyCTypesBuffer&& other)
    {
        py_buffer = other.py_buffer;
        other.py_buffer.buf = nullptr;
        return *this;
    }

    ~PyCTypesBuffer()
    {
        if (py_buffer.buf != nullptr) {
            PyBuffer_Release(&py_buffer);
        }
    }

    // Obtain the actual buffer as a CSampleWrapper, as required by
    // PyDataWriter::py_write
    operator rti::topic::cdr::CSampleWrapper&()
    {
        return *reinterpret_cast<rti::topic::cdr::CSampleWrapper*>(
                py_buffer.buf);
    }
};


}  // namespace pyrti