#include "PyConnext.hpp"
#include <dds/topic/AnyTopic.hpp>
#include "PyAnyTopic.hpp"

using namespace dds::topic;
using namespace dds::topic::qos;

template<>
void pyrti::init_class_defs(py::class_<PyAnyTopic>& cls) {
    cls
        .def(
            py::init<
                const pyrti::PyIAnyTopic&
            >(),
            py::arg("topic"),
            "Create an AnyTopic instance from a object that conforms to the "
            "IAnyTopic interface."
        );

    py::implicitly_convertible<pyrti::PyIAnyTopic, pyrti::PyAnyTopic>();
}

template<>
void pyrti::init_class_defs(py::class_<PyIAnyTopic>& cls) {
    cls
        .def_property(
            "qos",
            (TopicQos (PyIAnyTopic::*)() const) &PyIAnyTopic::py_qos,
            (void (PyIAnyTopic::*)(const TopicQos&)) &PyIAnyTopic::py_qos,
            "The QoS for this AnyTopic."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property_readonly(
            "name",
            (std::string (PyIAnyTopic::*)() const) &PyIAnyTopic::py_name,
            "The Topic name for this AnyTopic."
        )
        .def_property_readonly(
            "type_name",
            (std::string (PyIAnyTopic::*)() const) &PyIAnyTopic::py_type_name,
            "The type name for this AnyTopic."
        )
        .def(
            "close",
            &PyIAnyTopic::py_close,
            "Close this Topic."
        )
        .def(
            "__eq__",
            [](pyrti::PyIAnyTopic& at, pyrti::PyIAnyTopic& other) {
                return at.get_any_topic() == other.get_any_topic();
            },
            py::is_operator(),
            "Test for equality."
        )
        .def(
            "__ne__",
            [](pyrti::PyIAnyTopic& at, pyrti::PyIAnyTopic& other) {
                return at.get_any_topic() != other.get_any_topic();
            },
            py::is_operator(),
            "Test for inequality."
        );
}

template<>
void pyrti::process_inits<AnyTopic>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<PyIAnyTopic>(m, "IAnyTopic");
        }
    ); 

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<PyAnyTopic>(m, "AnyTopic");
        }
    ); 
}
