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
#include <rti/core/EntityLock.hpp>
#include "IdlDataReader.hpp"
#include "IdlTypeSupport.hpp"
#include "PyLoanedSample.hpp"
#include "PyLoanedSamples.hpp"

using namespace dds::core::xtypes;
using namespace dds::topic;
using namespace rti::topic::cdr;

namespace pyrti {

// Gets the Python objects associated to this reader
static CPySampleConverter* get_py_objects(
        dds::sub::DataReader<CSampleWrapper>& reader)
{
    auto objects = static_cast<CPySampleConverter*>(reader->get_user_data_());
    RTI_CHECK_PRECONDITION(objects != nullptr);
    return objects;
}

static CPySampleConverter* convert_sample(
        dds::sub::DataReader<CSampleWrapper>& reader,
        const py::object& sample)
{
    CPySampleConverter* obj_cache = get_py_objects(reader);

    // Important: this is a call into Python; the caller must acquire the GIL
    obj_cache->convert_to_c_sample(sample);

    return obj_cache;  // return the obj_cache for convenience
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

static py::list convert_data(
        PyDataReader<CSampleWrapper>& dr,
        dds::sub::LoanedSamples<CSampleWrapper>&& samples)
{
    size_t max_length = samples.length();
    auto valid_samples = rti::sub::valid_data(std::move(samples));

    py::gil_scoped_acquire acquire;
    py::list py_samples(max_length);
    auto obj_cache = get_py_objects(dr);
    size_t i = 0;
    for (auto& sample : valid_samples) {
        // Create a Python sample from the C one
        py_samples[i++] = invoke_py_sample_function(
                obj_cache->create_py_sample_func,
                obj_cache->type_support,
                sample.data());
    }

    if (i < max_length) {
        // If any samples had invalid data, we need to shrink the list;
        // SetSlice with a null itemlist argument does that.
        if (PyList_SetSlice(py_samples.ptr(), i, max_length, nullptr) != 0) {
            throw py::error_already_set();
        }
    }

    return py_samples;
}

using DataAndInfoVector =
        std::vector<std::pair<py::object, dds::sub::SampleInfo>>;

static py::list convert_data_w_info(
        PyDataReader<CSampleWrapper>& dr,
        dds::sub::LoanedSamples<CSampleWrapper>&& samples)
{
    size_t length = samples.length();
    py::gil_scoped_acquire acquire;
    py::list py_samples(length);
    //     py_samples.reserve(max_length);

    // This is the type support function that converts from C data
    // to the user-facing python object.
    auto obj_cache = get_py_objects(dr);

    size_t i = 0;
    for (auto& sample : samples) {
        const auto& info = sample.info();
        py::object py_data;
        if (info.valid()) {
            py_data = invoke_py_sample_function(
                    obj_cache->create_py_sample_func,
                    obj_cache->type_support,
                    sample.data());
        } else {
            py_data = py::none();
        }
        py_samples[i++] = obj_cache->create_py_data_info_sample(py_data, info);
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

// These two functions are defined only to provide autocompletion. The actual
// implementation is in Python code and set by importing rti.asyncio.

static py::object take_data_async(
        PyDataReader<CSampleWrapper>&,
        PyIReadCondition* = nullptr)
{
    throw dds::core::PreconditionNotMetError(
            "To use take_data_async, you need to import rti.asyncio");
}

static py::object take_data_and_info_async(
        PyDataReader<CSampleWrapper>&,
        PyIReadCondition * = nullptr)
{
    throw dds::core::PreconditionNotMetError(
            "To use take_async, you need to import rti.asyncio");
}

static auto read_data(PyDataReader<CSampleWrapper>& dr)
{
    return convert_data(dr, dr.read());
}

static auto read_data_and_info(PyDataReader<CSampleWrapper>& dr)
{
    return convert_data_w_info(dr, dr.read());
}

static auto read_native(PyDataReader<CSampleWrapper>& dr)
{
    return dr.read();
}

static auto take_native(PyDataReader<CSampleWrapper>& dr)
{
    return dr.take();
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

static auto read_selector_native(
        PyDataReader<CSampleWrapper>::Selector& selector)
{
    return selector.read();
}

static auto take_selector_native(
        PyDataReader<CSampleWrapper>::Selector& selector)
{
    return selector.take();
}

static py::object py_key_value(
        PyDataReader<CSampleWrapper>& reader,
        dds::core::InstanceHandle handle)
{
    rti::core::EntityLock lock_reader(reader);
    py::gil_scoped_acquire acquire_gil;
    // Entity lock + gil taken
    CPySampleConverter* obj_cache = get_py_objects(reader);
    reader.key_value(obj_cache->c_sample_buffer, handle);
    return obj_cache->create_py_sample();
}

static dds::core::InstanceHandle py_lookup_instance(
        PyDataReader<CSampleWrapper>& reader,
        py::object sample)
{
    rti::core::EntityLock lock_reader(reader);
    py::gil_scoped_acquire acquire_gil;
    // Entity lock + gil taken
    CPySampleConverter* obj_cache = convert_sample(reader, sample);
    py::gil_scoped_release release_gil_for_native_operation;

    return reader.lookup_instance(obj_cache->c_sample_buffer);
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

PyIdlDataReader create_idl_py_reader(
        const PySubscriber& subscriber,
        const PyTopic<CSampleWrapper>& topic,
        const PyContentFilteredTopic<CSampleWrapper>& cft,
        const dds::sub::qos::DataReaderQos* qos,
        PyDataReaderListenerPtr<CSampleWrapper>* listener,
        const dds::core::status::StatusMask& mask)
{
    // This function performs a few additional initialization steps compared to
    // the C++ constructors:
    // - Configures support for unbounded types via Qos
    // - Caches the Python objects required for converting Python/C samples in
    //   the reader read operations (and other operations)

    auto modified_qos = get_modified_qos(
            subscriber,
            cft == dds::core::null ? topic : cft.topic(),
            qos != nullptr ? *qos : subscriber.default_datareader_qos());

    PyIdlDataReader reader = dds::core::null;
    if (cft == dds::core::null) {
        reader = listener != nullptr
                ? PyIdlDataReader(
                        subscriber,
                        topic,
                        modified_qos,
                        *listener,
                        mask)
                : PyIdlDataReader(subscriber, topic, modified_qos);
    } else {
        reader = listener != nullptr
                ? PyIdlDataReader(
                        subscriber,
                        cft,
                        modified_qos,
                        *listener,
                        mask)
                : PyIdlDataReader(subscriber, cft, modified_qos);
    }

    CPySampleConverter::cache_idl_entity_py_objects(
            reader,
            reader.topic_description());
    return reader;
}


void init_dds_idl_datareader_constructors(IdlDataReaderPyClass& cls)
{
    // The constructors defined here are different from those defined for non
    // IDL types in a couple of ways:
    // - They automatically configure the Qos for unbounded support, if needed
    // - They cache the python objects needed for the C-to-py data conversions
    cls.def(py::init([](const PySubscriber& s,
                             const PyTopic<CSampleWrapper>& t) {
                return create_idl_py_reader(s, t);
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
                return create_idl_py_reader(
                        s,
                        t,
                        dds::core::null,
                        &q,
                        &listener,
                        m);
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

    cls.def(py::init(
                    [](const PySubscriber& s,
                            const PyContentFilteredTopic<CSampleWrapper>& cft) {
                        return create_idl_py_reader(s, dds::core::null, cft);
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
                return create_idl_py_reader(
                        s,
                        dds::core::null,
                        cft,
                        &q,
                        &listener,
                        m);
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
        py::class_<PyDataReader<CSampleWrapper>::Selector>& selector)
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

    selector.def(
            "read_loaned",
            read_selector_native,
            py::call_guard<py::gil_scoped_release>(),
            "(Advanced) Read data as a collection of loaned data in C "
            "format and info objects based on Selector settings");
    selector.def(
            "take_loaned",
            take_selector_native,
            py::call_guard<py::gil_scoped_release>(),
            "(Advanced) Take data as a collection of loaned data in C "
            "format and info objects based on Selector settings");
}

static void init_csamplewrapper_loaned_samples(IdlDataReaderPyClass& cls)
{
    py::class_<rti::sub::LoanedSample<CSampleWrapper>> loaned_sample_cls(
        cls,
        "LoanedSample");

    init_loaned_sample(loaned_sample_cls);

    py::class_<
            dds::sub::LoanedSamples<CSampleWrapper>,
            std::unique_ptr<
                    dds::sub::LoanedSamples<CSampleWrapper>,
                    no_gil_delete<dds::sub::LoanedSamples<CSampleWrapper>>>>
            loaned_samples_cls(cls, "LoanedSamples");

    init_loaned_samples<CSampleWrapper>(loaned_samples_cls);
}

template<>
void init_dds_typed_datareader_template(IdlDataReaderPyClass& cls)
{
    init_csamplewrapper_loaned_samples(cls);

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

    cls.def("take_data_async",
            take_data_async,
            py::arg("condition") = py::none(),
            py::call_guard<py::gil_scoped_release>(),
            "Takes copies of data as it becomes available in an async "
            "generator. Must ``import rti.asyncio``.\n\n"
            "Example:\n\n"
            "  .. code-block:: python\n\n"
            "    async for data in reader.take_data_async():\n"
            "      print(data)\n"
            "\n\n"
            "A ReadCondition or QueryCondition can be used to filter the data\n");

    cls.def("take_async",
            take_data_and_info_async,
            py::arg("condition") = py::none(),
            py::call_guard<py::gil_scoped_release>(),
            "Takes copies of data, info tuples as data becomes available in an "
            "async generator. Must ``import rti.asyncio``.\n\n"
            "Example:\n\n"
            "  .. code-block:: python\n\n"
            "    async for data, info in reader.take_async():\n"
            "        if info.valid:\n"
            "            print(data)\n"
            "        else:\n"
            "            print(info.state.instance_state)\n"
            "\n\n"
            "A ReadCondition or QueryCondition can be used to filter the data\n");

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
                return typename PyDataReader<CSampleWrapper>::Selector(
                        dr.select());
            },
            py::call_guard<py::gil_scoped_release>(),
            "Get a Selector to perform complex data selections, such "
            "as "
            "per-instance selection, content, and status filtering.");

    cls.def("key_value",
            &py_key_value,
            py::arg("handle"),
            py::call_guard<py::gil_scoped_release>(),
            "Retrieve the instance key that corresponds to an instance "
            "handle.");

    cls.def("lookup_instance",
            &py_lookup_instance,
            py::arg("key_holder"),
            py::call_guard<py::gil_scoped_release>(),
            "Retrieve the instance handle that corresponds to an instance "
            "key_holder");

    cls.def("take_loaned",
            take_native,
            py::call_guard<py::gil_scoped_release>(),
            "(Advanced) Take all data as a collection of loaned data in C "
            "format and info objects");

    cls.def("read_loaned",
            read_native,
            py::call_guard<py::gil_scoped_release>(),
            "(Advanced) Read all data as a collection of loaned data in C format "
            "and info objects");
}

} // namespace pyrti