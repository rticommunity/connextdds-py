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
#include "PyCondition.hpp"
#include <pybind11/functional.h>

namespace pyrti {

template<>
void init_class_defs(
        py::class_<
            PyGuardCondition,
            PyICondition,
            std::unique_ptr<PyGuardCondition, no_gil_delete<PyGuardCondition>>>& cls)
{
    cls.def(py::init<>(), "Create a GuardCondition in an untriggered state.")
            .def(py::init([](PyICondition& c) {
                     auto gc = c.get_condition();
                     return dds::core::polymorphic_cast<PyGuardCondition>(gc);
                 }),
                 py::arg("condition"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a GuardCondition from a Condition.")
            .def(
                    "set_handler",
                    [](PyGuardCondition& gc,
                       std::function<void(PyICondition*)>& func) {
                        gc->handler([func](dds::core::cond::Condition c) {
                            py::gil_scoped_acquire acquire;
                            auto py_c = dds::core::polymorphic_cast<
                                    PyGuardCondition>(c);
                            func(&py_c);
                        });
                    },
                    py::arg("func"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Set a handler function for this GuardCondition.")
            .def("reset_handler",
                 &PyGuardCondition::reset_handler,
                 py::call_guard<py::gil_scoped_release>(),
                 "Resets the handler for this GuardCondition.")
            .def_property(
                    "trigger_value",
                    [](PyGuardCondition& gc) {
                        py::gil_scoped_release release;
                        return gc.trigger_value();
                    },
                    [](PyGuardCondition& gc, bool trigger_value) {
                        py::gil_scoped_release release;
                        gc.trigger_value(trigger_value);
                    },
                    "Get/set the trigger value for this GuardCondition")
            .def(py::self == py::self,
                 py::call_guard<py::gil_scoped_release>(),
                 "Compare DataStateEx objects for equality.")
            .def(py::self != py::self,
                 py::call_guard<py::gil_scoped_release>(),
                 "Compare DataStateEx objects for inequality.");
}

template<>
void process_inits<dds::core::cond::GuardCondition>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<
            PyGuardCondition,
            PyICondition,
            std::unique_ptr<PyGuardCondition, no_gil_delete<PyGuardCondition>>>(m, "GuardCondition");
    });
}

}  // namespace pyrti
