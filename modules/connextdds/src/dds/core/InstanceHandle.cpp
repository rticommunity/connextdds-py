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

#include <sstream>
#include "PyConnext.hpp"
#include "PySeq.hpp"
#include "PyEntity.hpp"
#include <dds/core/InstanceHandle.hpp>

using namespace dds::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<InstanceHandle>& cls)
{
    cls.def(py::init([]() { return InstanceHandle::nil(); }),
            "Create a nil InstanceHandle")
            .def_property_readonly(
                    "is_nil",
                    &dds::core::InstanceHandle::is_nil,
                    "Nil status of InstanceStatus")
            .def("__bool__",
                 [](const InstanceHandle& h) { return !h.is_nil(); })
            .def("__nonzero__",
                 [](const InstanceHandle& h) { return !h.is_nil(); })
            .def("__str__",
                 [](const InstanceHandle& h) {
                     std::ostringstream oss;
                     oss << h;
                     return oss.str();
                 })
            .def("__hash__",
                [](const InstanceHandle& key) {
                    return std::hash<InstanceHandle>()(key);
                })
            .def_static(
                    "nil",
                    &InstanceHandle::nil,
                    "Create a nil InstanceHandle.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<InstanceHandle>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class_with_seq<InstanceHandle>(m, "InstanceHandle");
    });
}

}  // namespace pyrti
