#include "PyConnext.hpp"
#include "PyCondition.hpp"

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyGuardCondition, pyrti::PyICondition>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a GuardCondition in an untriggered state."
        )
        .def(
            py::init(
                [](pyrti::PyICondition& c) {
                    auto gc = c.get_condition();
                    return dds::core::polymorphic_cast<pyrti::PyGuardCondition>(gc);
                }
            ),
            py::arg("condition"),
            "Create a GuardCondition from a Condition."
        )
        .def(
            "handler",
            [](pyrti::PyGuardCondition& gc, std::function<void()>& func_obj) {
                gc->handler(func_obj);
            },
            py::arg("func"),
            "Set a handler function for this GuardCondition."
        )
        .def(
            "handler",
            [](pyrti::PyGuardCondition& gc, std::function<void(pyrti::PyICondition&)>& func_obj) {
                gc->handler(
                    [func_obj](dds::core::cond::Condition c) {
                        auto py_c = pyrti::PyCondition(c);
                        func_obj(py_c);
                    });
            },
            py::arg("func"),
            "Set a handler function for this GuardCondition."
        )
        .def(
            "reset_handler",
            &pyrti::PyGuardCondition::reset_handler,
            "Resets the handler for this GuardCondition."
        )
        .def_property(
            "trigger_value",
            (bool (pyrti::PyGuardCondition::*)() const) &pyrti::PyGuardCondition::trigger_value,
            (void (pyrti::PyGuardCondition::*)(bool)) &pyrti::PyGuardCondition::trigger_value,
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
void pyrti::process_inits<dds::core::cond::GuardCondition>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable{
            return pyrti::init_class<pyrti::PyGuardCondition, pyrti::PyICondition>(m, "GuardCondition");
        }
    );
}