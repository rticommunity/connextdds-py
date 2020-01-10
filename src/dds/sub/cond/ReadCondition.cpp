#include "PyConnext.hpp"
#include <pybind11/functional.h>
#include <dds/sub/cond/ReadCondition.hpp>
#include <dds/sub/cond/QueryCondition.hpp>
#include "PyCondition.hpp"
#include "PyAnyDataReader.hpp"

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyReadCondition, pyrti::PyIReadCondition>& cls) {
    cls
        .def(
            py::init([](pyrti::PyIAnyDataReader& adr, const dds::sub::status::DataState& ds) {
                return pyrti::PyReadCondition(rti::sub::cond::create_read_condition_ex(adr.get_any_datareader(), ds));
            }),
            py::arg("reader"),
            py::arg("status"),
            "Create a ReadCondition."
        )
        .def(
            py::init(
                [](pyrti::PyIAnyDataReader& dr, const dds::sub::status::DataState& ds, std::function<void()>& func) {
                    return pyrti::PyReadCondition(
                        rti::sub::cond::create_read_condition_ex(
                            dr.get_any_datareader(), 
                            ds,
                            func
                        )
                    );
                }
            ),
            py::arg("reader"),
            py::arg("status"),
            py::arg("handler"),
            py::keep_alive<1, 3>(),
            "Create a ReadCondition."
        )
        .def(
            py::init(
                [](pyrti::PyIAnyDataReader& dr, const rti::sub::status::DataStateEx& ds) {
                    return pyrti::PyReadCondition(
                        rti::sub::cond::create_read_condition_ex(
                            dr.get_any_datareader(), 
                            ds
                        )
                    );
                }
            ),
            py::arg("reader"),
            py::arg("status"),
            "Create a ReadCondition."
        )
        .def(
            py::init(
                [](pyrti::PyIAnyDataReader& dr, const rti::sub::status::DataStateEx& ds, std::function<void()>& func) {
                    return pyrti::PyReadCondition(
                        rti::sub::cond::create_read_condition_ex(
                            dr.get_any_datareader(), 
                            ds,
                            func
                        )
                    );
                }
            ),
            py::arg("reader"),
            py::arg("status"),
            py::arg("handler"),
            "Create a ReadCondition."
        )
        .def(
            py::init(
                [](pyrti::PyICondition& py_c) {
                    auto c = py_c.get_condition();
                    return pyrti::PyReadCondition(dds::core::polymorphic_cast<dds::sub::cond::ReadCondition>(c));
                }
            ),
            "Cast a compatible Condition to a ReadCondition."
        );
}

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyIReadCondition, pyrti::PyICondition>& cls) {
    cls
        .def_property_readonly(
            "state_filter",
            &pyrti::PyIReadCondition::py_state_filter,
            "Returns the DataState of this condition."
        )
        .def_property_readonly(
            "data_reader",
            &pyrti::PyIReadCondition::py_data_reader,
            "Returns the DataReader associated to this condition."
        )
        .def(
            "close",
            &pyrti::PyIReadCondition::py_close,
            "Returns the DataReader associated to this condition."
        )
        .def_property_readonly(
            "closed",
            &pyrti::PyIReadCondition::py_closed,
            "Returns the DataReader associated to this condition."
        )
        .def(
            "__enter__",
            [](pyrti::PyIReadCondition& rc) -> pyrti::PyIReadCondition& {
                return rc;
            }
        )
        .def(
            "__exit__",
            [](pyrti::PyIReadCondition& rc) {
                rc.py_close();
            }
        )
        .def(
            "__eq__",
            [](pyrti::PyIReadCondition& rc, pyrti::PyIReadCondition& other) {
                return rc.get_read_condition() == other.get_read_condition();
            },
            py::is_operator(),
            "Test for equality."
        )
        .def(
            "__ne__",
            [](pyrti::PyIReadCondition& rc, pyrti::PyIReadCondition& other) {
                return rc.get_read_condition() != other.get_read_condition();
            },
            py::is_operator(),
            "Test for inequality."
        );
}

template<>
void pyrti::process_inits<dds::sub::cond::ReadCondition>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyIReadCondition, pyrti::PyICondition>(m, "IReadCondition");
        }
    );

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyReadCondition, pyrti::PyIReadCondition>(m, "ReadCondition");
        }
    );
}
