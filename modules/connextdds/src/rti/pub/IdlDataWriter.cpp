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

#include <rti/core/memory.hpp>

using namespace dds::core::xtypes;
using namespace dds::topic;
using namespace rti::topic::cdr;

namespace pyrti {


// Caches python objects used by the DataWriter in the critical path for quick
// access
struct IdlDataWriterPyObjectCache {
    py::handle create_c_sample_func;
    py::handle type_support;
    rti::topic::cdr::CTypePlugin* type_plugin;
};

// Gets the Python objects associated to this writer
static IdlDataWriterPyObjectCache *get_py_objects(
        dds::pub::DataWriter<CSampleWrapper>& writer)
{
    auto objects =
            static_cast<IdlDataWriterPyObjectCache*>(writer->get_user_data_());
    RTI_CHECK_PRECONDITION(objects != nullptr);
    return objects;
}

static PyCTypesBuffer convert_sample(
        dds::pub::DataWriter<CSampleWrapper>& writer,
        const py::object& sample)
{
    IdlDataWriterPyObjectCache* obj_cache = get_py_objects(writer);
    return PyCTypesBuffer(
            obj_cache->create_c_sample_func(obj_cache->type_support, sample));
}

static void finalize_sample(
    dds::pub::DataWriter<CSampleWrapper>& writer,
    PyCTypesBuffer &c_sample)
{
    // TODO PY-17: Temporary function until we use a scratchpad sample instead
    // of creating one for every write.
    IdlDataWriterPyObjectCache* obj_cache = get_py_objects(writer);
    obj_cache->type_plugin->finalize_sample(c_sample);
}

// The Write implementation for IDL writers converts Python samples to C
// before calling the actual write operation.
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
        // TODO PY-17: Use a scratchpad sample instead of creating a new one
        // every time. Use it within the writer lock.
        PyCTypesBuffer c_sample = convert_sample(writer, sample);

        // release the GIL for the native write operation.
        {
            py::gil_scoped_release release;
            // Call the appropriate dds::pub::DataWriter::write overload
            writer.extensions().write(
                    c_sample,
                    std::forward<ExtraArgs>(extra_args)...);
        }

        // TODO PY-17: this is not exception-safe at the moment, but it should
        // be replaced when the scratchpad is implemented.
        finalize_sample(writer, c_sample);

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
        // least cache get_python_objects first).
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
        {
            py::gil_scoped_release release;
            return writer.extensions().register_instance(
                    cpp_sample,
                    std::forward<ExtraArgs>(extra_args)...);
        }

        finalize_sample(writer, cpp_sample);
    }
};

struct IdlDataWriterPostInitFunc {
    void operator()(PyDataWriter<CSampleWrapper>& writer)
    {
        using rti::core::memory::ObjectAllocator;

        // Store the IdlDataWriterPyObjectCache as this writer's user data
        if (writer->get_user_data_() == nullptr) {
            py::gil_scoped_acquire acquire;

            // Obtain all the cached objects
            auto type_support = get_py_type_support_from_topic(writer.topic());
            IdlDataWriterPyObjectCache tmp_obj_cache;
            tmp_obj_cache.type_support = type_support;
            tmp_obj_cache.create_c_sample_func =
                    py::type::of(type_support).attr("_create_c_sample");
            tmp_obj_cache.type_plugin =
                    py::cast<TypePlugin>(
                            type_support.attr("_plugin_dynamic_type"))
                            .type_plugin;

            // Once the objects have been successfully retrieved, allocate
            // the user data object and set it.
            auto obj_cache =
                    ObjectAllocator<IdlDataWriterPyObjectCache>::create();
            *obj_cache = std::move(tmp_obj_cache);
            writer->set_user_data_(obj_cache, [](void* ptr) {
                ObjectAllocator<IdlDataWriterPyObjectCache>::destroy(
                        static_cast<IdlDataWriterPyObjectCache*>(ptr));
            });
        }
    }
};


// Sets the Python methods for an IDL DataWriter
template<>
void init_dds_typed_datawriter_template(IdlDataWriterPyClass& cls)
{
    // Initialize the Python constructors with an additional initialization
    // functor that sets a Python object cache as the writer user data
    init_dds_datawriter_constructors<CSampleWrapper, IdlDataWriterPostInitFunc>(cls);

    // These untyped methods do not require any customization for IDL support.
    init_dds_datawriter_untyped_methods(cls);

    // Initialize the write methods with a custom implementation of the write
    // operation that translates from Python objects to ctypes objects.
    init_dds_datawriter_write_methods<CSampleWrapper, IdlWriteImpl>(cls);
}

} // namespace pyrti