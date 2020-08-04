#pragma once

#include "PyAnyDataReaderListener.hpp"
#include "PyConnext.hpp"
#include "PyEntity.hpp"
#include <dds/sub/SubscriberListener.hpp>

namespace pyrti {

class PySubscriberListener : public dds::sub::SubscriberListener,
                             public PyAnyDataReaderListener {
public:
  using dds::sub::SubscriberListener::SubscriberListener;
  using PyAnyDataReaderListener::PyAnyDataReaderListener;

  using dds::sub::SubscriberListener::on_data_on_readers;

  virtual void on_data_on_readers(PySubscriber &sub) = 0;

  virtual ~PySubscriberListener() {}
};

class PyNoOpSubscriberListener : public PySubscriberListener,
                                 public PyNoOpAnyDataReaderListener {
public:
  using PySubscriberListener::PySubscriberListener;

  using PyNoOpAnyDataReaderListener::on_data_available;
  using PyNoOpAnyDataReaderListener::on_liveliness_changed;
  using PyNoOpAnyDataReaderListener::on_requested_deadline_missed;
  using PyNoOpAnyDataReaderListener::on_requested_incompatible_qos;
  using PyNoOpAnyDataReaderListener::on_sample_lost;
  using PyNoOpAnyDataReaderListener::on_sample_rejected;
  using PyNoOpAnyDataReaderListener::on_subscription_matched;

  using PySubscriberListener::on_data_on_readers;

  void on_data_on_readers(PySubscriber &sub) override {}

  virtual ~PyNoOpSubscriberListener() {}
};

template <class SLBase = PySubscriberListener>
class PySubscriberListenerTrampoline
    : public PyAnyDataReaderListenerTrampoline<SLBase> {
public:
  using PyAnyDataReaderListenerTrampoline<
      SLBase>::PyAnyDataReaderListenerTrampoline;

  void on_data_on_readers(dds::sub::Subscriber &sub) override {
    PySubscriber s(sub);
    this->on_data_on_readers(s);
  }

  void on_data_on_readers(PySubscriber &sub) override {
    PYBIND11_OVERLOAD_PURE(void, SLBase, on_data_on_readers, sub);
  }

  virtual ~PySubscriberListenerTrampoline() {}
};

template <class SLBase = PyNoOpSubscriberListener>
class PyNoOpSubscriberListenerTrampoline
    : public PyNoOpAnyDataReaderListenerTrampoline<SLBase> {
public:
  using PyNoOpAnyDataReaderListenerTrampoline<
      SLBase>::PyNoOpAnyDataReaderListenerTrampoline;

  void on_data_on_readers(dds::sub::Subscriber &sub) override {
    PySubscriber s(sub);
    this->on_data_on_readers(s);
  }

  void on_data_on_readers(PySubscriber &sub) override {
    PYBIND11_OVERLOAD(void, SLBase, on_data_on_readers, sub);
  }

  virtual ~PyNoOpSubscriberListenerTrampoline() {}
};

} // namespace pyrti
