#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<SystemResourceLimits>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates a SystemResourceLimits qos policy with default values."
        )
        .def(
            py::init<
                int32_t
            >(),
            py::arg("max_objects_per_thread"),
            "Creates a SystemResourceLimits qos policy with the provided "
            "max_objects_per_thread."
        )
        .def_property(
            "max_objects_per_thread",
            (int32_t (SystemResourceLimits::*)() const) &SystemResourceLimits::max_objects_per_thread,
            (SystemResourceLimits& (SystemResourceLimits::*)(int32_t)) &SystemResourceLimits::max_objects_per_thread,
            "The maximum number of objects that can be stored per thread."
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
void process_inits<SystemResourceLimits>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<SystemResourceLimits>(m, "SystemResourceLimits");
        }
    );
}

}
