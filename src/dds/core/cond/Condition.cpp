#include "PyConnext.hpp"
#include "PyCondition.hpp"

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyICondition>& cls) {
    cls
        .def(
            "dispatch",
            &PyICondition::py_dispatch,
            "Dispatches the functions registered with the condition."
        )
        .def_property_readonly(
            "trigger_value",
            &PyICondition::py_trigger_value,
            "The trigger value of the condition."
        )
        .def(
            "__eq__",
            [](pyrti::PyICondition& c, pyrti::PyICondition& other) {
                return c.get_condition() == other.get_condition();
            },
            py::is_operator(),
            "Compare Condition objects for equality."
        )
        .def(
            "__ne__",
            [](pyrti::PyICondition& c, pyrti::PyICondition& other) {
                return c.get_condition() != other.get_condition();
            },
            py::is_operator(),
            "Compare Condition objects for inequality."
        );
}

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyCondition, pyrti::PyICondition>& cls) {}

template<>
void pyrti::process_inits<dds::core::cond::Condition>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyICondition>(m, "ICondition");
        }
    );

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyCondition, pyrti::PyICondition>(m, "Condition");
        }
    );
}
