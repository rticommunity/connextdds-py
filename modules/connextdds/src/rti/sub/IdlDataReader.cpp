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

auto get_create_py_sample_function(PyDataReader<CSampleWrapper> &dr)
{
    // TODO PY-17: support CFT
    auto topic = dds::core::polymorphic_cast<dds::topic::Topic<CSampleWrapper>>(
            dr.topic_description());
    auto type_support = get_py_type_support_from_topic(topic);
    return type_support.attr("_create_py_sample");
}

template <typename CreateSampleFunc>
py::object invoke_create_py_sample(
        CreateSampleFunc&& create_py_sample,
        const CSampleWrapper &sample)
{
    // We pass the pointer to the python function as an integer,
    // because that's what ctypes.cast expects.
    size_t sample_ptr = reinterpret_cast<size_t>(sample.sample);
    return create_py_sample(sample_ptr);
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
    auto create_py_sample = get_create_py_sample_function(dr);
    for (auto& sample : valid_samples) {
        py_samples.push_back(
                invoke_create_py_sample(create_py_sample, sample.data()));
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
    auto valid_samples = rti::sub::valid_data(std::move(samples));
    DataAndInfoVector py_samples;
    py_samples.reserve(max_length);

    py::gil_scoped_acquire acquire;
    // This is the type support function that converts from C data
    // to the user-facing python object.
    auto create_py_sample = get_create_py_sample_function(dr);
    for (auto& sample : valid_samples) {
        py::object py_data =
                invoke_create_py_sample(create_py_sample, sample.data());
        py_samples.push_back({ py_data , sample.info() });
    }

    return py_samples;
}

static py::list convert_data_py(
        PyDataReader<CSampleWrapper>& dr,
        dds::sub::LoanedSamples<CSampleWrapper>&& samples)
{
    size_t max_length = samples.length();
    auto valid_samples = rti::sub::valid_data(std::move(samples));
    py::list py_samples(max_length);

    py::gil_scoped_acquire acquire;
    // This is the type support function that converts from C data
    // to the user-facing python object.
    auto create_py_sample = get_create_py_sample_function(dr);
    size_t i = 0;
    for (auto& sample : valid_samples) {
        // py_samples.append(
        py_samples[i++] =
                invoke_create_py_sample(create_py_sample, sample.data());
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

template<>
void init_dds_typed_datareader_template(IdlDataReaderPyClass& cls)
{
    init_dds_typed_datareader_base_template(cls);

    cls.def("take_data",
            take_data,
            py::call_guard<py::gil_scoped_release>(),
            "Take copies of all available valid data");

    cls.def("take",
            take_data_and_info,
            py::call_guard<py::gil_scoped_release>(),
            "Take copies of all available valid data");

    cls.def("read_data",
            read_data,
            py::call_guard<py::gil_scoped_release>(),
            "Take copies of all available valid data");

    cls.def("read",
            read_data_and_info,
            py::call_guard<py::gil_scoped_release>(),
            "Take copies of all available valid data");
}

} // namespace pyrti