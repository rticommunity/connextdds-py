/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */

#include "PyConnext.hpp"
#include <dds/core/cond/WaitSet.hpp>
#include <rti/core/cond/WaitSetImpl.hpp>
#include <future>
#include <pybind11/functional.h>
#include "PyCondition.hpp"
#include "PyAsyncioExecutor.hpp"

using namespace dds::core::cond;

namespace pyrti {
class PyTriggeredConditions {
public:
    PyTriggeredConditions(const std::vector<Condition>&& v) : _v(v)
    {
    }

    std::vector<Condition>& v()
    {
        return this->_v;
    }

private:
    std::vector<Condition> _v;
};

class PyTriggeredConditionsIterator {
public:
    PyTriggeredConditionsIterator(PyTriggeredConditions& tc, bool reversed)
            : _tc(tc)
    {
        if (reversed) {
            this->_index = tc.v().size() - 1;
            this->_end = -1;
            this->_step = -1;
        } else {
            this->_index = 0;
            this->_end = tc.v().size();
            this->_step = 1;
        }
    }

    PyCondition next()
    {
        if (this->_index == this->_end)
            throw py::stop_iteration();
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
void init_class_defs(py::class_<PyTriggeredConditionsIterator>& cls)
{
    cls.def("__next__",
            &PyTriggeredConditionsIterator::next,
            "Get next triggered condition.");
}

template<>
void init_class_defs(py::class_<PyTriggeredConditions>& cls)
{
    cls.def("__getitem__",
            [](PyTriggeredConditions& tc, int index) {
                return PyCondition(tc.v()[index]);
            })
            .def("__contains__",
                 [](PyTriggeredConditions& tc, PyICondition& py_c) {
                     auto condition = py_c.get_condition();
                     for (auto c : tc.v()) {
                         if (condition == c)
                             return true;
                     }
                     return false;
                 })
            .def("__len__",
                 [](PyTriggeredConditions& tc) { return tc.v().size(); })
            .def(
                    "__iter__",
                    [](PyTriggeredConditions& tc) {
                        return PyTriggeredConditionsIterator(tc, false);
                    },
                    py::keep_alive<0, 1>())
            .def("__reverse__", [](PyTriggeredConditions& tc) {
                return PyTriggeredConditionsIterator(tc, true);
            });
}


template<>
void init_class_defs(py::class_<WaitSet>& cls)
{
    cls.def(py::init<>(),
            py::call_guard<py::gil_scoped_release>(),
            "Create a WaitSet with no conditions attached.")
            .def(py::init<const rti::core::cond::WaitSetProperty&>(),
                 py::arg("property"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a WaitSet with no conditions attached with the "
                 "specified "
                 "WaitSetProperty settings.")
            .def(
                    "wait",
                    [](WaitSet& ws, const dds::core::Duration& d) {
                        return PyTriggeredConditions(ws.wait(d));
                    },
                    py::arg("timeout"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Wait for conditions attached to this WaitSet to trigger "
                    "with a "
                    "timeout.")
            .def(
                    "wait",
                    [](WaitSet& ws) {
                        return PyTriggeredConditions(ws.wait());
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Wait indefinitely for conditions attached to this WaitSet "
                    "to "
                    "trigger.")
            .def(
                    "wait_async",
                    [](WaitSet& ws,
                       const dds::core::Duration& d) -> py::object {
                        return PyAsyncioExecutor::run<PyTriggeredConditions>(
                                std::function<PyTriggeredConditions()>([&ws,
                                                                        &d]() {
                                    return PyTriggeredConditions(ws.wait(d));
                                }));
                    },
                    py::arg("timeout"),
                    py::keep_alive<0, 1>(),
                    py::keep_alive<0, 2>(),
                    "Wait for conditions attached to this WaitSet to trigger "
                    "with a "
                    "timeout. This call is awaitable and only for use with "
                    "asyncio.")
            .def(
                    "wait_async",
                    [](WaitSet& ws) {
                        return PyAsyncioExecutor::run<PyTriggeredConditions>(
                                std::function<PyTriggeredConditions()>([&ws]() {
                                    return PyTriggeredConditions(ws.wait());
                                }));
                    },
                    py::keep_alive<0, 1>(),
                    "Wait indefinitely for conditions attached to this WaitSet "
                    "to "
                    "trigger. This call is awaitable and only for use with "
                    "asyncio.")
            .def("dispatch",
                 (void (WaitSet::*)(const dds::core::Duration&))
                         & WaitSet::dispatch,
                 py::arg("timeout"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Dispatch handlers for triggered conditions attached to this "
                 "WaitSet with a timeout.")
            .def("dispatch",
                 (void (WaitSet::*)()) & WaitSet::dispatch,
                 py::call_guard<py::gil_scoped_release>(),
                 "Dispatch handlers for triggered conditions attached to this "
                 "WaitSet with no timeout.")
            .def(
                    "dispatch_async",
                    [](WaitSet& ws, const dds::core::Duration& d) {
                        return PyAsyncioExecutor::run<void>(
                                std::function<void()>(
                                        [&ws, &d]() { ws.dispatch(d); }));
                    },
                    py::arg("timeout"),
                    py::keep_alive<0, 1>(),
                    py::keep_alive<0, 2>(),
                    "Dispatch handlers for triggered conditions attached to "
                    "this "
                    "WaitSet with a timeout. This call is awaitable and only "
                    "for "
                    "use with asyncio.")
            .def(
                    "dispatch_async",
                    [](WaitSet& ws) {
                        return PyAsyncioExecutor::run<void>(
                                std::function<void()>(
                                        [&ws]() { ws.dispatch(); }));
                    },
                    py::keep_alive<0, 1>(),
                    "Dispatch handlers for triggered conditions attached to "
                    "this "
                    "WaitSet with no timeout. This call is awaitable and only "
                    "for "
                    "use with asyncio.")
            .def(
                    "__iadd__",
                    [](WaitSet& ws, PyICondition& c) {
                        return ws += c.get_condition();
                    },
                    py::is_operator(),
                    py::call_guard<py::gil_scoped_release>(),
                    "Attach a condition to a WaitSet.")
            .def(
                    "__isub__",
                    [](WaitSet& ws, PyICondition& c) {
                        return ws -= c.get_condition();
                    },
                    py::is_operator(),
                    py::call_guard<py::gil_scoped_release>(),
                    "Detach a condition from a WaitSet.")
            .def(
                    "attach_condition",
                    [](WaitSet& ws, PyICondition& c) {
                        ws.attach_condition(c.get_condition());
                    },
                    py::arg("condition"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Attach a condition to the WaitSet.")
            .def(
                    "detach_condition",
                    [](WaitSet& ws, PyICondition& c) {
                        return ws.detach_condition(c.get_condition());
                    },
                    py::arg("condition"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Detach a condition from the WaitSet.")
            .def_property(
                    "conditions",
                    [](WaitSet& ws) {
                        py::gil_scoped_release release;
                        std::vector<PyCondition> conditions;
                        for (auto& c : ws.conditions()) {
                            conditions.push_back(PyCondition(c));
                        }
                        return conditions;
                    },
                    [](WaitSet& ws, std::vector<PyICondition*> conditions) {
                        py::gil_scoped_release release;
                        for (auto& c : conditions) {
                            ws += c->get_condition();
                        }
                    },
                    "Get/set the attached conditions for the WaitSet.")
            .def(
                    "detach_all",
                    [](WaitSet& ws) { ws->detach_all(); },
                    py::call_guard<py::gil_scoped_release>(),
                    "Detach all conditions from the WaitSet.")
            .def_property(
                    "property",
                    [](WaitSet& ws) {
                        py::gil_scoped_release release;
                        return ws->property(); 
                    },
                    [](WaitSet& ws, rti::core::cond::WaitSetProperty& p) {
                        py::gil_scoped_release release;
                        ws->property(p);
                    },
                    "Get/set the WaitSetProperty settings for the WaitSet.");
}

template<>
void process_inits<WaitSet>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable { return init_class<WaitSet>(m, "WaitSet"); });

    l.push_back([m]() mutable {
        return init_class<PyTriggeredConditions>(m, "TriggeredConditions");
    });

    l.push_back([m]() mutable {
        return init_class<PyTriggeredConditionsIterator>(
                m,
                "TriggeredConditionsIterator");
    });
}

}  // namespace pyrti
