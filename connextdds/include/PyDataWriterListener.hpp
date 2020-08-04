#pragma once

#include "PyConnext.hpp"
#include "PyDataWriter.hpp"
#include <dds/pub/DataWriterListener.hpp>

namespace pyrti {

template <typename T> class PyDataWriter;

template <typename T>
class PyDataWriterListener : public dds::pub::DataWriterListener<T> {
public:
  using dds::pub::DataWriterListener<T>::DataWriterListener;

  using dds::pub::DataWriterListener<T>::on_offered_deadline_missed;
  using dds::pub::DataWriterListener<T>::on_offered_incompatible_qos;
  using dds::pub::DataWriterListener<T>::on_liveliness_lost;
  using dds::pub::DataWriterListener<T>::on_publication_matched;
  using dds::pub::DataWriterListener<T>::on_reliable_writer_cache_changed;
  using dds::pub::DataWriterListener<T>::on_reliable_reader_activity_changed;
  using dds::pub::DataWriterListener<T>::on_instance_replaced;
  using dds::pub::DataWriterListener<T>::on_application_acknowledgment;
  using dds::pub::DataWriterListener<T>::on_service_request_accepted;

  virtual void on_offered_deadline_missed(
      PyDataWriter<T> &writer,
      const dds::core::status::OfferedDeadlineMissedStatus &status) = 0;

  virtual void on_offered_incompatible_qos(
      PyDataWriter<T> &writer,
      const dds::core::status::OfferedIncompatibleQosStatus &status) = 0;

  virtual void
  on_liveliness_lost(PyDataWriter<T> &writer,
                     const dds::core::status::LivelinessLostStatus &status) = 0;

  virtual void on_publication_matched(
      PyDataWriter<T> &writer,
      const dds::core::status::PublicationMatchedStatus &status) = 0;

  virtual void on_reliable_writer_cache_changed(
      PyDataWriter<T> &writer,
      const rti::core::status::ReliableWriterCacheChangedStatus &status) = 0;

  virtual void on_reliable_reader_activity_changed(
      PyDataWriter<T> &writer,
      const rti::core::status::ReliableReaderActivityChangedStatus &status) = 0;

  virtual void
  on_instance_replaced(PyDataWriter<T> &writer,
                       const dds::core::InstanceHandle &handle) = 0;

  virtual void on_application_acknowledgment(
      PyDataWriter<T> &writer,
      const rti::pub::AcknowledgmentInfo &acknowledgment_info) = 0;

  virtual void on_service_request_accepted(
      PyDataWriter<T> &writer,
      const rti::core::status::ServiceRequestAcceptedStatus &status) = 0;

#if rti_connext_version_gte(6, 0, 0)
  void on_destination_unreachable(dds::pub::DataWriter<T> &writer,
                                  const dds::core::InstanceHandle &handle,
                                  const rti::core::Locator &locator) override {}

  void *on_data_request(dds::pub::DataWriter<T> &writer,
                        const rti::core::Cookie &cookie) override {
    return nullptr;
  }

  void on_data_return(dds::pub::DataWriter<T> &writer, void *data,
                      const rti::core::Cookie &cookie) override {}

  void on_sample_removed(dds::pub::DataWriter<T> &writer,
                         const rti::core::Cookie &cookie) override {}
#endif
};

template <typename T>
class PyNoOpDataWriterListener : public PyDataWriterListener<T> {
public:
  using PyDataWriterListener<T>::PyDataWriterListener;

  using PyDataWriterListener<T>::on_offered_deadline_missed;
  using PyDataWriterListener<T>::on_offered_incompatible_qos;
  using PyDataWriterListener<T>::on_liveliness_lost;
  using PyDataWriterListener<T>::on_publication_matched;
  using PyDataWriterListener<T>::on_reliable_writer_cache_changed;
  using PyDataWriterListener<T>::on_reliable_reader_activity_changed;
  using PyDataWriterListener<T>::on_instance_replaced;
  using PyDataWriterListener<T>::on_application_acknowledgment;
  using PyDataWriterListener<T>::on_service_request_accepted;

  void on_offered_deadline_missed(
      PyDataWriter<T> &writer,
      const dds::core::status::OfferedDeadlineMissedStatus &status) override {}

  void on_offered_incompatible_qos(
      PyDataWriter<T> &writer,
      const dds::core::status::OfferedIncompatibleQosStatus &status) override {}

  void on_liveliness_lost(
      PyDataWriter<T> &writer,
      const dds::core::status::LivelinessLostStatus &status) override {}

  void on_publication_matched(
      PyDataWriter<T> &writer,
      const dds::core::status::PublicationMatchedStatus &status) override {}

  void on_reliable_writer_cache_changed(
      PyDataWriter<T> &writer,
      const rti::core::status::ReliableWriterCacheChangedStatus &status)
      override {}

  void on_reliable_reader_activity_changed(
      PyDataWriter<T> &writer,
      const rti::core::status::ReliableReaderActivityChangedStatus &status)
      override {}

  void on_instance_replaced(PyDataWriter<T> &writer,
                            const dds::core::InstanceHandle &handle) override {}

  void on_application_acknowledgment(
      PyDataWriter<T> &writer,
      const rti::pub::AcknowledgmentInfo &acknowledgment_info) override {}

  void on_service_request_accepted(
      PyDataWriter<T> &writer,
      const rti::core::status::ServiceRequestAcceptedStatus &status) override {}
};

template <typename T, class DWLBase = PyDataWriterListener<T>>
class PyDataWriterListenerTrampoline : public DWLBase {
public:
  using DWLBase::DWLBase;

  void on_offered_deadline_missed(
      dds::pub::DataWriter<T> &writer,
      const dds::core::status::OfferedDeadlineMissedStatus &status) override {
    PyDataWriter<T> dw(writer);
    this->on_offered_deadline_missed(dw, status);
  }

  void on_offered_incompatible_qos(
      dds::pub::DataWriter<T> &writer,
      const dds::core::status::OfferedIncompatibleQosStatus &status) override {
    PyDataWriter<T> dw(writer);
    this->on_offered_incompatible_qos(dw, status);
  }

  void on_liveliness_lost(
      dds::pub::DataWriter<T> &writer,
      const dds::core::status::LivelinessLostStatus &status) override {
    PyDataWriter<T> dw(writer);
    this->on_liveliness_lost(dw, status);
  }

  void on_publication_matched(
      dds::pub::DataWriter<T> &writer,
      const dds::core::status::PublicationMatchedStatus &status) override {
    PyDataWriter<T> dw(writer);
    this->on_publication_matched(dw, status);
  }

  void on_reliable_writer_cache_changed(
      dds::pub::DataWriter<T> &writer,
      const rti::core::status::ReliableWriterCacheChangedStatus &status)
      override {
    PyDataWriter<T> dw(writer);
    this->on_reliable_writer_cache_changed(dw, status);
  }

  void on_reliable_reader_activity_changed(
      dds::pub::DataWriter<T> &writer,
      const rti::core::status::ReliableReaderActivityChangedStatus &status)
      override {
    PyDataWriter<T> dw(writer);
    this->on_reliable_reader_activity_changed(dw, status);
  }

  void on_instance_replaced(dds::pub::DataWriter<T> &writer,
                            const dds::core::InstanceHandle &handle) override {
    PyDataWriter<T> dw(writer);
    this->on_instance_replaced(dw, handle);
  }

  void on_application_acknowledgment(
      dds::pub::DataWriter<T> &writer,
      const rti::pub::AcknowledgmentInfo &acknowledgment_info) override {
    PyDataWriter<T> dw(writer);
    this->on_application_acknowledgment(dw, acknowledgment_info);
  }

  void on_service_request_accepted(
      dds::pub::DataWriter<T> &writer,
      const rti::core::status::ServiceRequestAcceptedStatus &status) override {
    PyDataWriter<T> dw(writer);
    this->on_service_request_accepted(dw, status);
  }

  void on_offered_deadline_missed(
      PyDataWriter<T> &writer,
      const dds::core::status::OfferedDeadlineMissedStatus &status) override {
    PYBIND11_OVERLOAD_PURE(void, DWLBase, on_offered_deadline_missed, writer,
                           status);
  }

  void on_offered_incompatible_qos(
      PyDataWriter<T> &writer,
      const dds::core::status::OfferedIncompatibleQosStatus &status) override {
    PYBIND11_OVERLOAD_PURE(void, DWLBase, on_offered_incompatible_qos, writer,
                           status);
  }

  void on_liveliness_lost(
      PyDataWriter<T> &writer,
      const dds::core::status::LivelinessLostStatus &status) override {
    PYBIND11_OVERLOAD_PURE(void, DWLBase, on_liveliness_lost, writer, status);
  }

  void on_publication_matched(
      PyDataWriter<T> &writer,
      const dds::core::status::PublicationMatchedStatus &status) override {
    PYBIND11_OVERLOAD_PURE(void, DWLBase, on_publication_matched, writer,
                           status);
  }

  void on_reliable_writer_cache_changed(
      PyDataWriter<T> &writer,
      const rti::core::status::ReliableWriterCacheChangedStatus &status)
      override {
    PYBIND11_OVERLOAD_PURE(void, DWLBase, on_reliable_writer_cache_changed,
                           writer, status);
  }

  void on_reliable_reader_activity_changed(
      PyDataWriter<T> &writer,
      const rti::core::status::ReliableReaderActivityChangedStatus &status)
      override {
    PYBIND11_OVERLOAD_PURE(void, DWLBase, on_reliable_reader_activity_changed,
                           writer, status);
  }

  void on_instance_replaced(PyDataWriter<T> &writer,
                            const dds::core::InstanceHandle &handle) override {
    PYBIND11_OVERLOAD_PURE(void, DWLBase, on_instance_replaced, writer, handle);
  }

  void on_application_acknowledgment(
      PyDataWriter<T> &writer,
      const rti::pub::AcknowledgmentInfo &acknowledgment_info) override {
    PYBIND11_OVERLOAD_PURE(void, DWLBase, on_application_acknowledgment, writer,
                           acknowledgment_info);
  }

  void on_service_request_accepted(
      PyDataWriter<T> &writer,
      const rti::core::status::ServiceRequestAcceptedStatus &status) override {
    PYBIND11_OVERLOAD_PURE(void, DWLBase, on_service_request_accepted, writer,
                           status);
  }
};

template <typename T, class DWLBase = PyNoOpDataWriterListener<T>>
class PyNoOpDataWriterListenerTrampoline
    : public PyDataWriterListenerTrampoline<T, DWLBase> {
public:
  using PyDataWriterListenerTrampoline<T,
                                       DWLBase>::PyDataWriterListenerTrampoline;

  using PyDataWriterListenerTrampoline<T, DWLBase>::on_offered_deadline_missed;
  using PyDataWriterListenerTrampoline<T, DWLBase>::on_offered_incompatible_qos;
  using PyDataWriterListenerTrampoline<T, DWLBase>::on_liveliness_lost;
  using PyDataWriterListenerTrampoline<T, DWLBase>::on_publication_matched;
  using PyDataWriterListenerTrampoline<
      T, DWLBase>::on_reliable_writer_cache_changed;
  using PyDataWriterListenerTrampoline<
      T, DWLBase>::on_reliable_reader_activity_changed;
  using PyDataWriterListenerTrampoline<T, DWLBase>::on_instance_replaced;
  using PyDataWriterListenerTrampoline<T,
                                       DWLBase>::on_application_acknowledgment;
  using PyDataWriterListenerTrampoline<T, DWLBase>::on_service_request_accepted;

  void on_offered_deadline_missed(
      PyDataWriter<T> &writer,
      const dds::core::status::OfferedDeadlineMissedStatus &status) override {
    PYBIND11_OVERLOAD(void, DWLBase, on_offered_deadline_missed, writer,
                      status);
  }

  void on_offered_incompatible_qos(
      PyDataWriter<T> &writer,
      const dds::core::status::OfferedIncompatibleQosStatus &status) override {
    PYBIND11_OVERLOAD(void, DWLBase, on_offered_incompatible_qos, writer,
                      status);
  }

  void on_liveliness_lost(
      PyDataWriter<T> &writer,
      const dds::core::status::LivelinessLostStatus &status) override {
    PYBIND11_OVERLOAD(void, DWLBase, on_liveliness_lost, writer, status);
  }

  void on_publication_matched(
      PyDataWriter<T> &writer,
      const dds::core::status::PublicationMatchedStatus &status) override {
    PYBIND11_OVERLOAD(void, DWLBase, on_publication_matched, writer, status);
  }

  void on_reliable_writer_cache_changed(
      PyDataWriter<T> &writer,
      const rti::core::status::ReliableWriterCacheChangedStatus &status)
      override {
    PYBIND11_OVERLOAD(void, DWLBase, on_reliable_writer_cache_changed, writer,
                      status);
  }

  void on_reliable_reader_activity_changed(
      PyDataWriter<T> &writer,
      const rti::core::status::ReliableReaderActivityChangedStatus &status)
      override {
    PYBIND11_OVERLOAD(void, DWLBase, on_reliable_reader_activity_changed,
                      writer, status);
  }

  void on_instance_replaced(PyDataWriter<T> &writer,
                            const dds::core::InstanceHandle &handle) override {
    PYBIND11_OVERLOAD(void, DWLBase, on_instance_replaced, writer, handle);
  }

  void on_application_acknowledgment(
      PyDataWriter<T> &writer,
      const rti::pub::AcknowledgmentInfo &acknowledgment_info) override {
    PYBIND11_OVERLOAD(void, DWLBase, on_application_acknowledgment, writer,
                      acknowledgment_info);
  }

  void on_service_request_accepted(
      PyDataWriter<T> &writer,
      const rti::core::status::ServiceRequestAcceptedStatus &status) override {
    PYBIND11_OVERLOAD(void, DWLBase, on_service_request_accepted, writer,
                      status);
  }
};

template <typename T>
void init_class_defs(py::class_<PyDataWriterListener<T>,
                                PyDataWriterListenerTrampoline<T>> &cls) {
  cls.def(py::init<>())
      .def("on_offered_deadline_missed",
           (void (PyDataWriterListener<T>::*)(
               PyDataWriter<T> &,
               const dds::core::status::OfferedDeadlineMissedStatus &)) &
               PyDataWriterListener<T>::on_offered_deadline_missed,
           "Offered deadline missed callback.")
      .def("on_offered_incompatible_qos",
           (void (PyDataWriterListener<T>::*)(
               PyDataWriter<T> &,
               const dds::core::status::OfferedIncompatibleQosStatus &)) &
               PyDataWriterListener<T>::on_offered_incompatible_qos,
           "Offered incompatible QoS callback.")
      .def("on_liveliness_lost",
           (void (PyDataWriterListener<T>::*)(
               PyDataWriter<T> &,
               const dds::core::status::LivelinessLostStatus &)) &
               PyDataWriterListener<T>::on_liveliness_lost,
           "Liveliness lost callback.")
      .def("on_publication_matched",
           (void (PyDataWriterListener<T>::*)(
               PyDataWriter<T> &,
               const dds::core::status::PublicationMatchedStatus &)) &
               PyDataWriterListener<T>::on_publication_matched,
           "Publication matched callback.")
      .def("on_reliable_writer_cache_changed",
           (void (PyDataWriterListener<T>::*)(
               PyDataWriter<T> &,
               const rti::core::status::ReliableWriterCacheChangedStatus &)) &
               PyDataWriterListener<T>::on_reliable_writer_cache_changed,
           "Reliable writer cache changed callback.")
      .def(
          "on_reliable_reader_activity_changed",
          (void (PyDataWriterListener<T>::*)(
              PyDataWriter<T> &,
              const rti::core::status::ReliableReaderActivityChangedStatus &)) &
              PyDataWriterListener<T>::on_reliable_reader_activity_changed,
          "Reliable reader activity changed callback.")
      .def("on_instance_replaced",
           (void (PyDataWriterListener<T>::*)(
               PyDataWriter<T> &, const dds::core::InstanceHandle &)) &
               PyDataWriterListener<T>::on_instance_replaced,
           "On instance replaced callback.")
      .def("on_application_acknowledgment",
           (void (PyDataWriterListener<T>::*)(
               PyDataWriter<T> &, const rti::pub::AcknowledgmentInfo &)) &
               PyDataWriterListener<T>::on_application_acknowledgment,
           "On application acknowledgment callback")
      .def("on_service_request_accepted",
           (void (PyDataWriterListener<T>::*)(
               PyDataWriter<T> &,
               const rti::core::status::ServiceRequestAcceptedStatus &)) &
               PyDataWriterListener<T>::on_service_request_accepted,
           "On service request accepted callback.");
  /*.def(
      "on_destination_unreachable",
      &PyDataWriterListener<T>::on_destination_unreachable,
      "On destination unreachable callback"
  )
  .def(
      "on_data_request",
      &PyDataWriterListener<T>::on_data_request,
      "On data request callback"
  )
  .def(
      "on_data_return",
      &PyDataWriterListener<T>::on_data_return,
      "On data return callback"
  )
  .def(
      "on_sample_removed",
      &PyDataWriterListener<T>::on_sample_removed,
      "On sample removed callback"
  );*/
}

template <typename T>
void init_class_defs(
    py::class_<PyNoOpDataWriterListener<T>, PyDataWriterListener<T>,
               PyNoOpDataWriterListenerTrampoline<T>> &cls) {
  cls.def(py::init<>())
      .def("on_offered_deadline_missed",
           (void (PyNoOpDataWriterListener<T>::*)(
               PyDataWriter<T> &,
               const dds::core::status::OfferedDeadlineMissedStatus &)) &
               PyNoOpDataWriterListener<T>::on_offered_deadline_missed,
           "Offered deadline missed callback.")
      .def("on_offered_incompatible_qos",
           (void (PyNoOpDataWriterListener<T>::*)(
               PyDataWriter<T> &,
               const dds::core::status::OfferedIncompatibleQosStatus &)) &
               PyNoOpDataWriterListener<T>::on_offered_incompatible_qos,
           "Offered incompatible QoS callback.")
      .def("on_liveliness_lost",
           (void (PyNoOpDataWriterListener<T>::*)(
               PyDataWriter<T> &,
               const dds::core::status::LivelinessLostStatus &)) &
               PyNoOpDataWriterListener<T>::on_liveliness_lost,
           "Liveliness lost callback.")
      .def("on_publication_matched",
           (void (PyNoOpDataWriterListener<T>::*)(
               PyDataWriter<T> &,
               const dds::core::status::PublicationMatchedStatus &)) &
               PyNoOpDataWriterListener<T>::on_publication_matched,
           "Publication matched callback.")
      .def("on_reliable_writer_cache_changed",
           (void (PyNoOpDataWriterListener<T>::*)(
               PyDataWriter<T> &,
               const rti::core::status::ReliableWriterCacheChangedStatus &)) &
               PyNoOpDataWriterListener<T>::on_reliable_writer_cache_changed,
           "Reliable writer cache changed callback.")
      .def(
          "on_reliable_reader_activity_changed",
          (void (PyNoOpDataWriterListener<T>::*)(
              PyDataWriter<T> &,
              const rti::core::status::ReliableReaderActivityChangedStatus &)) &
              PyNoOpDataWriterListener<T>::on_reliable_reader_activity_changed,
          "Reliable reader activity changed callback.")
      .def("on_instance_replaced",
           (void (PyNoOpDataWriterListener<T>::*)(
               PyDataWriter<T> &, const dds::core::InstanceHandle &)) &
               PyNoOpDataWriterListener<T>::on_instance_replaced,
           "On instance replaced callback.")
      .def("on_application_acknowledgment",
           (void (PyNoOpDataWriterListener<T>::*)(
               PyDataWriter<T> &, const rti::pub::AcknowledgmentInfo &)) &
               PyNoOpDataWriterListener<T>::on_application_acknowledgment,
           "On application acknowledgment callback")
      .def("on_service_request_accepted",
           (void (PyNoOpDataWriterListener<T>::*)(
               PyDataWriter<T> &,
               const rti::core::status::ServiceRequestAcceptedStatus &)) &
               PyNoOpDataWriterListener<T>::on_service_request_accepted,
           "On service request accepted callback.");
  /*.def(
      "on_destination_unreachable",
      &PyNoOpDataWriterListener<T>::on_destination_unreachable,
      "On destination unreachable callback"
  )
  .def(
      "on_data_request",
      &PyNoOpDataWriterListener<T>::on_data_request,
      "On data request callback"
  )
  .def(
      "on_data_return",
      &PyNoOpDataWriterListener<T>::on_data_return,
      "On data return callback"
  )
  .def(
      "on_sample_removed",
      &PyNoOpDataWriterListener<T>::on_sample_removed,
      "On sample removed callback"
  ); */
}

template <typename T>
void init_datawriter_listener(
    py::class_<PyDataWriterListener<T>, PyDataWriterListenerTrampoline<T>> &dwl,
    py::class_<PyNoOpDataWriterListener<T>, PyDataWriterListener<T>,
               PyNoOpDataWriterListenerTrampoline<T>> &nodwl) {
  init_class_defs(dwl);
  init_class_defs(nodwl);
}

} // namespace pyrti
