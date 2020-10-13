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
#include <dds/topic/AnyTopic.hpp>
#include "PyAnyTopic.hpp"

using namespace dds::topic;
using namespace dds::topic::qos;

namespace pyrti {

template<>
void init_class_defs(py::class_<PyAnyTopic>& cls)
{
    cls.def(py::init<const PyIAnyTopic&>(),
            py::arg("topic"),
            "Create an AnyTopic instance from a object that conforms to the "
            "IAnyTopic interface.");

    py::implicitly_convertible<PyIAnyTopic, PyAnyTopic>();
}

template<>
void init_class_defs(py::class_<PyIAnyTopic>& cls)
{
    cls.def_property(
               "qos",
               (TopicQos(PyIAnyTopic::*)() const) & PyIAnyTopic::py_qos,
               (void (PyIAnyTopic::*)(const TopicQos&)) & PyIAnyTopic::py_qos,
               "The QoS for this AnyTopic."
               "\n\n"
               "This property's getter returns a deep copy.")
            .def_property_readonly(
                    "name",
                    (std::string(PyIAnyTopic::*)() const)
                            & PyIAnyTopic::py_name,
                    "The Topic name for this AnyTopic.")
            .def_property_readonly(
                    "type_name",
                    (std::string(PyIAnyTopic::*)() const)
                            & PyIAnyTopic::py_type_name,
                    "The type name for this AnyTopic.")
            .def("close", &PyIAnyTopic::py_close, "Close this Topic.")
            .def(
                    "__eq__",
                    [](PyIAnyTopic& at, PyIAnyTopic& other) {
                        return at.get_any_topic() == other.get_any_topic();
                    },
                    py::is_operator(),
                    "Test for equality.")
            .def(
                    "__ne__",
                    [](PyIAnyTopic& at, PyIAnyTopic& other) {
                        return at.get_any_topic() != other.get_any_topic();
                    },
                    py::is_operator(),
                    "Test for inequality.");
}

template<>
void process_inits<AnyTopic>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class_with_ptr_seq<PyIAnyTopic>(m, "IAnyTopic");
    });

    l.push_back([m]() mutable {
        return init_class_with_seq<PyAnyTopic>(m, "AnyTopic");
    });
}

}  // namespace pyrti
