#include "PyConnext.hpp"
#include "PyEntity.hpp"
#include "PySeq.hpp"
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
