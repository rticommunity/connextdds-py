#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

template<>
void pyrti::init_class_defs(py::class_<TransportPriority>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates a policy with priority 0."
        )
        .def(
            py::init<
                int32_t
            >(),
            py::arg("priority"),
            "Creates a policy with the specified priority."
        )
        .def_property(
            "value",
            (int32_t (TransportPriority::*)() const) &TransportPriority::value,
            (TransportPriority& (TransportPriority::*)(int32_t)) &TransportPriority::value,
            "The priority."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

template<>
void pyrti::process_inits<TransportPriority>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<TransportPriority>(m, "TransportPriority");
        }
    );     
}