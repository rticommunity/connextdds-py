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
#include <pybind11/functional.h>
#include <dds/sub/cond/QueryCondition.hpp>
#include "PyCondition.hpp"

namespace pyrti {

template<>
void init_class_defs(
        py::class_<
            PyQueryCondition,
            PyIReadCondition,
            std::unique_ptr<PyQueryCondition, no_gil_delete<PyQueryCondition>>>& cls)
{
    cls.def(py::init<
                    const dds::sub::Query&,
                    const dds::sub::status::DataState&>(),
            py::arg("query"),
            py::arg("status"),
            "Create a QueryCondition.")
#if rti_connext_version_gte(6, 0, 0, 0)
            .def(py::init([](const dds::sub::Query& q,
                             const dds::sub::status::DataState& ds,
                             std::function<void(PyICondition*)>& func) {
                     return PyQueryCondition(
                             q,
                             ds,
                             [func](dds::core::cond::Condition c) {
                                 py::gil_scoped_acquire acquire;
                                 auto py_c = dds::core::polymorphic_cast<
                                         PyQueryCondition>(c);
                                 func(&py_c);
                             });
                 }),
                 py::arg("query"),
                 py::arg("status"),
                 py::arg("handler"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a QueryCondition.")
#endif
            .def(py::init([](const dds::sub::Query& q,
                             const rti::sub::status::DataStateEx& ds) {
                     return PyQueryCondition(
                             rti::sub::cond::create_query_condition_ex(q, ds));
                 }),
                 py::arg("query"),
                 py::arg("status_ex"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a QueryCondition.")
#if rti_connext_version_gte(6, 0, 0, 0)
            .def(py::init([](const dds::sub::Query& q,
                             const rti::sub::status::DataStateEx& ds,
                             std::function<void(PyICondition*)>& func) {
                     return PyQueryCondition(
                             rti::sub::cond::create_query_condition_ex(
                                     q,
                                     ds,
                                     [func](dds::core::cond::Condition c) {
                                         py::gil_scoped_acquire acquire;
                                         auto py_c =
                                                 dds::core::polymorphic_cast<
                                                         PyQueryCondition>(c);
                                         func(&py_c);
                                     }));
                 }),
                 py::arg("query"),
                 py::arg("status_ex"),
                 py::arg("handler"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a QueryCondition.")
            .def(py::init([](PyICondition& py_c) {
                     auto condition = py_c.get_condition();
                     return PyQueryCondition(dds::core::polymorphic_cast<
                                             dds::sub::cond::QueryCondition>(
                             condition));
                 }),
                 py::arg("condition"),
                 "Cast a condition to a QueryCondition.")
#else
            .def(py::init([](PyICondition& py_c) {
                     auto c = py_c.get_condition();
                     auto qcd = rtiboost::dynamic_pointer_cast<
                             rti::sub::cond::QueryConditionImpl>(c.delegate());
                     if (qcd.get() == nullptr)
                         throw dds::core::InvalidDowncastError(
                                 "Could not create QueryCondtion from "
                                 "Condition");
                     return PyQueryCondition(qcd.get());
                 }),
                 py::arg("condition"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Cast a condition to a QueryCondition.")
#endif
            .def_property_readonly(
                    "expression",
                    [](PyQueryCondition& qc) {
                        py::gil_scoped_release relase;
                        return qc.expression();
                    },
                    "The expression.")
            .def_property(
                    "parameters",
                    [](PyQueryCondition& qc) {
                        py::gil_scoped_release release;
                        return qc.parameters();
                    },
                    [](PyQueryCondition& q, std::vector<std::string>& v) {
                        py::gil_scoped_release release;
                        q.parameters(v.begin(), v.end());
                    },
                    "The parameters for the expression.")
            .def_property_readonly(
                    "parameters_length",
                    [](PyQueryCondition& qc) {
                        py::gil_scoped_release release;
                        return qc.parameters_length();
                    },
                    "The parameter sequence length.")
            .def(
                    "__len__",
                    &PyQueryCondition::parameters_length,
                    py::call_guard<py::gil_scoped_release>())
            .def(
                    py::self == py::self,
                    py::call_guard<py::gil_scoped_release>(),
                    "Test for equality.")
            .def(
                    py::self != py::self,
                    py::call_guard<py::gil_scoped_release>(),
                    "Test for inequality.");

    cls.def(
            "set_handler_no_args",
            [](PyQueryCondition& rc, std::function<void()>& func) {
                // PY-41: Temporary workaround until handler() is exposed
                // in ReadCondition
                auto workaround_gc =
                        reinterpret_cast<rti::core::cond::GuardCondition*>(
                                rc.delegate().get());
                workaround_gc->handler([func]() {
                    py::gil_scoped_acquire acquire;
                    func();
                });
            },
            py::arg("func"),
            py::call_guard<py::gil_scoped_release>(),
            "Set a handler function receiving no arguments.");
    cls.def(
            "reset_handler",
            [](PyQueryCondition& rc) {
                // PY-41: Temporary workaround until reset_handler is exposed
                // in ReadCondition
                auto workaround_gc =
                        reinterpret_cast<rti::core::cond::GuardCondition*>(
                                rc.delegate().get());
                workaround_gc->reset_handler();
            },
            py::call_guard<py::gil_scoped_release>(),
            "Resets the handler for this QueryCondition.");
}

template<>
void process_inits<dds::sub::cond::QueryCondition>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<
            PyQueryCondition,
            PyIReadCondition,
            std::unique_ptr<PyQueryCondition, no_gil_delete<PyQueryCondition>>>(
                m,
                "QueryCondition");
    });
}

}  // namespace pyrti
