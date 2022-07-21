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
#include <rti/core/EntityLock.hpp>

using namespace dds::core::xtypes;
using namespace dds::topic;
using namespace rti::topic::cdr;

namespace pyrti {


// Caches python objects used by the DataWriter in the critical path for quick
// access
struct IdlDataWriterPyObjectCache {
    py::handle type_support;
    rti::topic::cdr::CTypePlugin* type_plugin;
    py::handle create_c_sample_func;
    py::handle convert_to_c_sample_func;
    py::handle create_py_sample_func;
    py::object c_sample;
    PyCTypesBuffer c_sample_buffer;

    IdlDataWriterPyObjectCache(py::handle the_type_support)
            : type_support(the_type_support),
              type_plugin(py::cast<TypePlugin>(
                                  type_support.attr("_plugin_dynamic_type"))
                                  .type_plugin),
              create_c_sample_func(py::type::of(type_support)
                                           .attr("_create_empty_c_sample")),
              convert_to_c_sample_func(
                      py::type::of(type_support).attr("_convert_to_c_sample")),
              create_py_sample_func(py::type::of(type_support).attr("_create_py_sample")),
              c_sample(create_c_sample_func(type_support)),
              c_sample_buffer(c_sample)

    {
    }

    ~IdlDataWriterPyObjectCache()
    {
        try {
            finalize_c_sample();
        } catch (...) {
            // Ignore exceptions
        }
    }

    // Disable copies
    IdlDataWriterPyObjectCache(const IdlDataWriterPyObjectCache&) = delete;
    IdlDataWriterPyObjectCache& operator=(const IdlDataWriterPyObjectCache&) =
            delete;
    // Default move
    IdlDataWriterPyObjectCache(IdlDataWriterPyObjectCache&&) = default;
    IdlDataWriterPyObjectCache& operator=(IdlDataWriterPyObjectCache&&) =
            default;

    void convert_to_c_sample(const py::object& py_sample)
    {
        convert_to_c_sample_func(type_support, c_sample, py_sample);
    }

    void finalize_c_sample()
    {
        if (type_plugin != nullptr && c_sample) {
            type_plugin->finalize_sample(c_sample_buffer);
        }
    }

    py::object create_py_sample()
    {
        // We pass the pointer to the python function as an integer,
        // because that's what ctypes.cast expects.
        size_t sample_ptr = reinterpret_cast<size_t>(
                (static_cast<CSampleWrapper&>(c_sample_buffer)).sample());
        return create_py_sample_func(type_support, sample_ptr);
    }
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

static IdlDataWriterPyObjectCache* convert_sample(
        dds::pub::DataWriter<CSampleWrapper>& writer,
        const py::object& sample)
{
    IdlDataWriterPyObjectCache *obj_cache = get_py_objects(writer);

    // Important: this is a call into Python; the caller must acquire the GIL
    obj_cache->convert_to_c_sample(sample);

    return obj_cache; // return the obj_cache for convenience
}

// The Write implementation for IDL writers converts Python samples to C
// before calling the actual write operation.
struct IdlWriteImpl {

    using py_sample = py::object;
    using gil_policy = py::gil_scoped_release;

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
        // Ensure that convert_sample and write are atomic by taking the writer
        // EA. The writer lock is taken outside the GIL to avoid deadlocks.
        rti::core::EntityLock lock_writer(writer);

        // Acquire the GIL, since convert_sample runs python code
        py::gil_scoped_acquire acquire_gil;

        // GIL: taken; Writer EA: taken
        auto obj_cache = convert_sample(writer, sample);

        // The native operation can run without the GIL
        py::gil_scoped_release release_gil_for_native_operation;

        // GIL: released; Writer EA: taken
        writer.extensions().write(
                obj_cache->c_sample_buffer,
                // call the appropriate overload of write()
                std::forward<ExtraArgs>(extra_args)...);
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
        rti::core::EntityLock lock_writer(writer);
        py::gil_scoped_acquire acquire_gil;
        auto obj_cache = convert_sample(writer, sample);

        py::gil_scoped_release release_gil_for_native_operation;
        return writer.extensions().register_instance(
                obj_cache->c_sample_buffer,
                std::forward<ExtraArgs>(extra_args)...);
    }

    static dds::core::InstanceHandle py_lookup_instance(
            IdlDataWriter& writer,
            py_sample key_holder)
    {
        rti::core::EntityLock lock_writer(writer);
        py::gil_scoped_acquire acquire_gil;
        // Entity lock + gil taken
        auto obj_cache = convert_sample(writer, key_holder);
        py::gil_scoped_release release_gil_for_native_operation;

        return writer.lookup_instance(obj_cache->c_sample_buffer);
    }
};

static dds::pub::qos::DataWriterQos get_modified_qos(
        const PyPublisher&,
        const PyTopic<CSampleWrapper>& topic,
        const dds::pub::qos::DataWriterQos& qos)
{
    using rti::core::policy::Property;
    static const char* pool_buffer_max_size_property_name =
            "dds.data_writer.history.memory_manager.fast_pool.pool_buffer_max_size";

    py::gil_scoped_acquire acquire;

    auto type_support = get_py_type_support_from_topic(topic);
    auto is_unbounded =
        py::cast<bool>(type_support.attr("is_unbounded"));
    if (!is_unbounded) {
        // If the type is not unbounded, we don't need to do anything
        return qos;
    }

    if (qos.policy<Property>().exists(pool_buffer_max_size_property_name)) {
        // If the property is already set, we honor that value
        return qos;
    }

    // The type is unbounded and the buffer size has not been set. We set it
    // to a predetermined value to support unbounded sequences out of the box.
    dds::pub::qos::DataWriterQos modified_qos = qos;
    modified_qos.policy<Property>().set({
        pool_buffer_max_size_property_name,
        "4096"
    });

    return modified_qos;
}

static void cache_idl_datawriter_py_objects(IdlDataWriter& writer)
{
    using rti::core::memory::ObjectAllocator;

    // Store the IdlDataWriterPyObjectCache as this writer's user data
    if (writer->get_user_data_() == nullptr) {
        py::gil_scoped_acquire acquire;

        // Obtain all the cached objects
        auto type_support = get_py_type_support_from_topic(writer.topic());
        auto obj_cache =
                ObjectAllocator<IdlDataWriterPyObjectCache>::create(type_support);
        writer->set_user_data_(obj_cache, [](void* ptr) {
            py::gil_scoped_acquire acquire; // we will finalize
            auto obj_cache = static_cast<IdlDataWriterPyObjectCache*>(ptr);
            obj_cache->finalize_c_sample();
            ObjectAllocator<IdlDataWriterPyObjectCache>::destroy(obj_cache);
        });
    }
}

static py::object py_key_value(
        IdlDataWriter& writer,
        dds::core::InstanceHandle handle)
{
    rti::core::EntityLock lock_writer(writer);
    py::gil_scoped_acquire acquire_gil;
    // Entity lock + gil taken
    IdlDataWriterPyObjectCache* obj_cache = get_py_objects(writer);
    writer.key_value(obj_cache->c_sample_buffer, handle);
    return obj_cache->create_py_sample();
}

void init_dds_idl_datawriter_constructors(IdlDataWriterPyClass& cls)
{
    // The constructors defined here are different from those defined for non
    // IDL types in a couple of ways:
    // - They automatically configure the Qos for unbounded support, if needed
    // - They cache the python objects needed for the py-to-C data conversions

    cls.def(py::init([](const PyPublisher& p,
                             const PyTopic<CSampleWrapper>& t) {
                dds::pub::qos::DataWriterQos modified_qos =
                        get_modified_qos(p, t, p.default_datawriter_qos());
                auto writer = IdlDataWriter(p, t, modified_qos);
                cache_idl_datawriter_py_objects(writer);
                return writer;
            }),
            py::arg("pub"),
            py::arg("topic"),
            py::call_guard<py::gil_scoped_release>(),
            "Creates a DataWriter.");

    cls.def(py::init([](const PyPublisher& p,
                             const PyTopic<CSampleWrapper>& t,
                             const dds::pub::qos::DataWriterQos& q,
                             PyDataWriterListenerPtr<CSampleWrapper> listener,
                             const dds::core::status::StatusMask& m) {
                dds::pub::qos::DataWriterQos modified_qos =
                        get_modified_qos(p, t, q);
                auto writer = IdlDataWriter(p, t, modified_qos, listener, m);
                cache_idl_datawriter_py_objects(writer);
                return writer;
            }),
            py::arg("pub"),
            py::arg("topic"),
            py::arg("qos"),
            py::arg("listener") = py::none(),
            py::arg_v(
                    "mask",
                    dds::core::status::StatusMask::all(),
                    "StatusMask.ALL"),
            py::call_guard<py::gil_scoped_release>(),
            "Creates a DataWriter with QoS and a listener.");
}

// Sets the Python methods for an IDL DataWriter
template<>
void init_dds_typed_datawriter_template(IdlDataWriterPyClass& cls)
{
    init_dds_idl_datawriter_constructors(cls);

    // Add the constructors that just do a cast; these don't need any
    // special functionality for IDL types.
    init_dds_datawriter_cast_constructors(cls);

    // These untyped methods do not require any customization for IDL support.
    init_dds_datawriter_untyped_methods(cls);

    // Initialize the write methods with a custom implementation of the write
    // operation that translates from Python objects to ctypes objects.
    init_dds_datawriter_write_methods<CSampleWrapper, IdlWriteImpl>(cls);

    cls.def("key_value",
            &py_key_value,
            py::arg("handle"),
            py::call_guard<py::gil_scoped_release>(),
            "Retrieve the instance key that corresponds to an instance "
            "handle.");
}

} // namespace pyrti