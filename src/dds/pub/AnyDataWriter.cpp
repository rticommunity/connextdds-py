#include "PyConnext.hpp"
#include "PyAnyDataWriter.hpp"

using namespace dds::pub;
using namespace dds::pub::qos;

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyAnyDataWriter, pyrti::PyIAnyDataWriter>& cls) {
    cls
        .def(
            py::init<
                const pyrti::PyIAnyDataWriter&
            >(),
            py::arg("writer"),
            "Create an AnyDataWriter instance from a object that conforms to "
            "the IAnyDataWriter interface."
        );

    py::implicitly_convertible<PyIAnyDataWriter, PyAnyDataWriter>();
}

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyIAnyDataWriter>& cls) {
    cls
        .def_property(
            "qos",
            (dds::pub::qos::DataWriterQos (pyrti::PyIAnyDataWriter::*)() const) &pyrti::PyIAnyDataWriter::py_qos,
            (void (pyrti::PyIAnyDataWriter::*)(const dds::pub::qos::DataWriterQos&)) &pyrti::PyIAnyDataWriter::py_qos,
            "The QoS for this AnyDataWriter."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property_readonly(
            "topic_name",
            &pyrti::PyIAnyDataWriter::py_topic_name,
            "The Topic name for this AnyDataWriter."
        )
        .def_property_readonly(
            "type_name",
            &pyrti::PyIAnyDataWriter::py_type_name,
            "The type name for this AnyDataWriter."
        )
        .def_property_readonly(
            "publisher",
            &pyrti::PyIAnyDataWriter::py_publisher,
            "The Publisher for this AnyDataWriter."
        )
        .def(
            "wait_for_acknowledgments",
            &pyrti::PyIAnyDataWriter::py_wait_for_acknowledgments,
            py::arg("timeout"),
            py::call_guard<py::gil_scoped_release>(),
            "Wait for acknowledgments from subscribers."
        )
        .def(
            "close",
            &pyrti::PyIAnyDataWriter::py_close,
            "Close this DataWriter."
        )
        .def(
            "retain",
            &pyrti::PyIAnyDataWriter::py_retain,
            "Retain this DataWriter."
        )
        .def(
            "__eq__",
            [](pyrti::PyIAnyDataWriter& adw, pyrti::PyIAnyDataWriter& other) {
                return adw.get_any_datawriter() == other.get_any_datawriter();
            },
            py::is_operator(),
            "Test for equality."
        )
        .def(
            "__ne__",
            [](pyrti::PyIAnyDataWriter& adw, pyrti::PyIAnyDataWriter& other) {
                return adw.get_any_datawriter() != other.get_any_datawriter();
            },
            py::is_operator(),
            "Test for inequality."
        );
}

template<>
void pyrti::process_inits<AnyDataWriter>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyIAnyDataWriter>(m, "IAnyDataWriter");
        }
    );
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyAnyDataWriter, pyrti::PyIAnyDataWriter>(m, "AnyDataWriter");
        }
    );
}