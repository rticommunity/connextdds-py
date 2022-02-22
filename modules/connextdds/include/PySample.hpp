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
#include <dds/sub/Sample.hpp>
#include <dds/core/xtypes/DynamicData.hpp>

namespace pyrti {

template<typename T>
void init_dds_typed_sample_base_template(py::class_<dds::sub::Sample<T>>& cls)
{
    cls.def(py::init<
                    const typename dds::sub::Sample<T>::DataType&,
                    const dds::sub::SampleInfo&>(),
            py::arg("data"),
            py::arg("info"),
            "Construct Sample with data and info.")
            .def(py::init<const dds::sub::Sample<T>&>(),
                 py::arg("sample"),
                 "Copy constructor.")
            .def(
                    "__iter__",
                    [](dds::sub::Sample<T>& s) {
                        auto py_s = py::cast(s);
                        auto data = s.info().valid() ? py::cast(s.data(), py::return_value_policy::reference_internal, py_s) : py::none();
                        auto info = py::cast(s.info(), py::return_value_policy::reference_internal, py_s);
                        return py::make_tuple(data, info).attr("__iter__")();
                    })
            .def_property(
                    "data",
                    (const T& (dds::sub::Sample<T>::*) () const)
                            & dds::sub::Sample<T>::data,
                    (void (dds::sub::Sample<T>::*)(const T&))
                            & dds::sub::Sample<T>::data,
                    "The data associated with the sample.")
            .def_property(
                    "info",
                    (const dds::sub::SampleInfo& (dds::sub::Sample<T>::*) ()
                             const)
                            & dds::sub::Sample<T>::info,
                    (void (dds::sub::Sample<T>::*)(const dds::sub::SampleInfo&))
                            & dds::sub::Sample<T>::info,
                    "Get the info associated with the sample.");
}

template<typename T>
void init_dds_typed_sample_template(py::class_<dds::sub::Sample<T>>& cls)
{
    init_dds_typed_sample_base_template<T>(cls);
    cls.def(py::init<>(), "Basic constructor")
            .def(py::init<const rti::sub::LoanedSample<T>&>(),
                 py::arg("loaned_sample"),
                 "Construct a sample with a loaned sample.");
}

template<typename T>
void init_sample(py::class_<dds::sub::Sample<T>>& s)
{
    init_dds_typed_sample_template(s);
}

}  // namespace pyrti
