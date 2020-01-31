#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace dds::core;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<Batch>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a Batch QoS policy with the default settings (disabled)."
        )
        .def_property(
            "enable",
            (bool (Batch::*)() const) &Batch::enable,
            (Batch& (Batch::*)(bool)) &Batch::enable,
            "Get/set boolean that specifies whether batching is enabled."
        )
        .def_property(
            "max_data_bytes",
            (int32_t (Batch::*)() const) &Batch::max_data_bytes,
            (Batch& (Batch::*)(int32_t)) &Batch::max_data_bytes,
            "Get/set the maximum cummulative size of serialized samples in a "
            "batch. When this limit is reached the batch will be flushed."
        )
        .def_property(
            "max_samples",
            (int32_t (Batch::*)() const) &Batch::max_samples,
            (Batch& (Batch::*)(int32_t)) &Batch::max_samples,
            "Get/set the maximum number of serialized samples in a batch. "
            "When this limit is reached the batch will be flushed."
        )
        .def_property(
            "max_flush_delay",
            (Duration (Batch::*)() const) &Batch::max_flush_delay,
            (Batch& (Batch::*)(const Duration&)) &Batch::max_flush_delay,
            "Get/set the maximum delay to flush a batch, measured from the "
            "time the first sample in the batch is written."
        )
        .def_property(
            "source_timestamp_resolution",
            (Duration (Batch::*)() const) &Batch::source_timestamp_resolution,
            (Batch& (Batch::*)(const Duration&)) &Batch::source_timestamp_resolution,
            "Get/set the timestamp resolution of the samples in the batch."
        )
        .def_property(
            "thread_safe_write",
            (bool (Batch::*)() const) &Batch::thread_safe_write,
            (Batch& (Batch::*)(bool)) &Batch::thread_safe_write,
            "Get/set boolean that specifies whether batch writes must be "
            "thread safe."
        )
        .def_static(
            "enabled",
            &Batch::Enabled,
            "Create a Batch policy with batching enabled."
        )
        .def_static(
            "disabled",
            &Batch::Disabled,
            "Create a Batch policy with batching disabled."
        )
        .def_static(
            "enabled_with_max_data_bytes",
            &Batch::EnabledWithMaxDataBytes,
            py::arg("max_data_bytes"),
            "Create a Batch policy with batching enabled that will flush "
            "based on the cummulative size of serialized samples."
        )
        .def_static(
            "enabled_with_max_samples",
            &Batch::EnabledWithMaxSamples,
            py::arg("max_samples"),
            "Create a Batch policy with batching enabled that will flush "
            "based on the number of samples written to the batch."
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
void process_inits<Batch>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<Batch>(m, "Batch");
        }
    );
}

}
