#pragma once

#include "PyConnext.hpp"
#include <rti/sub/LoanedSamplesImpl.hpp>
#include <dds/sub/LoanedSamples.hpp>

namespace pyrti {

template<typename T>
void init_valid_loaned_samples_def(py::class_<rti::sub::ValidLoanedSamples<T>>& cls) {
    cls
        .def(
            "__iter__",
            [](const rti::sub::ValidLoanedSamples<T>& ls) {
                return py::make_iterator(ls.begin(), ls.end());
            },
            // Keep dds::sub::ValidLoanedSamples until iterator is done.
            py::keep_alive<0, 1>()
        )
        .def(
            "__enter__",
            [](rti::sub::ValidLoanedSamples<T>& ls) -> rti::sub::ValidLoanedSamples<T>& {
                return ls;
            }
        )
        .def(
            "__exit__",
            [](rti::sub::ValidLoanedSamples<T>& ls, py::object, py::object, py::object) {}
        );
}

template<typename T>
void init_valid_loaned_samples(py::object& o) {
    py::class_<rti::sub::ValidLoanedSamples<T>> vls(o, "ValidLoanedSamples");
    
    init_valid_loaned_samples_def(vls);
}

}