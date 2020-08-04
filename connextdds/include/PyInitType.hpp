#pragma once

#include "PyAnyDataReader.hpp"
#include "PyAnyDataWriter.hpp"
#include "PyAnyTopic.hpp"
#include "PyConnext.hpp"
#include "PyContentFilter.hpp"
#include "PyContentFilteredTopic.hpp"
#include "PyDataReader.hpp"
#include "PyDataReaderListener.hpp"
#include "PyDataWriter.hpp"
#include "PyDataWriterListener.hpp"
#include "PyLoanedSample.hpp"
#include "PyLoanedSamples.hpp"
#include "PySample.hpp"
#include "PySharedSamples.hpp"
#include "PyTopic.hpp"
#include "PyTopicInstance.hpp"
#include "PyTopicListener.hpp"
#include "PyWriterContentFilter.hpp"
#include "PyWriterContentFilterHelper.hpp"
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#if rti_connext_version_gte(6, 0, 0)
#include "PyValidLoanedSamples.hpp"
#endif

namespace pyrti {

template <typename T, typename... Bases>
DefInitFunc init_type_class(py::object &parent, ClassInitList &l,
                            const std::string &cls_name) {
  py::class_<T, Bases...> cls(parent, cls_name.c_str());
  py::bind_vector<std::vector<T>>(parent, (cls_name + "Seq").c_str());
  py::implicitly_convertible<py::iterable, std::vector<T>>();

  l.push_back([cls] {
    py::class_<PyDataReaderListener<T>, PyDataReaderListenerTrampoline<T>> drl(
        cls, "DataReaderListener");
    py::class_<PyNoOpDataReaderListener<T>, PyDataReaderListener<T>,
               PyNoOpDataReaderListenerTrampoline<T>>
        nodrl(cls, "NoOpDataReaderListener");

    return (
        [drl, nodrl]() mutable { init_datareader_listener<T>(drl, nodrl); });
  });

  l.push_back([cls] {
    py::class_<PyDataWriterListener<T>, PyDataWriterListenerTrampoline<T>> dwl(
        cls, "DataWriterListener");
    py::class_<PyNoOpDataWriterListener<T>, PyDataWriterListener<T>,
               PyNoOpDataWriterListenerTrampoline<T>>
        nodwl(cls, "NoOpDataWriterListener");

    return (
        [dwl, nodwl]() mutable { init_datawriter_listener<T>(dwl, nodwl); });
  });

  l.push_back([cls] {
    py::class_<PyTopicListener<T>, PyTopicListenerTrampoline<T>> tl(
        cls, "TopicListener");
    py::class_<PyNoOpTopicListener<T>, PyTopicListener<T>,
               PyNoOpTopicListenerTrampoline<T>>
        notl(cls, "NoOpTopicListener");

    return ([tl, notl]() mutable { init_topic_listener<T>(tl, notl); });
  });

  l.push_back([cls] {
    py::class_<PyITopicDescription<T>, PyIEntity> itd(cls, "ITopicDescription");
    py::class_<PyTopicDescription<T>, PyITopicDescription<T>> td(
        cls, "TopicDescription");
    py::class_<PyTopic<T>, PyITopicDescription<T>, PyIAnyTopic> t(cls, "Topic");
    py::bind_vector<std::vector<PyTopic<T>>>(cls, "TopicSeq");
    py::implicitly_convertible<py::iterable, std::vector<PyTopic<T>>>();

    return ([itd, td, t]() mutable { init_topic<T>(itd, td, t); });
  });

  l.push_back([cls] {
    py::class_<dds::topic::TopicInstance<T>> ti(cls, "TopicInstance");
    py::bind_vector<std::vector<dds::topic::TopicInstance<T>>>(
        cls, "TopicInstanceSeq");
    py::implicitly_convertible<py::iterable,
                               std::vector<dds::topic::TopicInstance<T>>>();

    return ([ti, cls]() mutable {
      py::bind_vector<std::vector<
          std::pair<dds::topic::TopicInstance<T>, dds::core::Time>>>(
          cls, "TopicInstanceTimestampedSeq");
      py::implicitly_convertible<
          py::iterable, std::vector<std::pair<dds::topic::TopicInstance<T>,
                                              dds::core::Time>>>();
      init_topic_instance<T>(ti);
    });
  });

  l.push_back([cls] {
    py::class_<dds::sub::Sample<T>> s(cls, "Sample");

    return ([s]() mutable { init_sample<T>(s); });
  });

  l.push_back([cls] {
    py::class_<dds::sub::SharedSamples<T>> ss(cls, "SharedSamples");

    return ([ss]() mutable { init_shared_samples<T>(ss); });
  });

  l.push_back([cls] {
    py::class_<dds::sub::LoanedSamples<T>> ls(cls, "LoanedSamples");

    return ([ls]() mutable { init_loaned_samples<T>(ls); });
  });

#if rti_connext_version_gte(6, 0, 0)
  l.push_back([cls] {
    py::class_<rti::sub::ValidLoanedSamples<T>> vls(cls, "ValidLoanedSamples");

    return ([vls]() mutable { init_valid_loaned_samples<T>(vls); });
  });
#endif

  l.push_back([cls] {
    py::class_<rti::sub::LoanedSample<T>> ls(cls, "LoanedSample");

    return ([ls]() mutable { init_loaned_sample<T>(ls); });
  });

  l.push_back([cls] {
    py::class_<PyContentFilteredTopic<T>, PyITopicDescription<T>, PyIAnyTopic>
        cft(cls, "ContentFilteredTopic");
    py::bind_vector<std::vector<PyContentFilteredTopic<T>>>(
        cls, "ContentFilteredTopicSeq");
    py::implicitly_convertible<py::iterable,
                               std::vector<PyContentFilteredTopic<T>>>();

    return ([cft]() mutable { init_content_filtered_topic<T>(cft); });
  });

  l.push_back([cls] {
    py::class_<rti::topic::ContentFilter<T, dds::core::optional<py::object>>,
               rti::topic::ContentFilterBase, PyContentFilter<T>>
        cf(cls, "ContentFilter");

    return ([cf]() mutable { init_content_filter<T>(cf); });
  });

  l.push_back([cls] {
    py::class_<
        rti::topic::WriterContentFilter<T, dds::core::optional<py::object>,
                                        dds::core::optional<py::object>>,
        rti::topic::ContentFilter<T, dds::core::optional<py::object>>,
        PyWriterContentFilter<T>>
        wcf(cls, "WriterContentFilter");

    return ([wcf]() mutable { init_writer_content_filter<T>(wcf); });
  });

  l.push_back([cls] {
    py::class_<
        rti::topic::WriterContentFilterHelper<T,
                                              dds::core::optional<py::object>,
                                              dds::core::optional<py::object>>,
        rti::topic::WriterContentFilter<T, dds::core::optional<py::object>,
                                        dds::core::optional<py::object>>,
        PyWriterContentFilterHelper<T>>
        wcfh(cls, "WriterContentFilterHelper");

    return ([wcfh]() mutable { init_writer_content_filter_helper<T>(wcfh); });
  });

  l.push_back([cls] {
    py::class_<PyDataReader<T>, PyIDataReader> dr(cls, "DataReader");
    py::bind_vector<std::vector<PyDataReader<T>>>(cls, "DataReaderSeq");
    py::implicitly_convertible<py::iterable, std::vector<PyDataReader<T>>>();

    return ([dr]() mutable { init_datareader<T>(dr); });
  });

  l.push_back([cls] {
    py::class_<PyDataWriter<T>, PyIEntity, PyIAnyDataWriter> dw(cls,
                                                                "DataWriter");
    py::bind_vector<std::vector<PyDataWriter<T>>>(cls, "DataWriterSeq");
    py::implicitly_convertible<py::iterable, std::vector<PyDataWriter<T>>>();

    return ([dw]() mutable { init_datawriter<T>(dw); });
  });

  return ([cls, cls_name, parent]() mutable {
    py::bind_vector<std::vector<std::pair<T, dds::core::Time>>>(
        parent, (cls_name + "TimestampedSeq").c_str());
    py::implicitly_convertible<py::iterable,
                               std::vector<std::pair<T, dds::core::Time>>>();
    init_class_defs<T>(cls);
  });
}

} // namespace pyrti
