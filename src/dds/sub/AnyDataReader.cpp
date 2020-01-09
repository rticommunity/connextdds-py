#include "PyConnext.hpp"
#include <dds/sub/AnyDataReader.hpp>
#include "PyAnyDataReader.hpp"

using namespace dds::sub;
using namespace dds::sub::qos;

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyAnyDataReader, pyrti::PyIAnyDataReader>& cls) {
    cls
        .def(
            py::init<
                const pyrti::PyIAnyDataReader&
            >(),
            py::arg("reader"),
            "Create an AnyDataReader instance from a object that conforms to "
            "the IAnyDataReader interface."
        );

    py::implicitly_convertible<pyrti::PyIAnyDataReader, pyrti::PyAnyDataReader>();
}

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyIAnyDataReader>& cls) {
    cls
        .def_property(
            "qos",
            (DataReaderQos (pyrti::PyIAnyDataReader::*)() const) &pyrti::PyIAnyDataReader::py_qos,
            (void (pyrti::PyIAnyDataReader::*)(const DataReaderQos&)) &pyrti::PyIAnyDataReader::py_qos,
            "The QoS for this AnyDataReader."
            "\n\n"
            "This property uses value semantics."
        )
        .def_property_readonly(
            "topic_name",
            (std::string (pyrti::PyIAnyDataReader::*)() const) &pyrti::PyIAnyDataReader::py_topic_name,
            "The Topic name for this AnyDataReader."
        )
        .def_property_readonly(
            "type_name",
            (std::string (pyrti::PyIAnyDataReader::*)() const) &pyrti::PyIAnyDataReader::py_type_name,
            "The type name for this AnyDataReader."
        )
        .def_property_readonly(
            "subscriber",
            (const PySubscriber& (pyrti::PyIAnyDataReader::*)() const) &pyrti::PyIAnyDataReader::py_subscriber,
            "The Publisher for this AnyDataReader."
        )
        .def(
            "close",
            &pyrti::PyIAnyDataReader::py_close,
            "Close this DataReader."
        )
        .def(
            "retain",
            &pyrti::PyIAnyDataReader::py_retain,
            "Retain this DataReader."
        )
        .def(
            "__eq__",
            [](pyrti::PyIAnyDataReader &adr, pyrti::PyIAnyDataReader& other) {
                return adr.get_any_datareader() == other.get_any_datareader();
            },
            py::is_operator(),
            "Test for equality."
        )
        .def(
            "__ne__",
            [](pyrti::PyIAnyDataReader &adr, pyrti::PyIAnyDataReader& other) {
                return adr.get_any_datareader() != other.get_any_datareader();
            },
            py::is_operator(),
            "Test for inequality."
        );
}

template<>
void pyrti::process_inits<AnyDataReader>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyIAnyDataReader>(m, "IAnyDataReader");
        }
    ); 
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyAnyDataReader, pyrti::PyIAnyDataReader>(m, "AnyDataReader");
        }
    );
}
