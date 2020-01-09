#include "PyConnext.hpp"
#include <dds/sub/cond/QueryCondition.hpp>
#include "PyCondition.hpp"

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyQueryCondition, pyrti::PyIReadCondition>& cls) {
    cls
        .def(
            py::init<
                const dds::sub::Query&,
                const dds::sub::status::DataState&
            >(),
            "Create a QueryCondition."
        )
        .def(
            py::init(
                [](const dds::sub::Query& q, const dds::sub::status::DataState& ds, std::function<void()>& func) {
                    return pyrti::PyQueryCondition(q, ds, func);
                }
            ),
            "Create a QueryCondition."
        )
        .def(
            py::init(
                [](const dds::sub::Query& q, const dds::sub::status::DataState& ds, std::function<void(pyrti::PyICondition&)>& func) {
                    return pyrti::PyQueryCondition(
                        q, 
                        ds, 
                        [func](dds::core::cond::Condition c){ 
                            auto py_c = pyrti::PyCondition(c);
                            func(py_c); 
                        }
                    );
                }
            ),
            "Create a QueryCondition."
        )
        .def(
            py::init(
                [](const dds::sub::Query& q, const rti::sub::status::DataStateEx& ds) {
                    return pyrti::PyQueryCondition(rti::sub::cond::create_query_condition_ex(q, ds));
                }
            ),
            "Create a QueryCondition."
        )
        .def(
            py::init(
                [](const dds::sub::Query& q, const rti::sub::status::DataStateEx& ds, std::function<void()>& func) {
                    return pyrti::PyQueryCondition(rti::sub::cond::create_query_condition_ex(q, ds, func));
                }
            ),
            "Create a QueryCondition."
        )
        .def(
            py::init(
                [](const dds::sub::Query& q, const rti::sub::status::DataStateEx& ds, std::function<void(pyrti::PyICondition&)>& func) {
                    return pyrti::PyQueryCondition(
                        rti::sub::cond::create_query_condition_ex(
                            q, 
                            ds, 
                            [func](dds::core::cond::Condition c){ 
                                auto py_c = pyrti::PyCondition(c);
                                func(py_c);
                             }
                        )
                    );
                })
            ,
            "Create a QueryCondition."
        )
        .def(
            py::init(
                [](pyrti::PyICondition& py_c) {
                    auto condition = py_c.get_condition();
                    return pyrti::PyQueryCondition(dds::core::polymorphic_cast<dds::sub::cond::QueryCondition>(condition));
                }
            ),
            "Create a QueryCondition."
        )
        .def_property_readonly(
            "expression",
            &pyrti::PyQueryCondition::expression,
            "The expression."
        )
        .def_property(
            "parameters",
            (std::vector<std::string> (pyrti::PyQueryCondition::*)() const) &pyrti::PyQueryCondition::parameters,
            [](pyrti::PyQueryCondition& q, std::vector<std::string>& v) {
                q.parameters(v.begin(), v.end());
            },
            "The parameters for the expression."
        )
        .def_property_readonly(
            "parameters_length",
            &pyrti::PyQueryCondition::parameters_length,
            "The parameter sequence length."
        )
        .def(
            "__len__",
            &pyrti::PyQueryCondition::parameters_length
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );

}

template<>
void pyrti::process_inits<dds::sub::cond::QueryCondition>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyQueryCondition, pyrti::PyIReadCondition>(m, "QueryCondition");
        }
    );
}
