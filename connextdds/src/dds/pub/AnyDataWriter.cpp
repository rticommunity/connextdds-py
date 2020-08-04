#include "PyAnyDataWriter.hpp"
#include "PyConnext.hpp"
#include "PySeq.hpp"

using namespace dds::pub;
using namespace dds::pub::qos;

namespace pyrti {

template <>
void init_class_defs(py::class_<PyAnyDataWriter, PyIAnyDataWriter> &cls) {
  cls.def(py::init<const PyIAnyDataWriter &>(), py::arg("writer"),
          "Create an AnyDataWriter instance from a object that conforms to "
          "the IAnyDataWriter interface.");

  py::implicitly_convertible<PyIAnyDataWriter, PyAnyDataWriter>();
}

template <> void init_class_defs(py::class_<PyIAnyDataWriter> &cls) {
  cls.def_property(
         "qos",
         (dds::pub::qos::DataWriterQos(PyIAnyDataWriter::*)() const) &
             PyIAnyDataWriter::py_qos,
         (void (PyIAnyDataWriter::*)(const dds::pub::qos::DataWriterQos &)) &
             PyIAnyDataWriter::py_qos,
         "The QoS for this AnyDataWriter."
         "\n\n"
         "This property's getter returns a deep copy.")
      .def_property_readonly("topic_name", &PyIAnyDataWriter::py_topic_name,
                             "The Topic name for this AnyDataWriter.")
      .def_property_readonly("type_name", &PyIAnyDataWriter::py_type_name,
                             "The type name for this AnyDataWriter.")
      .def_property_readonly("publisher", &PyIAnyDataWriter::py_publisher,
                             "The Publisher for this AnyDataWriter.")
      .def("wait_for_acknowledgments",
           &PyIAnyDataWriter::py_wait_for_acknowledgments, py::arg("timeout"),
           py::call_guard<py::gil_scoped_release>(),
           "Wait for acknowledgments from subscribers.")
      .def("close", &PyIAnyDataWriter::py_close, "Close this DataWriter.")
      .def("retain", &PyIAnyDataWriter::py_retain, "Retain this DataWriter.")
      .def(
          "__eq__",
          [](PyIAnyDataWriter &adw, PyIAnyDataWriter &other) {
            return adw.get_any_datawriter() == other.get_any_datawriter();
          },
          py::is_operator(), "Test for equality.")
      .def(
          "__ne__",
          [](PyIAnyDataWriter &adw, PyIAnyDataWriter &other) {
            return adw.get_any_datawriter() != other.get_any_datawriter();
          },
          py::is_operator(), "Test for inequality.");
}

template <> void process_inits<AnyDataWriter>(py::module &m, ClassInitList &l) {
  l.push_back([m]() mutable {
    return init_class_with_ptr_seq<PyIAnyDataWriter>(m, "IAnyDataWriter");
  });
  l.push_back([m]() mutable {
    return init_class_with_seq<PyAnyDataWriter, PyIAnyDataWriter>(
        m, "AnyDataWriter");
  });
}

} // namespace pyrti
