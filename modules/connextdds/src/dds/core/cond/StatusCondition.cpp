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
#include "PyEntity.hpp"
#include "PyCondition.hpp"
#include <pybind11/functional.h>
namespace pyrti {

template<>
void init_class_defs(
        py::class_<
            PyStatusCondition,
            PyICondition,
            std::unique_ptr<PyStatusCondition, no_gil_delete<PyStatusCondition>>>& cls)
{
    cls.def(py::init([](PyIEntity& e) {
                return PyStatusCondition(e.get_entity());
            }),
            py::arg("entity"),
            py::keep_alive<1, 2>(),
            "Obtain a referene to an entity's StatusCondition object")
            .def(py::init([](PyICondition& c) {
                     auto sc = c.get_condition();
                     return dds::core::polymorphic_cast<PyStatusCondition>(sc);
                 }),
                 py::arg("condition"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Downcast a Condition to a StatusCondition.")
            .def_property(
                    "enabled_statuses",
                    [](PyStatusCondition& sc) { 
                        py::gil_scoped_release release;
                        return sc.enabled_statuses();
                    },
                    [](PyStatusCondition& sc, const dds::core::status::StatusMask& mask) { 
                        py::gil_scoped_release release;
                        sc.enabled_statuses(mask);
                    },
                    "Get/set the enabled statuses for this condition.")
            .def_property_readonly(
                    "entity",
                    [](const PyStatusCondition& sc) {
                        auto e = PyEntity(sc.entity());
                        return e;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the Entity associated with this StatusCondition.")
            .def(
                    "set_handler",
                    [](PyStatusCondition& sc,
                       std::function<void(PyICondition*)>& func) {
                        sc->handler([func](dds::core::cond::Condition c) {
                            py::gil_scoped_acquire acquire;
                            auto py_c = dds::core::polymorphic_cast<
                                    PyStatusCondition>(c);
                            func(&py_c);
                        });
                    },
                    py::arg("func"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Set a handler function for this StatusCondition.")
            .def(
                    "reset_handler",
                    [](PyStatusCondition& sc) { sc->reset_handler(); },
                    py::call_guard<py::gil_scoped_release>(),
                    "Resets the handler for this StatusCondition.")
            .def("dispatch",
                 &PyStatusCondition::dispatch,
                 py::call_guard<py::gil_scoped_release>(),
                 "Dispatches the functions registered with the condition.")
            .def_property_readonly(
                    "trigger_value",
                    &PyStatusCondition::trigger_value,
                    py::call_guard<py::gil_scoped_release>(),
                    "The trigger value of the condition.")
            .def(py::self == py::self,
                 py::call_guard<py::gil_scoped_release>(),
                 "Compare StatusCondition objects for equality.")
            .def(py::self != py::self,
                 py::call_guard<py::gil_scoped_release>(),
                 "Compare StatusCondition objects for inequality.");
}

template<>
void process_inits<dds::core::cond::StatusCondition>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<
            PyStatusCondition,
            PyICondition,
            std::unique_ptr<PyStatusCondition, no_gil_delete<PyStatusCondition>>>(
                m,
                "StatusCondition");
    });
}

}  // namespace pyrti
