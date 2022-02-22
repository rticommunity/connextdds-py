/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */

#pragma once

#include <pybind11/pybind11.h>
#include <dds/sub/SharedSamples.hpp>
#include <rti/rti.hpp>

namespace py = pybind11;

namespace pyrti {

template<typename T>
void init_shared_samples_defs(
    py::class_<
        dds::sub::SharedSamples<T>,
        std::unique_ptr<dds::sub::SharedSamples<T>, no_gil_delete<dds::sub::SharedSamples<T>>>>& cls)
{
    cls.def(py::init<dds::sub::LoanedSamples<T>&>(),
            py::arg("loaned_samples"),
            "Constructs an instance of SharedSamples, removing ownership of "
            "the loan from the Loaned Samples.")
            .def("__len__",
                 &dds::sub::SharedSamples<T>::length,
                 "Returns the number of samples.")
            .def(
                    "__getitem__",
                    [](dds::sub::SharedSamples<T>& samples, size_t index) {
                        if (index >= samples.length())
                            throw py::index_error();
                        return samples[index];
                    },
                    py::return_value_policy::reference_internal,
                    "Get the sample at the specified index.")
            .def(
                    "__iter__",
                    [](dds::sub::SharedSamples<T>& samples) {
                        return py::make_iterator(
                                samples.begin(),
                                samples.end());
                    },
                    py::keep_alive<0, 1>(),
                    "Make a sample iterator");
}

template<typename T>
void init_shared_samples(
    py::class_<
        dds::sub::SharedSamples<T>,
        std::unique_ptr<dds::sub::SharedSamples<T>, no_gil_delete<dds::sub::SharedSamples<T>>>>& ss)
{
    init_shared_samples_defs(ss);
}

}  // namespace pyrti
