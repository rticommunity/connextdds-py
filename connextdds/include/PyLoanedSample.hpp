#pragma once

#include "PyConnext.hpp"
#include <rti/sub/LoanedSample.hpp>

namespace pyrti {

template<typename T>
void init_loaned_sample_defs(py::class_<rti::sub::LoanedSample<T>>& cls) {
    cls
        .def(
            py::init<>(),
            "Basic constructor"
        )
#if rti_connext_version_gte(6, 0, 0)
        .def(
            py::init<
                const typename rti::sub::LoanedSample<T>::DataType&,
                const typename rti::sub::LoanedSample<T>::InfoType&>(),
            py::arg("data"),
            py::arg("info"),
            "Construct LoanedSample with data and info."
        )
#endif
        .def_property_readonly(
            "data",
            &rti::sub::LoanedSample<T>::data,
            "Get the data associated with the sample."
        )
        .def_property_readonly(
            "info",
            &rti::sub::LoanedSample<T>::info,
            "Get the info associated with the sample."
        )
        .def(
            py::self == py::self,
            "Check if two samples are equivalent"
        )
        .def(
            py::self != py::self,
            "Check if two samples are not equivalent."
        );
}

template<typename T>
void init_loaned_sample(py::object& o) {
    py::class_<rti::sub::LoanedSample<T>> ls(o, "LoanedSample");

    init_loaned_sample_defs(ls);
}

}
