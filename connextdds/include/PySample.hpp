#pragma once

#include "PyConnext.hpp"
#include <dds/core/xtypes/DynamicData.hpp>
#include <dds/sub/Sample.hpp>

namespace pyrti {

template <typename T>
void init_dds_typed_sample_base_template(py::class_<dds::sub::Sample<T>> &cls) {
  cls.def(py::init<const typename dds::sub::Sample<T>::DataType &,
                   const dds::sub::SampleInfo &>(),
          py::arg("data"), py::arg("info"),
          "Construct Sample with data and info.")
      .def(py::init<const dds::sub::Sample<T> &>(), py::arg("sample"),
           "Copy constructor.")
      .def(
          "__getitem__",
          [](dds::sub::Sample<T> &s, int index) {
            switch (index) {
            case 0:
              return py::cast(s.data());
            case 1:
              return py::cast(s.info());
            default:
              throw py::index_error("Invalid Sample item index.");
            }
          },
          py::keep_alive<0, 1>())
      .def_property("data",
                    (const T &(dds::sub::Sample<T>::*)() const) &
                        dds::sub::Sample<T>::data,
                    (void (dds::sub::Sample<T>::*)(const T &)) &
                        dds::sub::Sample<T>::data,
                    "The data associated with the sample.")
      .def_property(
          "info",
          (const dds::sub::SampleInfo &(dds::sub::Sample<T>::*)() const) &
              dds::sub::Sample<T>::info,
          (void (dds::sub::Sample<T>::*)(const dds::sub::SampleInfo &)) &
              dds::sub::Sample<T>::info,
          "Get the info associated with the sample.");
}

template <typename T>
void init_dds_typed_sample_template(py::class_<dds::sub::Sample<T>> &cls) {
  init_dds_typed_sample_base_template<T>(cls);
  cls.def(py::init<>(), "Basic constructor")
      .def(py::init<const rti::sub::LoanedSample<T> &>(),
           py::arg("loaned_sample"),
           "Construct a sample with a loaned sample.");
}

template <typename T> void init_sample(py::class_<dds::sub::Sample<T>> &s) {
  init_dds_typed_sample_template(s);
}

} // namespace pyrti
