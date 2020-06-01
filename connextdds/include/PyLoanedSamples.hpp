#pragma once

#include <pybind11/pybind11.h>
#include <dds/sub/LoanedSamples.hpp>

namespace py = pybind11;

namespace pyrti {

template<typename T>
void init_loaned_samples_defs(py::class_<dds::sub::LoanedSamples<T>>& cls) {
    cls
        .def(
            py::init<>(),
            "Create an empty LoanedSamples object."
        )
        .def(
            "__getitem__",
            [](dds::sub::LoanedSamples<T>& ls, size_t index) {
                if (index >= ls.length()) throw py::index_error();
                return ls[index];
            },
            "Access a LoanedSample object in an array-like syntax"
        )
        .def("__len__",
            &dds::sub::LoanedSamples<T>::length,
            "Get the number of samples in the loan."
        )
        .def_property_readonly("length",
            &dds::sub::LoanedSamples<T>::length,
            "Get the number of samples in the loan."
        )
        .def(
            "return_loan",
            &dds::sub::LoanedSamples<T>::return_loan,
            "Returns the loan to the DataReader."
        )
        .def(
            "__iter__",
            [](dds::sub::LoanedSamples<T>& ls) {
                return py::make_iterator(ls.begin(), ls.end());
            },
            // Keep dds::sub::LoanedSamples until iterator is done.
            py::keep_alive<0, 1>()
        )
        .def(
            "__enter__",
            [](dds::sub::LoanedSamples<T>& ls) -> dds::sub::LoanedSamples<T>& {
                return ls;
            },
            "Enter a context for the loaned samples, loan returned on context exit."
        )
        .def(
            "__exit__",
            [](dds::sub::LoanedSamples<T>& ls, py::object, py::object, py::object) {
                ls.return_loan();
            },
            "Exit the context for the loaned samples, returning the resources."
        );
}

template<typename T>
void init_loaned_samples(py::class_<dds::sub::LoanedSamples<T>>& ls) {
    init_loaned_samples_defs(ls);
}

}