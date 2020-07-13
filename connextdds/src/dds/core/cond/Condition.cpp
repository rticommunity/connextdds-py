#include "PyConnext.hpp"
#include "PySeq.hpp"
#include "PyCondition.hpp"

namespace pyrti {

template<>
void init_class_defs(py::class_<PyICondition>& cls) {
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
            [](PyICondition& c, PyICondition& other) {
                return c.get_condition() == other.get_condition();
            },
            py::is_operator(),
            "Compare Condition objects for equality."
        )
        .def(
            "__ne__",
            [](PyICondition& c, PyICondition& other) {
                return c.get_condition() != other.get_condition();
            },
            py::is_operator(),
            "Compare Condition objects for inequality."
        );
}

template<>
void init_class_defs(py::class_<PyCondition, PyICondition>& cls) {}

template<>
void process_inits<dds::core::cond::Condition>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class_with_ptr_seq<PyICondition>(m, "ICondition");
        }
    );

    l.push_back(
        [m]() mutable {
            return init_class_with_seq<PyCondition, PyICondition>(m, "Condition");
        }
    );
}

}
