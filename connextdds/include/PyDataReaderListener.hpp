#pragma once

#include "PyConnext.hpp"
#include "PyDataReader.hpp"
#include <dds/sub/DataReaderListener.hpp>

namespace pyrti {

template <typename T> class PyDataReader;

template <typename T>
class PyDataReaderListener : public dds::sub::DataReaderListener<T> {
public:
  using dds::sub::DataReaderListener<T>::DataReaderListener;

  using dds::sub::DataReaderListener<T>::on_requested_deadline_missed;
  using dds::sub::DataReaderListener<T>::on_requested_incompatible_qos;
  using dds::sub::DataReaderListener<T>::on_sample_rejected;
  using dds::sub::DataReaderListener<T>::on_liveliness_changed;
  using dds::sub::DataReaderListener<T>::on_data_available;
  using dds::sub::DataReaderListener<T>::on_subscription_matched;
  using dds::sub::DataReaderListener<T>::on_sample_lost;

  virtual void on_requested_deadline_missed(
      PyDataReader<T> &reader,
      const dds::core::status::RequestedDeadlineMissedStatus &status) = 0;

  virtual void on_requested_incompatible_qos(
      PyDataReader<T> &reader,
      const dds::core::status::RequestedIncompatibleQosStatus &status) = 0;

  virtual void
  on_sample_rejected(PyDataReader<T> &reader,
                     const dds::core::status::SampleRejectedStatus &status) = 0;

  virtual void on_liveliness_changed(
      PyDataReader<T> &reader,
      const dds::core::status::LivelinessChangedStatus &status) = 0;

  virtual void on_data_available(PyDataReader<T> &reader) = 0;

  virtual void on_subscription_matched(
      PyDataReader<T> &reader,
      const dds::core::status::SubscriptionMatchedStatus &status) = 0;

  virtual void
  on_sample_lost(PyDataReader<T> &reader,
                 const dds::core::status::SampleLostStatus &status) = 0;
};

template <typename T>
class PyNoOpDataReaderListener : public PyDataReaderListener<T> {
public:
  using PyDataReaderListener<T>::PyDataReaderListener;

  using PyDataReaderListener<T>::on_requested_deadline_missed;
  using PyDataReaderListener<T>::on_requested_incompatible_qos;
  using PyDataReaderListener<T>::on_sample_rejected;
  using PyDataReaderListener<T>::on_liveliness_changed;
  using PyDataReaderListener<T>::on_data_available;
  using PyDataReaderListener<T>::on_subscription_matched;
  using PyDataReaderListener<T>::on_sample_lost;

  void on_requested_deadline_missed(
      PyDataReader<T> &reader,
      const dds::core::status::RequestedDeadlineMissedStatus &status) override {
  }

  void on_requested_incompatible_qos(
      PyDataReader<T> &reader,
      const dds::core::status::RequestedIncompatibleQosStatus &status)
      override {}

  void on_sample_rejected(
      PyDataReader<T> &reader,
      const dds::core::status::SampleRejectedStatus &status) override {}

  void on_liveliness_changed(
      PyDataReader<T> &reader,
      const dds::core::status::LivelinessChangedStatus &status) override {}

  void on_data_available(PyDataReader<T> &reader) override {}

  void on_subscription_matched(
      PyDataReader<T> &reader,
      const dds::core::status::SubscriptionMatchedStatus &status) override {}

  void
  on_sample_lost(PyDataReader<T> &reader,
                 const dds::core::status::SampleLostStatus &status) override {}
};

template <typename T, class DRLBase = PyDataReaderListener<T>>
class PyDataReaderListenerTrampoline : public DRLBase {
public:
  using DRLBase::DRLBase;

  void on_requested_deadline_missed(
      dds::sub::DataReader<T> &reader,
      const dds::core::status::RequestedDeadlineMissedStatus &status) override {
    PyDataReader<T> dr(reader);
    this->on_requested_deadline_missed(dr, status);
  }

  void on_requested_incompatible_qos(
      dds::sub::DataReader<T> &reader,
      const dds::core::status::RequestedIncompatibleQosStatus &status)
      override {
    PyDataReader<T> dr(reader);
    this->on_requested_incompatible_qos(dr, status);
  }

  void on_sample_rejected(
      dds::sub::DataReader<T> &reader,
      const dds::core::status::SampleRejectedStatus &status) override {
    PyDataReader<T> dr(reader);
    this->on_sample_rejected(dr, status);
  }

  void on_liveliness_changed(
      dds::sub::DataReader<T> &reader,
      const dds::core::status::LivelinessChangedStatus &status) override {
    PyDataReader<T> dr(reader);
    this->on_liveliness_changed(dr, status);
  }

  void on_data_available(dds::sub::DataReader<T> &reader) override {
    PyDataReader<T> dr(reader);
    this->on_data_available(dr);
  }

  void on_subscription_matched(
      dds::sub::DataReader<T> &reader,
      const dds::core::status::SubscriptionMatchedStatus &status) override {
    PyDataReader<T> dr(reader);
    this->on_subscription_matched(dr, status);
  }

  void
  on_sample_lost(dds::sub::DataReader<T> &reader,
                 const dds::core::status::SampleLostStatus &status) override {
    PyDataReader<T> dr(reader);
    this->on_sample_lost(dr, status);
  }

  void on_requested_deadline_missed(
      PyDataReader<T> &reader,
      const dds::core::status::RequestedDeadlineMissedStatus &status) override {
    PYBIND11_OVERLOAD_PURE(void, DRLBase, on_requested_deadline_missed, reader,
                           status);
  }

  void on_requested_incompatible_qos(
      PyDataReader<T> &reader,
      const dds::core::status::RequestedIncompatibleQosStatus &status)
      override {
    PYBIND11_OVERLOAD_PURE(void, DRLBase, on_requested_incompatible_qos, reader,
                           status);
  }

  void on_sample_rejected(
      PyDataReader<T> &reader,
      const dds::core::status::SampleRejectedStatus &status) override {
    PYBIND11_OVERLOAD_PURE(void, DRLBase, on_sample_rejected, reader, status);
  }

  void on_liveliness_changed(
      PyDataReader<T> &reader,
      const dds::core::status::LivelinessChangedStatus &status) override {
    PYBIND11_OVERLOAD_PURE(void, DRLBase, on_liveliness_changed, reader,
                           status);
  }

  void on_data_available(PyDataReader<T> &reader) override {
    PYBIND11_OVERLOAD_PURE(void, DRLBase, on_data_available, reader);
  }

  void on_subscription_matched(
      PyDataReader<T> &reader,
      const dds::core::status::SubscriptionMatchedStatus &status) override {
    PYBIND11_OVERLOAD_PURE(void, DRLBase, on_subscription_matched, reader,
                           status);
  }

  void
  on_sample_lost(PyDataReader<T> &reader,
                 const dds::core::status::SampleLostStatus &status) override {
    PYBIND11_OVERLOAD_PURE(void, DRLBase, on_sample_lost, reader, status);
  }
};

template <typename T, class DRLBase = PyNoOpDataReaderListener<T>>
class PyNoOpDataReaderListenerTrampoline
    : public PyDataReaderListenerTrampoline<T, DRLBase> {
public:
  using PyDataReaderListenerTrampoline<T,
                                       DRLBase>::PyDataReaderListenerTrampoline;

  using PyDataReaderListenerTrampoline<T,
                                       DRLBase>::on_requested_deadline_missed;
  using PyDataReaderListenerTrampoline<T,
                                       DRLBase>::on_requested_incompatible_qos;
  using PyDataReaderListenerTrampoline<T, DRLBase>::on_sample_rejected;
  using PyDataReaderListenerTrampoline<T, DRLBase>::on_liveliness_changed;
  using PyDataReaderListenerTrampoline<T, DRLBase>::on_data_available;
  using PyDataReaderListenerTrampoline<T, DRLBase>::on_subscription_matched;
  using PyDataReaderListenerTrampoline<T, DRLBase>::on_sample_lost;

  void on_requested_deadline_missed(
      PyDataReader<T> &reader,
      const dds::core::status::RequestedDeadlineMissedStatus &status) override {
    PYBIND11_OVERLOAD(void, DRLBase, on_requested_deadline_missed, reader,
                      status);
  }

  void on_requested_incompatible_qos(
      PyDataReader<T> &reader,
      const dds::core::status::RequestedIncompatibleQosStatus &status)
      override {
    PYBIND11_OVERLOAD(void, DRLBase, on_requested_incompatible_qos, reader,
                      status);
  }

  void on_sample_rejected(
      PyDataReader<T> &reader,
      const dds::core::status::SampleRejectedStatus &status) override {
    PYBIND11_OVERLOAD(void, DRLBase, on_sample_rejected, reader, status);
  }

  void on_liveliness_changed(
      PyDataReader<T> &reader,
      const dds::core::status::LivelinessChangedStatus &status) override {
    PYBIND11_OVERLOAD(void, DRLBase, on_liveliness_changed, reader, status);
  }

  void on_data_available(PyDataReader<T> &reader) override {
    PYBIND11_OVERLOAD(void, DRLBase, on_data_available, reader);
  }

  void on_subscription_matched(
      PyDataReader<T> &reader,
      const dds::core::status::SubscriptionMatchedStatus &status) override {
    PYBIND11_OVERLOAD(void, DRLBase, on_subscription_matched, reader, status);
  }

  void
  on_sample_lost(PyDataReader<T> &reader,
                 const dds::core::status::SampleLostStatus &status) override {
    PYBIND11_OVERLOAD(void, DRLBase, on_sample_lost, reader, status);
  }
};

template <typename T>
void init_class_defs(py::class_<PyDataReaderListener<T>,
                                PyDataReaderListenerTrampoline<T>> &cls) {
  cls.def(py::init<>())
      .def("on_requested_deadline_missed",
           (void (PyDataReaderListener<T>::*)(
               PyDataReader<T> &,
               const dds::core::status::RequestedDeadlineMissedStatus &)) &
               PyDataReaderListener<T>::on_requested_deadline_missed,
           "Requested deadline missed callback.")
      .def("on_requested_incompatible_qos",
           (void (PyDataReaderListener<T>::*)(
               PyDataReader<T> &,
               const dds::core::status::RequestedIncompatibleQosStatus &)) &
               PyDataReaderListener<T>::on_requested_incompatible_qos,
           "Requested incompatible QoS callback.")
      .def("on_sample_rejected",
           (void (PyDataReaderListener<T>::*)(
               PyDataReader<T> &,
               const dds::core::status::SampleRejectedStatus &)) &
               PyDataReaderListener<T>::on_sample_rejected,
           "Sample rejected callback.")
      .def("on_liveliness_changed",
           (void (PyDataReaderListener<T>::*)(
               PyDataReader<T> &,
               const dds::core::status::LivelinessChangedStatus &)) &
               PyDataReaderListener<T>::on_liveliness_changed,
           "Liveliness changed callback.")
      .def("on_data_available",
           (void (PyDataReaderListener<T>::*)(PyDataReader<T> &)) &
               PyDataReaderListener<T>::on_data_available,
           "Data available callback.")
      .def("on_subscription_matched",
           (void (PyDataReaderListener<T>::*)(
               PyDataReader<T> &,
               const dds::core::status::SubscriptionMatchedStatus &)) &
               PyDataReaderListener<T>::on_subscription_matched,
           "Subscription matched callback.")
      .def(
          "on_sample_lost",
          (void (PyDataReaderListener<T>::*)(
              PyDataReader<T> &, const dds::core::status::SampleLostStatus &)) &
              PyDataReaderListener<T>::on_sample_lost,
          "Sample lost callback.");
}

template <typename T>
void init_class_defs(
    py::class_<PyNoOpDataReaderListener<T>, PyDataReaderListener<T>,
               PyNoOpDataReaderListenerTrampoline<T>> &cls) {
  cls.def(py::init<>())
      .def("on_requested_deadline_missed",
           (void (PyNoOpDataReaderListener<T>::*)(
               PyDataReader<T> &,
               const dds::core::status::RequestedDeadlineMissedStatus &)) &
               PyNoOpDataReaderListener<T>::on_requested_deadline_missed,
           "Requested deadline missed callback.")
      .def("on_requested_incompatible_qos",
           (void (PyNoOpDataReaderListener<T>::*)(
               PyDataReader<T> &,
               const dds::core::status::RequestedIncompatibleQosStatus &)) &
               PyNoOpDataReaderListener<T>::on_requested_incompatible_qos,
           "Requested incompatible QoS callback.")
      .def("on_sample_rejected",
           (void (PyNoOpDataReaderListener<T>::*)(
               PyDataReader<T> &,
               const dds::core::status::SampleRejectedStatus &)) &
               PyNoOpDataReaderListener<T>::on_sample_rejected,
           "Sample rejected callback.")
      .def("on_liveliness_changed",
           (void (PyNoOpDataReaderListener<T>::*)(
               PyDataReader<T> &,
               const dds::core::status::LivelinessChangedStatus &)) &
               PyNoOpDataReaderListener<T>::on_liveliness_changed,
           "Liveliness changed callback.")
      .def("on_data_available",
           (void (PyNoOpDataReaderListener<T>::*)(PyDataReader<T> &)) &
               PyNoOpDataReaderListener<T>::on_data_available,
           "Data available callback.")
      .def("on_subscription_matched",
           (void (PyNoOpDataReaderListener<T>::*)(
               PyDataReader<T> &,
               const dds::core::status::SubscriptionMatchedStatus &)) &
               PyNoOpDataReaderListener<T>::on_subscription_matched,
           "Subscription matched callback.")
      .def(
          "on_sample_lost",
          (void (PyNoOpDataReaderListener<T>::*)(
              PyDataReader<T> &, const dds::core::status::SampleLostStatus &)) &
              PyNoOpDataReaderListener<T>::on_sample_lost,
          "Sample lost callback.");
}

template <typename T>
void init_datareader_listener(
    py::class_<PyDataReaderListener<T>, PyDataReaderListenerTrampoline<T>> &drl,
    py::class_<PyNoOpDataReaderListener<T>, PyDataReaderListener<T>,
               PyNoOpDataReaderListenerTrampoline<T>> &nodrl) {
  init_class_defs(drl);
  init_class_defs(nodrl);
}

} // namespace pyrti
