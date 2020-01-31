#pragma once

#include <pybind11/pybind11.h>
#include <dds/sub/SharedSamples.hpp>
#include <rti/rti.hpp>

namespace py = pybind11;

namespace pyrti {

template<typename T>
void init_shared_samples_defs(py::class_<dds::sub::SharedSamples<T>>& cls) {
    cls
        .def(
            py::init<
                dds::sub::LoanedSamples<T>&
            >(),
            py::arg("loaned_samples"),
            "Constructs an instance of SharedSamples, removing ownership of the loan from the Loaned Samples."
        )
        .def(
            "__len__",
            &dds::sub::SharedSamples<T>::length,
            "Returns the number of samples."
        )
        .def(
            "__getitem__",
            [](dds::sub::SharedSamples<T>& samples, size_t index) {
                if (index >= samples.length()) throw py::index_error();
                return samples[index];
            },
            "Get the sample at the specified index."
        )
        .def(
            "__iter__",
            [](dds::sub::SharedSamples<T>& samples) {
                return py::make_iterator(samples.begin(), samples.end());
            },
            py::keep_alive<0, 1>(),
            "Make a sample iterator"
        )
        .def(
            "unpack",
            [](dds::sub::SharedSamples<T>& samples) {
                return rti::sub::unpack(samples);
            },
            "Unpacks all samples into a list."
        );
}

template<typename T>
void init_shared_samples(py::object& o) {
    py::class_<dds::sub::SharedSamples<T>> ss(o, "SharedSamples");

    init_shared_samples_defs(ss);
}

}
