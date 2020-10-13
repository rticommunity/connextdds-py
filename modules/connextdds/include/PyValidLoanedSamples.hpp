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

#include "PyConnext.hpp"
#include <rti/sub/LoanedSamplesImpl.hpp>
#include <dds/sub/LoanedSamples.hpp>

namespace pyrti {

template<typename T>
void init_valid_loaned_samples_def(
        py::class_<rti::sub::ValidLoanedSamples<T>>& cls)
{
    cls.def(
               "__iter__",
               [](const rti::sub::ValidLoanedSamples<T>& ls) {
                   return py::make_iterator(ls.begin(), ls.end());
               },
               // Keep dds::sub::ValidLoanedSamples until iterator is done.
               py::keep_alive<0, 1>())
            .def("__enter__",
                 [](rti::sub::ValidLoanedSamples<T>& ls)
                         -> rti::sub::ValidLoanedSamples<T>& { return ls; })
            .def("__exit__",
                 [](rti::sub::ValidLoanedSamples<T>& ls,
                    py::object,
                    py::object,
                    py::object) {});
}

template<typename T>
void init_valid_loaned_samples(py::class_<rti::sub::ValidLoanedSamples<T>>& vls)
{
    init_valid_loaned_samples_def(vls);
}

}  // namespace pyrti