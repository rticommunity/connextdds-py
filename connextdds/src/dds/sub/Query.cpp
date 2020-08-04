#include "PyConnext.hpp"
#include "PyDataReader.hpp"
#include <dds/sub/Query.hpp>

using namespace dds::sub;

namespace pyrti {

template <> void init_class_defs(py::class_<Query> &cls) {
  cls.def(py::init([](PyIDataReader &dr, const std::string &expression) {
            return dr.create_query(expression);
          }),
          py::arg("reader"), py::arg("expression"),
          "Init a Query for a DataReader with an expression.")
      .def(py::init([](PyIDataReader &dr, const std::string &expression,
                       const std::vector<std::string> &params) {
             return dr.create_query(expression, params);
           }),
           py::arg("reader"), py::arg("expression"), py::arg("params"),
           "Init a Query for a DataReader with an expression and parameters.")
      .def_property_readonly("expression", &Query::expression,
                             "The expression.")
      .def_property(
          "parameters", [](const Query &q) { return q->parameters(); },
          [](Query &q, std::vector<std::string> &v) {
            q.parameters(v.begin(), v.end());
          },
          "The parameters for the expression.")
      .def_property_readonly("parameters_length", &Query::parameters_length,
                             "The parameter sequence length.")
      .def_property(
          "name", [](const Query &q) { return q->name(); },
          [](Query &q, const std::string &n) { q->name(n); },
          "The filter name.")
      .def_property_readonly(
          "data_reader",
          [](Query &q) { return PyAnyDataReader(q.data_reader()); },
          "The DataReader as an AnyDataReader.")
      .def("add_parameter", &Query::add_parameter, "Appends a parameter.")
      .def(
          "__iter__",
          [](Query &q) { return py::make_iterator(q.begin(), q.end()); },
          py::keep_alive<0, 1>())
      .def("__len__", &Query::parameters_length)
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.");
}

template <> void process_inits<Query>(py::module &m, ClassInitList &l) {
  l.push_back([m]() mutable { return init_class<Query>(m, "Query"); });
}

} // namespace pyrti
