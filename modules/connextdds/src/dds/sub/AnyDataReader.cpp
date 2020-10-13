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
#include <dds/sub/AnyDataReader.hpp>
#include "PyAnyDataReader.hpp"

using namespace dds::sub;
using namespace dds::sub::qos;

namespace pyrti {

template<>
void init_class_defs(py::class_<PyAnyDataReader, PyIAnyDataReader>& cls)
{
    cls.def(py::init<const PyIAnyDataReader&>(),
            py::arg("reader"),
            "Create an AnyDataReader instance from a object that conforms to "
            "the IAnyDataReader interface.");

    py::implicitly_convertible<PyIAnyDataReader, PyAnyDataReader>();
}

template<>
void init_class_defs(py::class_<PyIAnyDataReader>& cls)
{
    cls.def_property(
               "qos",
               (DataReaderQos(PyIAnyDataReader::*)() const)
                       & PyIAnyDataReader::py_qos,
               (void (PyIAnyDataReader::*)(const DataReaderQos&))
                       & PyIAnyDataReader::py_qos,
               "The QoS for this AnyDataReader."
               "\n\n"
               "This property's getter returns a deep copy.")
            .def_property_readonly(
                    "topic_name",
                    (std::string(PyIAnyDataReader::*)() const)
                            & PyIAnyDataReader::py_topic_name,
                    "The Topic name for this AnyDataReader.")
            .def_property_readonly(
                    "type_name",
                    (std::string(PyIAnyDataReader::*)() const)
                            & PyIAnyDataReader::py_type_name,
                    "The type name for this AnyDataReader.")
            .def_property_readonly(
                    "subscriber",
                    (const PySubscriber& (PyIAnyDataReader::*) () const)
                            & PyIAnyDataReader::py_subscriber,
                    "The Publisher for this AnyDataReader.")
            .def("close", &PyIAnyDataReader::py_close, "Close this DataReader.")
            .def("retain",
                 &PyIAnyDataReader::py_retain,
                 "Retain this DataReader.")
            .def(
                    "__eq__",
                    [](PyIAnyDataReader& adr, PyIAnyDataReader& other) {
                        return adr.get_any_datareader()
                                == other.get_any_datareader();
                    },
                    py::is_operator(),
                    "Test for equality.")
            .def(
                    "__ne__",
                    [](PyIAnyDataReader& adr, PyIAnyDataReader& other) {
                        return adr.get_any_datareader()
                                != other.get_any_datareader();
                    },
                    py::is_operator(),
                    "Test for inequality.");
}

template<>
void process_inits<AnyDataReader>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class_with_ptr_seq<PyIAnyDataReader>(m, "IAnyDataReader");
    });
    l.push_back([m]() mutable {
        return init_class_with_seq<PyAnyDataReader, PyIAnyDataReader>(
                m,
                "AnyDataReader");
    });
}

}  // namespace pyrti
