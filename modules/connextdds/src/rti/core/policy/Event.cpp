#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<Event>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<const rti::core::ThreadSettings&, int32_t, int32_t>(),
                 py::arg("thread"),
                 py::arg("initial_count"),
                 py::arg("max_count"),
                 "Creates an instance with all the parameters set.")
            .def_property(
                    "thread",
                    (rti::core::ThreadSettings & (Event::*) ()) & Event::thread,
                    (Event & (Event::*) (const rti::core::ThreadSettings&) )
                            & Event::thread,
                    "Event thread QoS.")
            .def_property(
                    "initial_count",
                    (int32_t(Event::*)() const) & Event::initial_count,
                    (Event & (Event::*) (int32_t)) & Event::initial_count,
                    "Event thread QoS.")
            .def_property(
                    "max_count",
                    (int32_t(Event::*)() const) & Event::max_count,
                    (Event & (Event::*) (int32_t)) & Event::max_count,
                    "Event thread QoS.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<Event>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable { return init_class<Event>(m, "Event"); });
}

}  // namespace pyrti
