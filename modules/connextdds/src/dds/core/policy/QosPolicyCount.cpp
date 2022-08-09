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
#include "PySeq.hpp"
#include <dds/core/policy/CorePolicy.hpp>
#include "PyQosPolicy.hpp"

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<QosPolicyCount>& cls)
{
    cls.def_property_readonly(
                    "policy",
                    [] (QosPolicyCount& self) {
                        auto m = py::module::import("rti.connextdds");
                        return get_policy_type_from_id(m, self.policy_id());
                    },
                    "The policy class.")
            .def_property_readonly(
                    "count",
                    &QosPolicyCount::count,
                    "The counter.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<QosPolicyCount>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class_with_seq<QosPolicyCount>(m, "QosPolicyCount");
    });
}

}  // namespace pyrti
