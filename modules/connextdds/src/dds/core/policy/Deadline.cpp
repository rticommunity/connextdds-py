#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<Deadline>& cls)
{
    cls.def(py::init<>(),
            "Creates the default deadline, with an infinite period.")
            .def(py::init<const dds::core::Duration&>(),
                 py::arg("period"),
                 "Creates a deadline policy with the specified period.")
            .def(py::init([](int32_t sec, int32_t nanosec) {
                     return Deadline(dds::core::Duration(sec, nanosec));
                 }),
                 py::arg("sec"),
                 py::arg("nanosec"),
                 "Creates a deadline policy with the specified period.")
            .def_property(
                    "period",
                    (const dds::core::Duration (Deadline::*)() const)
                            & Deadline::period,
                    (Deadline & (Deadline::*) (const dds::core::Duration&) )
                            & Deadline::period,
                    "The duration of the deadline period.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<Deadline>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable { return init_class<Deadline>(m, "Deadline"); });
}

}  // namespace pyrti
