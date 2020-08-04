#pragma once

#include "PyConnext.hpp"
#include "PyEntity.hpp"
#include <dds/pub/AnyDataWriter.hpp>

namespace pyrti {

class PyIAnyDataWriter {
public:
  virtual dds::pub::AnyDataWriter get_any_datawriter() const = 0;

  virtual dds::pub::qos::DataWriterQos py_qos() const = 0;

  virtual void py_qos(const dds::pub::qos::DataWriterQos &) = 0;

  virtual const std::string &py_topic_name() const = 0;

  virtual const std::string &py_type_name() const = 0;

  virtual const PyPublisher py_publisher() const = 0;

  virtual void py_wait_for_acknowledgments(const dds::core::Duration &) = 0;

  virtual void py_close() = 0;

  virtual void py_retain() = 0;

  virtual ~PyIAnyDataWriter() {}
};

class PyAnyDataWriter : public dds::pub::AnyDataWriter,
                        public PyIAnyDataWriter {
public:
  using dds::pub::AnyDataWriter::AnyDataWriter;

  PyAnyDataWriter(dds::pub::AnyDataWriter &adw)
      : dds::pub::AnyDataWriter(adw) {}

  PyAnyDataWriter(const dds::pub::AnyDataWriter &adw)
      : dds::pub::AnyDataWriter(adw) {}

  PyAnyDataWriter(const PyIAnyDataWriter &adw)
      : dds::pub::AnyDataWriter(adw.get_any_datawriter()) {}

  dds::pub::AnyDataWriter get_any_datawriter() const override {
    return (*this);
  }

  dds::pub::qos::DataWriterQos py_qos() const override { return this->qos(); }

  void py_qos(const dds::pub::qos::DataWriterQos &q) override { this->qos(q); }

  const std::string &py_topic_name() const override {
    return this->topic_name();
  }

  const std::string &py_type_name() const override { return this->type_name(); }

  const PyPublisher py_publisher() const override {
    auto p = this->publisher();
    return PyPublisher(p);
  }

  void
  py_wait_for_acknowledgments(const dds::core::Duration &timeout) override {
    this->wait_for_acknowledgments(timeout);
  }

  void py_close() override { this->close(); }

  void py_retain() override { this->retain(); }

  virtual ~PyAnyDataWriter() {}
};

} // namespace pyrti
