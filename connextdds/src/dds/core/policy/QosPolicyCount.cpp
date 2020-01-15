#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

template<>
void pyrti::init_class_defs(py::class_<QosPolicyCount>& cls)
{
    cls
        .def(
            py::init<
                uint32_t, 
                int32_t
            >(),
            py::arg("policy_id"),
            py::arg("count"),
            "Creates an instance with the policy ID and its counter."
        )
        .def_property_readonly(
            "policy_id",
            &QosPolicyCount::policy_id,
            "The policy ID."
        )
        .def_property_readonly(
            "count",
            &QosPolicyCount::count,
            "The counter."
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
void pyrti::process_inits<QosPolicyCount>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<QosPolicyCount>(m, "QosPolicyCount");
        }
    );
}