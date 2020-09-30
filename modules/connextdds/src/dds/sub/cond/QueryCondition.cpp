#include "PyConnext.hpp"
#include <pybind11/functional.h>
#include <dds/sub/cond/QueryCondition.hpp>
#include "PyCondition.hpp"

namespace pyrti {

template<>
void init_class_defs(py::class_<PyQueryCondition, PyIReadCondition>& cls)
{
    cls.def(py::init<
                    const dds::sub::Query&,
                    const dds::sub::status::DataState&>(),
            py::arg("query"),
            py::arg("status"),
            "Create a QueryCondition.")
#if rti_connext_version_gte(6, 0, 0)
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
                 "Create a QueryCondition.")
#endif
            .def(py::init([](const dds::sub::Query& q,
                             const rti::sub::status::DataStateEx& ds) {
                     return PyQueryCondition(
                             rti::sub::cond::create_query_condition_ex(q, ds));
                 }),
                 py::arg("query"),
                 py::arg("status_ex"),
                 "Create a QueryCondition.")
#if rti_connext_version_gte(6, 0, 0)
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
                 "Cast a condition to a QueryCondition.")
#endif
            .def_property_readonly(
                    "expression",
                    &PyQueryCondition::expression,
                    "The expression.")
            .def_property(
                    "parameters",
                    (std::vector<std::string>(PyQueryCondition::*)() const)
                            & PyQueryCondition::parameters,
                    [](PyQueryCondition& q, std::vector<std::string>& v) {
                        q.parameters(v.begin(), v.end());
                    },
                    "The parameters for the expression.")
            .def_property_readonly(
                    "parameters_length",
                    &PyQueryCondition::parameters_length,
                    "The parameter sequence length.")
            .def("__len__", &PyQueryCondition::parameters_length)
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<dds::sub::cond::QueryCondition>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<PyQueryCondition, PyIReadCondition>(
                m,
                "QueryCondition");
    });
}

}  // namespace pyrti
