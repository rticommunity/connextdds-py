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
    if (!py::hasattr(type_support, "is_valid_topic_type")) {
        throw py::type_error(
                "Incompatible 'type' argument: an @idl.struct or @idl.union is "
                "required");
    }

    if (!py::cast<bool>(type_support.attr("is_valid_topic_type"))) {
        throw py::type_error(
                "Incompatible 'type' argument: an @idl.struct or @idl.union is "
                "required");
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

    // Obtain the actual buffer as a rti::topic::cdr::CSampleWrapper, as required by
    // PyDataWriter::py_write
    operator rti::topic::cdr::CSampleWrapper&()
    {
        return *reinterpret_cast<rti::topic::cdr::CSampleWrapper*>(
                py_buffer.buf);
    }
};
// Gets the python TypeSupport from a C++ Topic object, which is stored in its
// user data.
inline static py::handle get_py_type_support_from_topic(
        const dds::topic::TopicDescription<rti::topic::cdr::CSampleWrapper>&
                topic)
{
    // TODO PY-17: The type support must be cached in the writer and reader
    // instead of looked up every time
    using namespace dds::topic;
    using namespace rti::topic::cdr;
    PyObject* user_data = static_cast<PyObject*>(topic->get_user_data_());
    if (user_data == nullptr) {
        // If it's null it means that it's of type cft
        try {
            user_data = static_cast<PyObject*>(
                    dds::core::polymorphic_cast<
                            ContentFilteredTopic<CSampleWrapper>,
                            TopicDescription<CSampleWrapper>>(topic)
                            ->topic()
                            ->get_user_data_());
        } catch (const dds::core::InvalidDowncastError&) {
            user_data = nullptr;
        }
    }

    if (user_data == nullptr) {
        throw dds::core::IllegalOperationError("Not a valid Python Topic");
    }

    auto type_support = py::handle(user_data);

#ifndef NDEBUG
    // this shouldn't fail; for performance reasons, check this only in debug
    // mode
    assert_valid_type_support(type_support);
#endif

    return type_support;
}

// Here we change the visibility because the py::objects have lower visibility
// and without this we will get compiler warnings
struct PYRTI_SYMBOL_HIDDEN CPySampleConverter {
    py::handle type_support; // TypeSupport object associated to the user type
    rti::topic::cdr::CTypePlugin* type_plugin;
    // The following are direct references to attributes of type_support
    py::handle sample_tuple_type;
    py::handle create_py_sample_func;
    py::handle create_c_sample_func;
    py::handle convert_to_c_sample_func;
    py::object c_sample;  // Reusable ctypes sample used to temporarily convert
                          // a python object into its C representation
    PyCTypesBuffer c_sample_buffer;  // This buffer points to the memory of
                                     // c_sample

    CPySampleConverter(py::handle the_type_support)
            : type_support(the_type_support),
              type_plugin(py::cast<TypePlugin>(
                                  type_support.attr("_plugin_dynamic_type"))
                                  .type_plugin),
              sample_tuple_type(type_support.attr("sample_type")),
              create_py_sample_func(
                      py::type::of(type_support).attr("_create_py_sample")),
              create_c_sample_func(py::type::of(type_support)
                                           .attr("_create_empty_c_sample")),
              convert_to_c_sample_func(
                      py::type::of(type_support).attr("_convert_to_c_sample")),
              c_sample(create_c_sample_func(type_support)),
              c_sample_buffer(c_sample)
    {
    }

    // Disable copies
    CPySampleConverter(const CPySampleConverter&) = delete;
    CPySampleConverter& operator=(const CPySampleConverter&) = delete;
    // Default move
    CPySampleConverter(CPySampleConverter&&) = default;
    CPySampleConverter& operator=(CPySampleConverter&&) = default;

    void convert_to_c_sample(const py::object& py_sample)
    {
        convert_to_c_sample_func(type_support, c_sample, py_sample);
    }

    ~CPySampleConverter()
    {
        try {
            finalize_c_sample();
        } catch (...) {
            // Ignore exceptions
        }
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
                (static_cast<rti::topic::cdr::CSampleWrapper&>(c_sample_buffer))
                        .sample());
        return create_py_sample_func(type_support, sample_ptr);
    }

    py::object create_py_data_info_sample(
        py::object& py_data,
        const dds::sub::SampleInfo& info)
    {
        return sample_tuple_type(py_data, py::cast(info));
    }

    static void cache_idl_entity_py_objects(
            dds::core::Entity entity,
            const dds::topic::TopicDescription<
                    rti::topic::cdr::CSampleWrapper>& topic)
    {
        using rti::core::memory::ObjectAllocator; 

        // Store the CPySampleConverter as this reader's user data
        if (entity->get_user_data_() == nullptr) {
            py::gil_scoped_acquire acquire;

            // Obtain all the cached objects
            auto type_support =
                    get_py_type_support_from_topic(topic);
            auto obj_cache =
                    ObjectAllocator<CPySampleConverter>::create(type_support);
            entity->set_user_data_(obj_cache, [](void* ptr) {
                py::gil_scoped_acquire acquire;
                auto obj_cache = static_cast<CPySampleConverter*>(ptr);
                obj_cache->finalize_c_sample();
                ObjectAllocator<CPySampleConverter>::destroy(
                        static_cast<CPySampleConverter*>(ptr));
            });
        }
    }
};

// Implements the creation of IDL endpoints for XML App Creation
class OMG_DDS_API PyFactoryIdlPluginSupport 
    : private rti::domain::FactoryPluginSupport {
public:
    using rti::domain::FactoryPluginSupport::is_set;

    static DDS_Topic* create_topic(
            DDS_DomainParticipant* cParticipant,
            const char* topic_name,
            const char* type_name,
            const struct DDS_TopicQos* qos,
            const ListenerInfo listenerInfo,
            DDS_StatusMask mask);

    static DDS_TopicDescription* create_content_filtered_topic(
            DDS_DomainParticipant* participant,
            const char* name,
            DDS_Topic* related_topic,
            const char* filter_expression,
            const struct DDS_StringSeq* filter_parameters,
            const char* filter_name);

    static DDS_DataWriter* create_datawriter(
            DDS_Publisher* publisher,
            DDS_Topic* topic,
            const struct DDS_DataWriterQos* qos,
            const ListenerInfo listenerInfo,
            DDS_StatusMask mask);

    static DDS_DataReader* create_datareader(
            DDS_Subscriber* subscriber,
            DDS_TopicDescription* topicDescription,
            const struct DDS_DataReaderQos* qos,
            const ListenerInfo listenerInfo,
            DDS_StatusMask mask);

    static void create_factory_plugin_support(
            DDS_FactoryPluginSupport* plugin_support);
};

// Registers a Python class with the DPF so it can be referenced in XML-defined
// topics
void register_idl_type(
        py::object& type,
        const std::string& type_name);

}  // namespace pyrti