#include "PyConnext.hpp"
#include "PyEntity.hpp"
#include "PyCondition.hpp"
#include <pybind11/functional.h>
namespace pyrti {

template<>
void init_class_defs(py::class_<PyStatusCondition, PyICondition>& cls) {
    cls
        .def(
            py::init(
                [](PyIEntity& e) {
                    return PyStatusCondition(e.get_entity());
                }
            ),
            py::arg("entity"),
            py::keep_alive<1, 2>(),
            "Obtain a referene to an entity's StatusCondition object")
        .def(
            py::init(
                [](PyICondition& c) {
                    auto sc = c.get_condition();
                    return dds::core::polymorphic_cast<PyStatusCondition>(sc);
                }
            ),
            py::arg("condition"),
            "Downcast a Condition to a StatusCondition."
        )
        .def_property(
            "enabled_statuses",
            (const dds::core::status::StatusMask (PyStatusCondition::*)() const) &PyStatusCondition::enabled_statuses,
            (void (PyStatusCondition::*)(const dds::core::status::StatusMask&)) &PyStatusCondition::enabled_statuses,
            "Get/set the enabled statuses for this condition."
        )
        .def_property_readonly(
            "entity",
            [](const PyStatusCondition& sc) {
                auto e = PyEntity(sc.entity());
                return e;
            },
            "Get the Entity associated with this StatusCondition."
        )
        .def(
            "handler",
            [](PyStatusCondition& sc, std::function<void()>& func) {
                sc->handler(func);
            },
            py::arg("func"),
            "Set a handler function for this StatusCondition."
        )
        .def(
            "reset_handler",
            [](PyStatusCondition& sc) {
                sc->reset_handler();
            },
            "Resets the handler for this StatusCondition."
        )
        .def(
            "dispatch",
            &PyStatusCondition::dispatch,
            "Dispatches the functions registered with the condition."
        )
        .def_property_readonly(
            "trigger_value",
            &PyStatusCondition::trigger_value,
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
void process_inits<dds::core::cond::StatusCondition>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<PyStatusCondition, PyICondition>(m, "StatusCondition");
        }
    );
}

}
