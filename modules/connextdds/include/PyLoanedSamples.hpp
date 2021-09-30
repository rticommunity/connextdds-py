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
#include <dds/sub/LoanedSamples.hpp>

namespace py = pybind11;

namespace pyrti {

template<typename T>
void init_loaned_samples_defs(
    py::class_<
        dds::sub::LoanedSamples<T>,
        std::unique_ptr<dds::sub::LoanedSamples<T>, no_gil_delete<dds::sub::LoanedSamples<T>>>>& cls)
{
    cls.def(py::init<>(), "Create an empty LoanedSamples object.")
            .def(
                    "__getitem__",
                    [](dds::sub::LoanedSamples<T>& ls, size_t index) {
                        if (index >= ls.length())
                            throw py::index_error();
                        return ls[index];
                    },
                    "Access a LoanedSample object in an array-like syntax")
            .def("__len__",
                    &dds::sub::LoanedSamples<T>::length,
                    "Get the number of samples in the loan.")
            .def_property_readonly(
                    "length",
                    &dds::sub::LoanedSamples<T>::length,
                    "Get the number of samples in the loan.")
            .def("return_loan",
                    &dds::sub::LoanedSamples<T>::return_loan,
                    py::call_guard<py::gil_scoped_release>(),
                    "Returns the loan to the DataReader.")
            .def(
                    "__iter__",
                    [](dds::sub::LoanedSamples<T>& ls) {
                        return py::make_iterator(ls.begin(), ls.end());
                    },
                    // Keep dds::sub::LoanedSamples until iterator is done.
                    py::keep_alive<0, 1>())
            .def(
                    "__enter__",
                    [](dds::sub::LoanedSamples<T>& ls)
                            -> dds::sub::LoanedSamples<T>& { return ls; },
                    "Enter a context for the loaned samples, loan returned on "
                    "context exit.")
            .def(
                    "__exit__",
                    [](dds::sub::LoanedSamples<T>& ls,
                       py::object,
                       py::object,
                       py::object) { ls.return_loan(); },
                    py::call_guard<py::gil_scoped_release>(),
                    "Exit the context for the loaned samples, returning the "
                    "resources.");
}

template<typename T>
void init_loaned_samples(
        py::class_<
            dds::sub::LoanedSamples<T>,
            std::unique_ptr<dds::sub::LoanedSamples<T>, no_gil_delete<dds::sub::LoanedSamples<T>>>>& ls)
{
    init_loaned_samples_defs(ls);
}

}  // namespace pyrti