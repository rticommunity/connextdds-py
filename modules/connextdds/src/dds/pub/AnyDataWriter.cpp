/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */

#include "PyConnext.hpp"
#include "PySeq.hpp"
#include "PyAnyDataWriter.hpp"

using namespace dds::pub;
using namespace dds::pub::qos;

namespace pyrti {

template<>
void init_class_defs(py::class_<PyAnyDataWriter, PyIAnyDataWriter>& cls)
{
    cls.def(py::init<const PyIAnyDataWriter&>(),
            py::arg("writer"),
            py::call_guard<py::gil_scoped_release>(),
            "Create an AnyDataWriter instance from a object that conforms to "
            "the IAnyDataWriter interface.");

    py::implicitly_convertible<PyIAnyDataWriter, PyAnyDataWriter>();
}

template<>
void init_class_defs(py::class_<PyIAnyDataWriter>& cls)
{
    cls.def_property(
               "qos",
               (dds::pub::qos::DataWriterQos(PyIAnyDataWriter::*)() const)
                       & PyIAnyDataWriter::py_qos,
               (void (PyIAnyDataWriter::*)(const dds::pub::qos::DataWriterQos&))
                       & PyIAnyDataWriter::py_qos,
               py::call_guard<py::gil_scoped_release>(),
               "The QoS for this AnyDataWriter."
               "\n\n"
               "This property's getter returns a deep copy.")
            .def_property_readonly(
                    "topic_name",
                    &PyIAnyDataWriter::py_topic_name,
                    py::call_guard<py::gil_scoped_release>(),
                    "The Topic name for this AnyDataWriter.")
            .def_property_readonly(
                    "type_name",
                    &PyIAnyDataWriter::py_type_name,
                    py::call_guard<py::gil_scoped_release>(),
                    "The type name for this AnyDataWriter.")
            .def_property_readonly(
                    "publisher",
                    &PyIAnyDataWriter::py_publisher,
                    py::call_guard<py::gil_scoped_release>(),
                    "The Publisher for this AnyDataWriter.")
            .def("wait_for_acknowledgments",
                 &PyIAnyDataWriter::py_wait_for_acknowledgments,
                 py::arg("timeout"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Wait for acknowledgments from subscribers.")
            .def("close", &PyIAnyDataWriter::py_close, "Close this DataWriter.")
            .def("retain",
                 &PyIAnyDataWriter::py_retain,
                 "Retain this DataWriter.")
            .def(
                    "__eq__",
                    [](PyIAnyDataWriter& adw, PyIAnyDataWriter& other) {
                        return adw.get_any_datawriter()
                                == other.get_any_datawriter();
                    },
                    py::is_operator(),
                    py::call_guard<py::gil_scoped_release>(),
                    "Test for equality.")
            .def(
                    "__ne__",
                    [](PyIAnyDataWriter& adw, PyIAnyDataWriter& other) {
                        return adw.get_any_datawriter()
                                != other.get_any_datawriter();
                    },
                    py::is_operator(),
                    py::call_guard<py::gil_scoped_release>(),
                    "Test for inequality.");
}

template<>
void process_inits<AnyDataWriter>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class_with_ptr_seq<PyIAnyDataWriter>(m, "IAnyDataWriter");
    });
    l.push_back([m]() mutable {
        return init_class_with_seq<PyAnyDataWriter, PyIAnyDataWriter>(
                m,
                "AnyDataWriter");
    });
}

}  // namespace pyrti
