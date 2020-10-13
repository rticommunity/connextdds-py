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
#include <rti/sub/LoanedSample.hpp>

namespace pyrti {

template<typename T>
void init_loaned_sample_defs(py::class_<rti::sub::LoanedSample<T>>& cls)
{
    cls.def(py::init<>(), "Basic constructor")
#if rti_connext_version_gte(6, 0, 0)
            .def(py::init<
                         const typename rti::sub::LoanedSample<T>::DataType&,
                         const typename rti::sub::LoanedSample<T>::InfoType&>(),
                 py::arg("data"),
                 py::arg("info"),
                 "Construct LoanedSample with data and info.")
#endif
            .def_property_readonly(
                    "data",
                    &rti::sub::LoanedSample<T>::data,
                    "Get the data associated with the sample.")
            .def_property_readonly(
                    "info",
                    &rti::sub::LoanedSample<T>::info,
                    "Get the info associated with the sample.")
            .def(
                    "__getitem__",
                    [](rti::sub::LoanedSample<T>& ls, int index) {
                        switch (index) {
                        case 0:
                            return py::cast(ls.data());
                        case 1:
                            return py::cast(ls.info());
                        default:
                            throw py::index_error(
                                    "Invalid LoanedSample item index.");
                        }
                    },
                    py::keep_alive<0, 1>())
            .def(py::self == py::self, "Check if two samples are equivalent")
            .def(py::self != py::self,
                 "Check if two samples are not equivalent.");
}

template<typename T>
void init_loaned_sample(py::class_<rti::sub::LoanedSample<T>>& ls)
{
    init_loaned_sample_defs(ls);
}

}  // namespace pyrti
