#include "PyConnext.hpp"
#include "PyEntity.hpp"
#include "PyCondition.hpp"

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyStatusCondition, pyrti::PyICondition>& cls) {
    cls
        .def(
            py::init(
                [](pyrti::PyIEntity& e) {
                    return pyrti::PyStatusCondition(e.get_entity());
                }
            ),
            py::arg("entity"),
            py::keep_alive<1, 2>(),
            "Obtain a referene to an entity's StatusCondition object")
        .def(
            py::init(
                [](pyrti::PyICondition& c) {
                    auto sc = c.get_condition();
                    return dds::core::polymorphic_cast<pyrti::PyStatusCondition>(sc);
                }
            ),
            py::arg("condition"),
            "Downcast a Condition to a StatusCondition."
        )
        .def_property(
            "enabled_statuses",
            (const dds::core::status::StatusMask (pyrti::PyStatusCondition::*)() const) &pyrti::PyStatusCondition::enabled_statuses,
            (void (pyrti::PyStatusCondition::*)(const dds::core::status::StatusMask&)) &pyrti::PyStatusCondition::enabled_statuses,
            "Get/set the enabled statuses for this condition."
        )
        .def_property_readonly(
            "entity",
            [](const pyrti::PyStatusCondition& sc) {
                auto e = pyrti::PyEntity(sc.entity());
                return e;
            },
            "Get the Entity associated with this StatusCondition."
        )
        .def(
            "handler",
            [](pyrti::PyStatusCondition& sc, std::function<void()>& func_obj) {
                sc->handler(func_obj);
            },
            py::arg("func"),
            "Set a handler function for this StatusCondition."
        )
        .def(
            "handler",
            [](pyrti::PyStatusCondition& sc, std::function<void(pyrti::PyICondition&)>& func_obj) {
                sc->handler(
                    [func_obj](dds::core::cond::Condition c) {
                        auto py_c = pyrti::PyCondition(c);
                        func_obj(py_c);
                    });
            },
            py::arg("func"),
            "Set a handler function for this StatusCondition."
        )
        .def(
            "reset_handler",
            [](pyrti::PyStatusCondition& sc) {
                sc->reset_handler();
            },
            "Resets the handler for this StatusCondition."
        )
        .def(
            "dispatch",
            &pyrti::PyStatusCondition::dispatch,
            "Dispatches the functions registered with the condition."
        )
        .def_property_readonly(
            "trigger_value",
            &pyrti::PyStatusCondition::trigger_value,
            "The trigger value of the condition."
        )
        .def(
            py::self == py::self,
            "Compare StatusCondition objects for equality."
        )
        .def(
            py::self != py::self,
            "Compare StatusCondition objects for inequality."
        );
}

template<>
void pyrti::process_inits<dds::core::cond::StatusCondition>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyStatusCondition, pyrti::PyICondition>(m, "StatusCondition");
        }
    );
}
