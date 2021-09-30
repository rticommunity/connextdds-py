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
#include "PyCondition.hpp"

namespace pyrti {

template<>
void init_class_defs(
        py::class_<
            PyICondition,
            std::unique_ptr<PyICondition, no_gil_delete<PyICondition>>>& cls)
{
    cls.def("dispatch",
            &PyICondition::py_dispatch,
            "Dispatches the functions registered with the condition.")
            .def_property_readonly(
                    "trigger_value",
                    [](PyICondition& c) {
                        py::gil_scoped_release release;
                        return c.py_trigger_value();
                    },
                    "The trigger value of the condition.")
            .def(
                    "__eq__",
                    [](PyICondition& c, PyICondition& other) {
                        return c.get_condition() == other.get_condition();
                    },
                    py::is_operator(),
                    py::call_guard<py::gil_scoped_release>(),
                    "Compare Condition objects for equality.")
            .def(
                    "__ne__",
                    [](PyICondition& c, PyICondition& other) {
                        return c.get_condition() != other.get_condition();
                    },
                    py::is_operator(),
                    py::call_guard<py::gil_scoped_release>(),
                    "Compare Condition objects for inequality.");
}

template<>
void init_class_defs(
        py::class_<
            PyCondition,
            PyICondition,
            std::unique_ptr<PyCondition, no_gil_delete<PyCondition>>>& cls)
{
}

template<>
void process_inits<dds::core::cond::Condition>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class_with_ptr_seq<
            PyICondition,
            std::unique_ptr<PyICondition, no_gil_delete<PyICondition>>>(
                m,
                "ICondition");
    });

    l.push_back([m]() mutable {
        return init_class_with_seq<
            PyCondition,
            PyICondition,
            std::unique_ptr<PyCondition, no_gil_delete<PyCondition>>>(
                m,
                "Condition");
    });
}

}  // namespace pyrti
