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

#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>

#include "IdlDataWriter.hpp"


using namespace dds::core::xtypes;
using namespace dds::topic;
using namespace rti::topic::cdr;

namespace pyrti {

struct PyCTypesBuffer {
    Py_buffer py_buffer;

    explicit PyCTypesBuffer(const py::object& ctypes_sample)
    {
        PyObject* py_object_ptr = ctypes_sample.ptr();
        PyObject_GetBuffer(py_object_ptr, &py_buffer, PyBUF_SIMPLE);
    }

    // move-only struct
    PyCTypesBuffer(const PyCTypesBuffer&) = delete;
    PyCTypesBuffer& operator=(const PyCTypesBuffer&) = delete;
    PyCTypesBuffer(PyCTypesBuffer&& other) = default;
    PyCTypesBuffer& operator=(PyCTypesBuffer&& other) = default;

    ~PyCTypesBuffer()
    {
        PyBuffer_Release(&py_buffer);
    }

    // Obtain the actual buffer as a CSampleWrapper, as required by
    // PyDataWriter::py_write
    operator CSampleWrapper()
    {
        return { py_buffer.buf };
    }
};

auto get_create_c_sample_function(dds::pub::DataWriter<CSampleWrapper>& writer)
{
    auto type_support = get_py_type_support_from_topic(writer.topic());
    return type_support.attr("_create_c_sample");
}

static PyCTypesBuffer convert_sample(
        dds::pub::DataWriter<CSampleWrapper>& writer,
        const py::object& sample)
{
    auto&& create_c_sample_func = get_create_c_sample_function(writer);
    return PyCTypesBuffer(create_c_sample_func(sample));
}


struct IdlWriteImpl {

    using py_sample = py::object;
    using gil_policy = py::gil_scoped_acquire;

    // This function converts the data sample from its python type into the
    // C++ type that the C++ writer will use.
    //
    // The variadict template (ExtraArgs) allows using this same implementation
    // for several write() overloads (timestamp, instance handle, etc.)
    //
    // For IDL types, PySample is py::object and the C++ type is CSampleWrapper;
    // for other types (DynamicData and built-in types) both are the same type,
    // and the conversion is a no-op, because pybind11 already took care of
    // this one-to-one conversion.
    template<typename... ExtraArgs>
    static void py_write(
            IdlDataWriter& writer,
            const py_sample& sample,
            ExtraArgs&&... extra_args)
    {
        PyCTypesBuffer c_sample = convert_sample(writer, sample);

        // release the GIL for the native write operation.
        py::gil_scoped_release release;

        // Call the appropriate dds::pub::DataWriter::write overload
        writer.extensions().write(
                c_sample,
                std::forward<ExtraArgs>(extra_args)...);

        // c_sample's destructor releases resources acquired by convert();
    }

    template<typename... ExtraArgs>
    static void py_write_range(
            IdlDataWriter& writer,
            const std::vector<py_sample>& samples,
            ExtraArgs&&... extra_args)
    {
        // TODO PY-17: Evaluate if this can be optimized (e.g. convert all
        // first to avoid taking/releasing the GIL in each iteration, or at
        // least cache get_create_c_sample_function first).
        for (const auto& sample : samples) {
            py_write(writer, sample, std::forward<ExtraArgs>(extra_args)...);
        }
    }

    template<typename... ExtraArgs>
    static dds::core::InstanceHandle py_register_instance(
            IdlDataWriter& writer,
            const py_sample& sample,
            ExtraArgs&&... extra_args)
    {
        // See py_write for a description of the implementation
        PyCTypesBuffer cpp_sample = convert_sample(writer, sample);

        // release the GIL for the native write operation.
        py::gil_scoped_release release;

        return writer.extensions().register_instance(
                cpp_sample,
                std::forward<ExtraArgs>(extra_args)...);
    }
};

template<>
void init_dds_typed_datawriter_template(IdlDataWriterPyClass& cls)
{
    init_dds_datawriter_untyped_methods(cls);
    init_dds_datawriter_write_methods<CSampleWrapper, IdlWriteImpl>(cls);
}

} // namespace pyrti