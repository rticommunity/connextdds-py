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

#include "IdlDataReader.hpp"


using namespace dds::core::xtypes;
using namespace dds::topic;
using namespace rti::topic::cdr;

namespace pyrti {

struct IdlDataReaderPyObjectCache {
    py::handle create_py_sample_func;
    py::handle cast_c_sample_func;
    py::handle type_support;
};

// Gets the Python objects associated to this writer
static IdlDataReaderPyObjectCache* get_py_objects(
        dds::sub::DataReader<CSampleWrapper>& reader)
{
    auto objects =
            static_cast<IdlDataReaderPyObjectCache*>(reader->get_user_data_());
    RTI_CHECK_PRECONDITION(objects != nullptr);
    return objects;
}

template <typename CreateSampleFunc>
py::object invoke_py_sample_function(
        CreateSampleFunc&& py_function,
        py::handle& type_support,
        const CSampleWrapper &sample)
{
    // We pass the pointer to the python function as an integer,
    // because that's what ctypes.cast expects.
    size_t sample_ptr = reinterpret_cast<size_t>(sample.sample());
    return py_function(type_support, sample_ptr);
}

static std::vector<py::object> convert_data(
        PyDataReader<CSampleWrapper>& dr,
        dds::sub::LoanedSamples<CSampleWrapper>&& samples)
{
    size_t max_length = samples.length();
    auto valid_samples = rti::sub::valid_data(std::move(samples));
    std::vector<py::object> py_samples;
    py_samples.reserve(max_length);

    py::gil_scoped_acquire acquire;
    // This is the type support function that converts from C data
    // to the user-facing python object.
    auto obj_cache = get_py_objects(dr);
    for (auto& sample : valid_samples) {
        py_samples.push_back(invoke_py_sample_function(
                obj_cache->create_py_sample_func,
                obj_cache->type_support,
                sample.data()));
    }

    return py_samples;
}

using DataAndInfoVector =
        std::vector<std::pair<py::object, dds::sub::SampleInfo>>;

static DataAndInfoVector convert_data_w_info(
        PyDataReader<CSampleWrapper>& dr,
        dds::sub::LoanedSamples<CSampleWrapper>&& samples)
{
    size_t max_length = samples.length();
    DataAndInfoVector py_samples;
    py_samples.reserve(max_length);

    py::gil_scoped_acquire acquire;
    // This is the type support function that converts from C data
    // to the user-facing python object.
    auto obj_cache = get_py_objects(dr);
    for (auto& sample : samples) {
        py::object py_data;
        if (sample.info().valid()) {
            py_data = invoke_py_sample_function(
                    obj_cache->create_py_sample_func,
                    obj_cache->type_support,
                    sample.data());
        } else {
            py_data = py::none();
        }
        py_samples.push_back({ py_data , sample.info() });
    }

    return py_samples;
}

static std::vector<size_t> get_native_ptrs(
        PyDataReader<CSampleWrapper>&,
        dds::sub::LoanedSamples<CSampleWrapper>&& samples)
{
    size_t max_length = samples.length();
    auto valid_samples = rti::sub::valid_data(std::move(samples));
    std::vector<size_t> py_samples(max_length);

    py::gil_scoped_acquire acquire;
    // This is the type support function that converts from C data
    // to the user-facing python object.
    size_t i = 0;
    for (auto& sample : valid_samples) {
        // py_samples.append(
        py_samples[i++] = reinterpret_cast<size_t>(sample.data().sample());
    }

    return py_samples;
}

static auto take_data(PyDataReader<CSampleWrapper>& dr)
{
    return convert_data(dr, dr.take());
}

static auto take_data_and_info(PyDataReader<CSampleWrapper>& dr)
{
    return convert_data_w_info(dr, dr.take());
}

static auto read_data(PyDataReader<CSampleWrapper>& dr)
{
    return convert_data(dr, dr.read());
}

static auto read_data_and_info(PyDataReader<CSampleWrapper>& dr)
{
    return convert_data_w_info(dr, dr.read());
}

static auto read_data_native(PyDataReader<CSampleWrapper>& dr)
{
    // TODO PY-17: Initial implementation only.
    return get_native_ptrs(dr, dr.read());
}

static auto take_selector_data(PyDataReader<CSampleWrapper>::Selector& selector)
{
    PyDataReader<CSampleWrapper> dr(selector.reader());
    return convert_data(dr, selector.take());
}

static auto take_selector_data_and_info(
        PyDataReader<CSampleWrapper>::Selector& selector)
{
    PyDataReader<CSampleWrapper> dr(selector.reader());
    return convert_data_w_info(dr, selector.take());
}

static auto read_selector_data(PyDataReader<CSampleWrapper>::Selector& selector)
{
    PyDataReader<CSampleWrapper> dr(selector.reader());
    return convert_data(dr, selector.read());
}

static auto read_selector_data_and_info(PyDataReader<CSampleWrapper>::Selector& selector)
{
    PyDataReader<CSampleWrapper> dr(selector.reader());
    return convert_data_w_info(dr, selector.read());
}

// For testing purposes only
static void test_native_reads(PyDataReader<CSampleWrapper>& dr)
{
    struct Point {
        int32_t x;
        int32_t y;
    };
    auto samples = rti::sub::valid_data(dr.read());
    for (auto& sample : samples) {
        auto point = reinterpret_cast<const Point*>(sample.data().sample());
        if (point->x != point->y) {
            throw std::runtime_error("Sample is not correct");
        }

    }
}

static dds::sub::qos::DataReaderQos get_modified_qos(
        const PySubscriber&,
        const dds::topic::Topic<CSampleWrapper>& topic,
        const dds::sub::qos::DataReaderQos& qos)
{
    using rti::core::policy::Property;
    static const char* pool_buffer_max_size_property_name =
            "dds.data_reader.history.memory_manager.fast_pool.pool_buffer_max_size";

    py::gil_scoped_acquire acquire;

    auto type_support = get_py_type_support_from_topic(topic);
    auto is_unbounded = py::cast<bool>(type_support.attr("is_unbounded"));
    auto is_keyed = py::cast<bool>(type_support.attr("is_keyed"));
    if (!is_unbounded || !is_keyed) {
        // If the type is bounded or unkeyed, we don't need to do anything
        return qos;
    }

    if (qos.policy<Property>().exists(pool_buffer_max_size_property_name)) {
        // If the property is already set, we honor that value
        return qos;
    }

    // The type is unbounded and the buffer size has not been set. We set it
    // to a predetermined value to support unbounded sequences out of the box.
    dds::sub::qos::DataReaderQos modified_qos = qos;
    modified_qos.policy<Property>().set({
        pool_buffer_max_size_property_name,
        "4096"
    });

    return modified_qos;
}

static void cache_idl_datareader_py_objects(
        PyDataReader<CSampleWrapper>& reader)
{
    using rti::core::memory::ObjectAllocator;

    // Store the IdlDataReaderPyObjectCache as this reader's user data
    if (reader->get_user_data_() == nullptr) {
        py::gil_scoped_acquire acquire;

        // Obtain all the cached objects
        auto type_support =
                get_py_type_support_from_topic(reader.topic_description());
        IdlDataReaderPyObjectCache tmp_obj_cache;
        tmp_obj_cache.type_support = type_support;
        tmp_obj_cache.create_py_sample_func =
                py::type::of(type_support).attr("_create_py_sample");
        tmp_obj_cache.cast_c_sample_func =
                py::type::of(type_support).attr("_cast_c_sample");

        // Once the objects have been successfully retrieved, allocate
        // the user data object and set it.
        auto obj_cache =
                ObjectAllocator<IdlDataReaderPyObjectCache>::create();
        *obj_cache = std::move(tmp_obj_cache);
        reader->set_user_data_(obj_cache, [](void* ptr) {
            ObjectAllocator<IdlDataReaderPyObjectCache>::destroy(
                    static_cast<IdlDataReaderPyObjectCache*>(ptr));
        });
    }
}


void init_dds_idl_datareader_constructors(IdlDataReaderPyClass& cls)
{
    // The constructors defined here are different from those defined for non
    // IDL types in a couple of ways:
    // - They automatically configure the Qos for unbounded support, if needed
    // - They cache the python objects needed for the C-to-py data conversions
    cls.def(py::init([](const PySubscriber& s,
                             const PyTopic<CSampleWrapper>& t) {
                auto modified_qos =
                        get_modified_qos(s, t, s.default_datareader_qos());
                auto reader = PyIdlDataReader(s, t, modified_qos);
                cache_idl_datareader_py_objects(reader);
                return reader;
            }),
            py::arg("sub"),
            py::arg("topic"),
            py::call_guard<py::gil_scoped_release>(),
            "Create a DataReader.");

    cls.def(py::init([](const PySubscriber& s,
                             const PyTopic<CSampleWrapper>& t,
                             const dds::sub::qos::DataReaderQos& q,
                             PyDataReaderListenerPtr<CSampleWrapper> listener,
                             const dds::core::status::StatusMask& m) {
                auto modified_qos = get_modified_qos(s, t, q);
                auto reader = PyIdlDataReader(s, t, modified_qos, listener, m);
                cache_idl_datareader_py_objects(reader);
                return reader;
            }),
            py::arg("sub"),
            py::arg("topic"),
            py::arg("qos"),
            py::arg("listener") = py::none(),
            py::arg_v(
                    "mask",
                    dds::core::status::StatusMask::all(),
                    "StatusMask.ALL"),
            py::call_guard<py::gil_scoped_release>(),
            "Create a DataReader.");

    cls.def(py::init([](const PySubscriber& s,
                             const PyContentFilteredTopic<CSampleWrapper>& cft) {
                auto modified_qos = get_modified_qos(
                        s,
                        cft.topic(),
                        s.default_datareader_qos());
                auto reader = PyIdlDataReader(s, cft, modified_qos);
                cache_idl_datareader_py_objects(reader);
                return reader;
            }),
            py::arg("sub"),
            py::arg("cft"),
            py::call_guard<py::gil_scoped_release>(),
            "Create a DataReader with a ContentFilteredTopic.");

    cls.def(py::init([](const PySubscriber& s,
                             const PyContentFilteredTopic<CSampleWrapper>& cft,
                             const dds::sub::qos::DataReaderQos& q,
                             PyDataReaderListenerPtr<CSampleWrapper> listener,
                             const dds::core::status::StatusMask& m) {
                auto modified_qos = get_modified_qos(s, cft.topic(), q);
                auto reader = PyIdlDataReader(s, cft, modified_qos, listener, m);
                cache_idl_datareader_py_objects(reader);
                return reader;
            }),
            py::arg("sub"),
            py::arg("cft"),
            py::arg("qos"),
            py::arg("listener") = py::none(),
            py::arg_v(
                    "mask",
                    dds::core::status::StatusMask::all(),
                    "StatusMask.ALL"),
            py::call_guard<py::gil_scoped_release>(),
            "Create a DataReader with a ContentFilteredTopic.");
}

template<>
void init_dds_datareader_selector_read_methods<CSampleWrapper>(
        py::class_<typename PyDataReader<CSampleWrapper>::Selector>& selector)
{
    selector.def(
            "read",
            read_selector_data_and_info,
            py::call_guard<py::gil_scoped_release>(),
            "Read copies of all available data and info based on Selector "
            "settings.");
    selector.def(
            "read_data",
            read_selector_data,
            py::call_guard<py::gil_scoped_release>(),
            "Read copies of all available valid data based on Selector settings.");
    selector.def(
            "take",
            take_selector_data_and_info,
            py::call_guard<py::gil_scoped_release>(),
            "Take copies of all available data and info based on Selector "
            "settings.");
    selector.def(
            "take_data",
            take_selector_data,
            py::call_guard<py::gil_scoped_release>(),
            "Take copies of all available valid data based on Selector "
            "settings.");
}

template<>
void init_dds_typed_datareader_template(IdlDataReaderPyClass& cls)
{
    // These constructors have a specific implementation for IDL types
    init_dds_idl_datareader_constructors(cls);

    // These constructors are the same for all types
    init_dds_datareader_cast_constructors(cls);

    init_dds_typed_datareader_base_template(cls);

    py::class_<typename PyDataReader<CSampleWrapper>::Selector> selector(
            cls,
            "Selector");
    init_dds_datareader_selector_base<CSampleWrapper>(selector);
    init_dds_datareader_selector_read_methods<CSampleWrapper>(selector);

    cls.def("take_data",
            take_data,
            py::call_guard<py::gil_scoped_release>(),
            "Take copies of all available valid data");

    cls.def("take",
            take_data_and_info,
            py::call_guard<py::gil_scoped_release>(),
            "Take copies of all available data and info");

    cls.def("read_data",
            read_data,
            py::call_guard<py::gil_scoped_release>(),
            "Read copies of all available valid data");

    cls.def("read",
            read_data_and_info,
            py::call_guard<py::gil_scoped_release>(),
            "Read copies of all available data and info");

    cls.def(
            "select",
            [](PyDataReader<CSampleWrapper>& dr) {
                return typename PyDataReader<CSampleWrapper>::Selector(dr.select());
            },
            py::call_guard<py::gil_scoped_release>(),
            "Get a Selector to perform complex data selections, such "
            "as "
            "per-instance selection, content, and status filtering.");

    cls.def("read_data_native",
            read_data_native,
            py::call_guard<py::gil_scoped_release>(),
            "(Advanced) Read data in C format");

    // TODO PY-17: For testing purposes only. Remove in final version.
    cls.def("_test_native_reads",
            test_native_reads,
            py::call_guard<py::gil_scoped_release>(),
            "For testing purposes only");
}

} // namespace pyrti