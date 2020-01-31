#include "PyConnext.hpp"
#include <dds/core/cond/WaitSet.hpp>
#include <rti/core/cond/WaitSetImpl.hpp>
#include "PyCondition.hpp"

using namespace dds::core::cond;

namespace pyrti {
class PyTriggeredConditions {
public:
    PyTriggeredConditions(const std::vector<Condition>&& v) : _v(v) {}

std::vector<Condition>& v() { return this->_v; }

private:
    std::vector<Condition> _v;
};

class PyTriggeredConditionsIterator {
public:
    PyTriggeredConditionsIterator(PyTriggeredConditions& tc, bool reversed) : _tc(tc) {
        if (reversed) {
            this->_index = tc.v().size() - 1;
            this->_end = -1;
            this->_step = -1;
        }
        else {
            this->_index = 0;
            this->_end = tc.v().size();
            this->_step = 1;
        }
    }

    PyCondition next() {
        if (this->_index == this->_end) throw py::stop_iteration();
        auto retval = PyCondition(this->_tc.v()[this->_index]);
        this->_index += this->_step;
        return retval;
    }

private:
    PyTriggeredConditions& _tc;
    int32_t _index;
    int32_t _step;
    int32_t _end;
};


template<>
void init_class_defs(py::class_<PyTriggeredConditionsIterator>& cls) {
    cls
        .def(
            "__next__",
            &PyTriggeredConditionsIterator::next,
            "Get next triggered condition."
        );
}

template<>
void init_class_defs(py::class_<PyTriggeredConditions>& cls) {
    cls
        .def(
            "__getitem__",
            [](PyTriggeredConditions& tc, int index) {
                return PyCondition(tc.v()[index]);
            }
        )
        .def(
            "__contains__",
            [](PyTriggeredConditions& tc, PyICondition& py_c) {
                auto condition = py_c.get_condition();
                for (auto c : tc.v()) {
                    if (condition == c) return true;
                }
                return false;
            }
        )
        .def(
            "__len__",
            [](PyTriggeredConditions& tc) {
                return tc.v().size();
            }
        )
        .def(
            "__iter__",
            [](PyTriggeredConditions& tc) {
                return PyTriggeredConditionsIterator(tc, false);
            },
            py::keep_alive<0, 1>()
        )
        .def(
            "__reverse__",
            [](PyTriggeredConditions& tc) {
                return PyTriggeredConditionsIterator(tc, true);
            }
        );
}

template<>
void init_class_defs(py::class_<WaitSet>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a WaitSet with no conditions attached."
        )
        .def(
            py::init<const rti::core::cond::WaitSetProperty&>(),
            py::arg("property"),
            "Create a WaitSet with no conditions attached with the specified WaitSetProperty settings."
        )
        .def(
            "wait",
            [](WaitSet& ws, const dds::core::Duration& d) {
                return PyTriggeredConditions(ws.wait(d));
                /*auto active_conditions = ws.wait(d);
                std::vector<PyCondition> conditions;
                for (auto& c : active_conditions) {
                    conditions.push_back(PyCondition(c));
                }
                return conditions; */
            },
            py::arg("timeout"),
            py::call_guard<py::gil_scoped_release>(),
            "Wait for conditions attached to this WaitSet to trigger with a timeout."
        )
        .def(
            "wait",
            [](WaitSet& ws) {
                return PyTriggeredConditions(ws.wait());
                /*auto active_conditions = ws.wait();
                std::vector<PyCondition> conditions;
                for (auto& c : active_conditions) {
                    conditions.push_back(PyCondition(c));
                }
                return conditions;*/
            },
            py::call_guard<py::gil_scoped_release>(),
            "Wait indefinitely for conditions attached to this WaitSet to trigger."
        )
        .def(
            "dispatch",
            (void (WaitSet::*)(const dds::core::Duration&)) &WaitSet::dispatch,
            py::arg("timeout"),
            py::call_guard<py::gil_scoped_release>(),
            "Dispatch handlers for triggered conditions attached to this WaitSet with a timeout."
        )
        .def(
            "dispatch",
            (void (WaitSet::*)()) &WaitSet::dispatch,
            py::call_guard<py::gil_scoped_release>(),
            "Dispatch handlers for triggered conditions attached to this WaitSet with no timeout."
        )
        .def(
            "__iadd__",
            [](WaitSet& ws, PyICondition& c) {
                return ws += c.get_condition();
            },
            py::is_operator(),
            "Attach a condition to a WaitSet."
        )
        .def(
            "__isub__",
            [](WaitSet& ws, PyICondition& c) {
            return  ws -= c.get_condition();
            },
            py::is_operator(),
            "Detach a condition from a WaitSet."
        )
        .def(
            "attach_condition",
            [](WaitSet& ws, PyICondition& c) {
                ws.attach_condition(c.get_condition());
            },
            py::arg("condition"),
            "Attach a condition to the WaitSet."
        )
        .def(
            "detach_condition",
            [](WaitSet& ws, PyICondition& c) {
                return ws.detach_condition(c.get_condition());
            },
            py::arg("condition"),
            "Detach a condition from the WaitSet."
        )
        .def_property(
            "conditions",
            [](WaitSet& ws) {
                std::vector<PyCondition> conditions;
                for (auto& c : ws.conditions()) {
                    conditions.push_back(PyCondition(c));
                }
                return conditions;
            },
            [](WaitSet& ws, std::vector<PyICondition*> conditions) {
                for (auto& c : conditions) {
                    ws += c->get_condition();
                }
            },
            "Get/set the attached conditions for the WaitSet."
        )
        .def(
            "detach_all",
            [](WaitSet& ws) {
                ws->detach_all();
            },
            "Detach all conditions from the WaitSet."
        )
        .def_property(
            "property",
            [](WaitSet& ws) {
                return ws->property();
            },
            [](WaitSet& ws, rti::core::cond::WaitSetProperty& p) {
                ws->property(p);
            },
            "Get/set the WaitSetProperty settings for the WaitSet."
        );
}

template<>
void process_inits<WaitSet>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<WaitSet>(m, "WaitSet");
        }
    );

    l.push_back(
        [m]() mutable {
            return init_class<PyTriggeredConditions>(m, "TriggeredConditions");
        }
    );

    l.push_back(
        [m]() mutable {
            return init_class<PyTriggeredConditionsIterator>(m, "TriggeredConditionsIterator");
        }
    );
}

}
