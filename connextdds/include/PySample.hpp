#pragma once

#include "PyConnext.hpp"
#include <dds/sub/Sample.hpp>
#include <dds/core/xtypes/DynamicData.hpp>

namespace pyrti {

template<typename T>
void init_dds_typed_sample_base_template(py::class_<dds::sub::Sample<T>>& cls) {
    cls
        .def(
            py::init<
                const typename dds::sub::Sample<T>::DataType&,
                const dds::sub::SampleInfo&
            >(),
            py::arg("data"),
            py::arg("info"),
            "Construct Sample with data and info."
        )
        .def(
            py::init<
                const dds::sub::Sample<T>&
            >(),
            py::arg("sample"),
            "Copy constructor."
        )
        .def_property(
            "data",
            (const T& (dds::sub::Sample<T>::*)() const) &dds::sub::Sample<T>::data,
            (void (dds::sub::Sample<T>::*)(const T&)) &dds::sub::Sample<T>::data,
            "The data associated with the sample."
        )
        .def_property(
            "info",
            (const dds::sub::SampleInfo& (dds::sub::Sample<T>::*)() const) &dds::sub::Sample<T>::info,
            (void (dds::sub::Sample<T>::*)(const dds::sub::SampleInfo&)) &dds::sub::Sample<T>::info,
            "Get the info associated with the sample."
        );
}

template<typename T>
void init_dds_typed_sample_template(py::class_<dds::sub::Sample<T>>& cls) {
    init_dds_typed_sample_base_template<T>(cls);
    cls
        .def(
            py::init<>(),
            "Basic constructor"
        )
        .def(
            py::init<
                const rti::sub::LoanedSample<T>&
            >(),
            py::arg("loaned_sample"),
            "Construct a sample with a loaned sample."
        );
}

template<typename T>
void init_sample(py::object& o) {
    py::class_<dds::sub::Sample<T>> s(o, "Sample");

    init_dds_typed_sample_template(s);
}

}
