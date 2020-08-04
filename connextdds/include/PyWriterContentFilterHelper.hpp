#pragma once

#include "PyConnext.hpp"
#include "PyContentFilter.hpp"

namespace pyrti {

template <typename T, class WCFHBase = rti::topic::WriterContentFilterHelper<
                          T, dds::core::optional<py::object>,
                          dds::core::optional<py::object>>>
class PyWriterContentFilterHelper : public PyWriterContentFilter<T, WCFHBase> {
public:
  using PyWriterContentFilter<T, WCFHBase>::PyWriterContentFilter;

  void writer_evaluate_helper(
      dds::core::optional<py::object> &writer_filter_data, const T &sample,
      const rti::topic::FilterSampleInfo &meta_data) override {
    PYBIND11_OVERLOAD_PURE(void, WCFHBase, writer_evaluate, writer_filter_data,
                           sample, meta_data);
  }
};

template <typename T, class WCFHBase = rti::topic::WriterContentFilterHelper<
                          T, dds::core::optional<py::object>,
                          dds::core::optional<py::object>>>
class PyWriterContentFilterHelperHelper : public WCFHBase {
public:
  using WCFHBase::add_cookie;
};

template <typename T>
void init_writer_content_filter_helper_defs(
    py::class_<
        rti::topic::WriterContentFilterHelper<T,
                                              dds::core::optional<py::object>,
                                              dds::core::optional<py::object>>,
        rti::topic::WriterContentFilter<T, dds::core::optional<py::object>,
                                        dds::core::optional<py::object>>,
        PyWriterContentFilterHelper<T>> &cls) {
  cls.def(py::init<>())
      .def("writer_evaluate_helper",
           &rti::topic::WriterContentFilterHelper<
               T, dds::core::optional<py::object>,
               dds::core::optional<py::object>>::writer_evaluate_helper,
           py::arg("writer_filter_data"), py::arg("sample"),
           py::arg("meta_data"),
           "A writer-side filtering API to compile an instance of the "
           "content filter according to the filter expression and "
           "parameters specified by a matching DataReader.")
      .def("add_cookie",
           static_cast<void (rti::topic::WriterContentFilter<
                             T, dds::core::optional<py::object>,
                             dds::core::optional<py::object>>::*)(
               rti::core::Cookie &)>(
               &PyWriterContentFilterHelperHelper<T>::add_cookie),
           py::arg("cookie"),
           "A helper function which will add a Cookie to the Cookie "
           "sequence that is then returned by the writer_evaluate "
           "function.");
}

template <typename T>
void init_writer_content_filter_helper(
    py::class_<
        rti::topic::WriterContentFilterHelper<T,
                                              dds::core::optional<py::object>,
                                              dds::core::optional<py::object>>,
        rti::topic::WriterContentFilter<T, dds::core::optional<py::object>,
                                        dds::core::optional<py::object>>,
        PyWriterContentFilterHelper<T>> &wcfh) {
  init_writer_content_filter_helper_defs(wcfh);
}

} // namespace pyrti
