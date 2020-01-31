#include "PyConnext.hpp"
#include "PyCondition.hpp"
#include <pybind11/functional.h>

namespace pyrti {

template<>
void init_class_defs(py::class_<PyGuardCondition, PyICondition>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a GuardCondition in an untriggered state."
        )
        .def(
            py::init(
                [](PyICondition& c) {
                    auto gc = c.get_condition();
                    return dds::core::polymorphic_cast<PyGuardCondition>(gc);
                }
            ),
            py::arg("condition"),
            "Create a GuardCondition from a Condition."
        )
        .def(
            "handler",
            [](PyGuardCondition& gc, std::function<void()>& func) {
                gc->handler(func);
            },
            py::arg("func"),
            "Set a handler function for this GuardCondition."
        )
        .def(
            "reset_handler",
            &PyGuardCondition::reset_handler,
            "Resets the handler for this GuardCondition."
        )
        .def_property(
            "trigger_value",
            (bool (PyGuardCondition::*)() const) &PyGuardCondition::trigger_value,
            (void (PyGuardCondition::*)(bool)) &PyGuardCondition::trigger_value,
            "Get/set the trigger value for this GuardCondition"
        )
        .def(
            py::self == py::self,
            "Compare DataStateEx objects for equality."
        )
        .def(
            py::self != py::self,
            "Compare DataStateEx objects for inequality."
        );
}

template<>
void process_inits<dds::core::cond::GuardCondition>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable{
            return init_class<PyGuardCondition, PyICondition>(m, "GuardCondition");
        }
    );
}

}
