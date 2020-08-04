#include "PyAnyDataReader.hpp"
#include "PyCondition.hpp"
#include "PyConnext.hpp"
#include <dds/sub/cond/QueryCondition.hpp>
#include <dds/sub/cond/ReadCondition.hpp>
#include <pybind11/functional.h>

namespace pyrti {

template <>
void init_class_defs(py::class_<PyReadCondition, PyIReadCondition> &cls) {
  cls.def(py::init(
              [](PyIAnyDataReader &adr, const dds::sub::status::DataState &ds) {
                return PyReadCondition(rti::sub::cond::create_read_condition_ex(
                    adr.get_any_datareader(), ds));
              }),
          py::arg("reader"), py::arg("status"), "Create a ReadCondition.")
#if rti_connext_version_gte(6, 0, 0)
      .def(py::init([](PyIAnyDataReader &dr,
                       const dds::sub::status::DataState &ds,
                       std::function<void(PyICondition *)> &func) {
             return PyReadCondition(rti::sub::cond::create_read_condition_ex(
                 dr.get_any_datareader(), ds,
                 [&func](dds::core::cond::Condition c) {
                   py::gil_scoped_acquire acquire;
                   auto py_c = dds::core::polymorphic_cast<PyReadCondition>(c);
                   func(&py_c);
                 }));
           }),
           py::arg("reader"), py::arg("status"), py::arg("handler"),
           "Create a ReadCondition.")
#endif
      .def(py::init([](PyIAnyDataReader &dr,
                       const rti::sub::status::DataStateEx &ds) {
             return PyReadCondition(rti::sub::cond::create_read_condition_ex(
                 dr.get_any_datareader(), ds));
           }),
           py::arg("reader"), py::arg("status"), "Create a ReadCondition.")
#if rti_connext_version_gte(6, 0, 0)
      .def(py::init([](PyIAnyDataReader &dr,
                       const rti::sub::status::DataStateEx &ds,
                       std::function<void(PyICondition *)> &func) {
             return PyReadCondition(rti::sub::cond::create_read_condition_ex(
                 dr.get_any_datareader(), ds,
                 [&func](dds::core::cond::Condition c) {
                   py::gil_scoped_acquire acquire;
                   auto py_c = dds::core::polymorphic_cast<PyReadCondition>(c);
                   func(&py_c);
                 }));
           }),
           py::arg("reader"), py::arg("status"), py::arg("handler"),
           py::keep_alive<1, 4>(), "Create a ReadCondition.")
      .def(py::init([](PyICondition &py_c) {
             auto c = py_c.get_condition();
             return PyReadCondition(
                 dds::core::polymorphic_cast<dds::sub::cond::ReadCondition>(c));
           }),
           "Cast a compatible Condition to a ReadCondition.");
#else
      .def(py::init([](PyICondition &py_c) {
             auto c = py_c.get_condition();
             auto rcd = rtiboost::dynamic_pointer_cast<
                 rti::sub::cond::ReadConditionImpl>(c.delegate());
             if (rcd.get() == nullptr)
               throw dds::core::InvalidDowncastError(
                   "Could not create ReadCondtion from Condition");
             return PyReadCondition(rcd.get());
           }),
           "Cast a compatible Condition to a ReadCondition.");
#endif
}

template <>
void init_class_defs(py::class_<PyIReadCondition, PyICondition> &cls) {
  cls.def_property_readonly("state_filter", &PyIReadCondition::py_state_filter,
                            "Returns the DataState of this condition.")
      .def_property_readonly(
          "data_reader", &PyIReadCondition::py_data_reader,
          "Returns the DataReader associated to this condition.")
      .def("close", &PyIReadCondition::py_close,
           "Returns the DataReader associated to this condition.")
      .def_property_readonly(
          "closed", &PyIReadCondition::py_closed,
          "Returns the DataReader associated to this condition.")
      .def("__enter__",
           [](PyIReadCondition &rc) -> PyIReadCondition & { return rc; })
      .def("__exit__", [](PyIReadCondition &rc, py::object, py::object,
                          py::object) { rc.py_close(); })
      .def(
          "__eq__",
          [](PyIReadCondition &rc, PyIReadCondition &other) {
            return rc.get_read_condition() == other.get_read_condition();
          },
          py::is_operator(), "Test for equality.")
      .def(
          "__ne__",
          [](PyIReadCondition &rc, PyIReadCondition &other) {
            return rc.get_read_condition() != other.get_read_condition();
          },
          py::is_operator(), "Test for inequality.");
}

template <>
void process_inits<dds::sub::cond::ReadCondition>(py::module &m,
                                                  ClassInitList &l) {
  l.push_back([m]() mutable {
    return init_class<PyIReadCondition, PyICondition>(m, "IReadCondition");
  });

  l.push_back([m]() mutable {
    return init_class<PyReadCondition, PyIReadCondition>(m, "ReadCondition");
  });
}

} // namespace pyrti
