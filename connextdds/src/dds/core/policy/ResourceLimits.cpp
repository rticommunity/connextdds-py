#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>

using namespace dds::core::policy;

template<>
void pyrti::init_class_defs(py::class_<ResourceLimits>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates the default policy."
        )
        .def(
            py::init<
                int32_t,
                int32_t,
                int32_t
            >(),
            py::arg("max_samples"),
            py::arg("max_instances"),
            py::arg("max_samples_per_instance"),
            "Creates the default policy."
        )
        .def_property(
            "max_samples",
            (int32_t (ResourceLimits::*)() const) &ResourceLimits::max_samples,
            (ResourceLimits& (ResourceLimits::*)(int32_t)) &ResourceLimits::max_samples,
            "Sets the maximum number of data samples that a DataWriter or a "
            "DataReader can manage across all instances."
        )
        .def_property(
            "max_instances",
            (int32_t (ResourceLimits::*)() const) &ResourceLimits::max_instances,
            (ResourceLimits& (ResourceLimits::*)(int32_t)) &ResourceLimits::max_instances,
            "Sets the maximum number of instances that a DataWriter or a "
            "DataReader can manage."
        )
        .def_property(
            "max_samples_per_instance",
            (int32_t (ResourceLimits::*)() const) &ResourceLimits::max_samples_per_instance,
            (ResourceLimits& (ResourceLimits::*)(int32_t)) &ResourceLimits::max_samples_per_instance,
            "Sets the maximum number of data samples per instance that a "
            "DataWriter or a DataReader can manage."
        )
        .def_property(
            "initial_samples",
            [](const ResourceLimits& rl) {
                return rl->initial_samples();
            },
            [](ResourceLimits& rl, int32_t val) {
                return rl->initial_samples(val);
            },
            "The maximum number of data samples per instance that a "
            "DataWriter or a DataReader can manage."
        )
        .def_property(
            "initial_instances",
            [](const ResourceLimits& rl) {
                return rl->initial_instances();
            },
            [](ResourceLimits& rl, int32_t val) {
                return rl->initial_instances(val);
            },
            "The number of instances that a DataWriter or a DataWriter will "
            "preallocate."
        )
        .def_property(
            "instance_hash_buckets",
            [](const ResourceLimits& rl) {
                return rl->instance_hash_buckets();
            },
            [](ResourceLimits& rl, int32_t val) {
                return rl->instance_hash_buckets(val);
            },
            "The number of instances that a DataWriter or a DataWriter will "
            "preallocate."
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
void pyrti::process_inits<ResourceLimits>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<ResourceLimits>(m, "ResourceLimits");
        }
    );
}