#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

template<>
void pyrti::init_class_defs(py::class_<BuiltinTopicReaderResourceLimits>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a policy with default settings."
        )
        .def_property(
            "initial_samples",
            (int32_t (BuiltinTopicReaderResourceLimits::*)() const) &BuiltinTopicReaderResourceLimits::initial_samples,
            (BuiltinTopicReaderResourceLimits& (BuiltinTopicReaderResourceLimits::*)(int32_t)) &BuiltinTopicReaderResourceLimits::initial_samples,
            "Initial number of samples"
        )
        .def_property(
            "max_samples",
            (int32_t (BuiltinTopicReaderResourceLimits::*)() const) &BuiltinTopicReaderResourceLimits::max_samples,
            (BuiltinTopicReaderResourceLimits& (BuiltinTopicReaderResourceLimits::*)(int32_t)) &BuiltinTopicReaderResourceLimits::max_samples,
            "Maximum number of samples"
        )
        .def_property(
            "initial_infos",
            (int32_t (BuiltinTopicReaderResourceLimits::*)() const) &BuiltinTopicReaderResourceLimits::initial_infos,
            (BuiltinTopicReaderResourceLimits& (BuiltinTopicReaderResourceLimits::*)(int32_t)) &BuiltinTopicReaderResourceLimits::initial_infos,
            "Initial number of sample infos."
        )
        .def_property(
            "max_infos",
            (int32_t (BuiltinTopicReaderResourceLimits::*)() const) &BuiltinTopicReaderResourceLimits::max_infos,
            (BuiltinTopicReaderResourceLimits& (BuiltinTopicReaderResourceLimits::*)(int32_t)) &BuiltinTopicReaderResourceLimits::max_infos,
            "Maximum number of sample infos."
        )
        .def_property(
            "initial_outstanding_reads",
            (int32_t (BuiltinTopicReaderResourceLimits::*)() const) &BuiltinTopicReaderResourceLimits::initial_outstanding_reads,
            (BuiltinTopicReaderResourceLimits& (BuiltinTopicReaderResourceLimits::*)(int32_t)) &BuiltinTopicReaderResourceLimits::initial_outstanding_reads,
            "The initial number of outstanding calls to read/take (or one of "
            "their variants) on the same DataReader for which memory has not "
            "been returned by calling LoanedSamples.return_loan()."
        )
        .def_property(
            "max_outstanding_reads",
            (int32_t (BuiltinTopicReaderResourceLimits::*)() const) &BuiltinTopicReaderResourceLimits::max_outstanding_reads,
            (BuiltinTopicReaderResourceLimits& (BuiltinTopicReaderResourceLimits::*)(int32_t)) &BuiltinTopicReaderResourceLimits::max_outstanding_reads,
            "The max number of outstanding calls to read/take (or one of "
            "their variants) on the same DataReader for which memory has not "
            "been returned by calling LoanedSamples.return_loan()."
        )
        .def_property(
            "max_samples_per_read",
            (int32_t (BuiltinTopicReaderResourceLimits::*)() const) &BuiltinTopicReaderResourceLimits::max_samples_per_read,
            (BuiltinTopicReaderResourceLimits& (BuiltinTopicReaderResourceLimits::*)(int32_t)) &BuiltinTopicReaderResourceLimits::max_samples_per_read,
            "The maximum number of data samples that the application can "
            "receive from the middleware in a single call to "
            "DataReader.read() or DataReader.take(). If more data exists in "
            "the middleware, the application will need to issue multiple "
            "read/take calls."
        )
        .def_property(
            "disable_fragmentation_support",
            (bool (BuiltinTopicReaderResourceLimits::*)() const) &BuiltinTopicReaderResourceLimits::disable_fragmentation_support,
            (BuiltinTopicReaderResourceLimits& (BuiltinTopicReaderResourceLimits::*)(bool)) &BuiltinTopicReaderResourceLimits::disable_fragmentation_support,
            "Determines whether the DataReader can receive fragmented samples."
        )
        .def_property(
            "max_fragmented_samples",
            (int32_t (BuiltinTopicReaderResourceLimits::*)() const) &BuiltinTopicReaderResourceLimits::max_fragmented_samples,
            (BuiltinTopicReaderResourceLimits& (BuiltinTopicReaderResourceLimits::*)(int32_t)) &BuiltinTopicReaderResourceLimits::max_fragmented_samples,
            "The maximum number of samples for which the DataReader may "
            "store fragments at a given point in time."
        )
        .def_property(
            "initial_fragmented_samples",
            (int32_t (BuiltinTopicReaderResourceLimits::*)() const) &BuiltinTopicReaderResourceLimits::initial_fragmented_samples,
            (BuiltinTopicReaderResourceLimits& (BuiltinTopicReaderResourceLimits::*)(int32_t)) &BuiltinTopicReaderResourceLimits::initial_fragmented_samples,
            "The initial number of samples for which a DataReader may store "
            "fragments."
        )
        .def_property(
            "max_fragments_per_sample",
            (int32_t (BuiltinTopicReaderResourceLimits::*)() const) &BuiltinTopicReaderResourceLimits::max_fragments_per_sample,
            (BuiltinTopicReaderResourceLimits& (BuiltinTopicReaderResourceLimits::*)(int32_t)) &BuiltinTopicReaderResourceLimits::max_fragments_per_sample,
            "Maximum number of fragments for a single sample."
        )
        .def_property(
            "dynamically_allocate_fragmented_samples",
            (bool (BuiltinTopicReaderResourceLimits::*)() const) &BuiltinTopicReaderResourceLimits::dynamically_allocate_fragmented_samples,
            (BuiltinTopicReaderResourceLimits& (BuiltinTopicReaderResourceLimits::*)(bool)) &BuiltinTopicReaderResourceLimits::dynamically_allocate_fragmented_samples,
            "Determines whether the DataReader pre-allocates storage for "
            "storing fragmented samples."
        )
        .def(
            py::self == py::self
        )
        .def(
            py::self != py::self
        );
}

template<>
void pyrti::process_inits<BuiltinTopicReaderResourceLimits>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<BuiltinTopicReaderResourceLimits>(m, "BuiltinTopicReaderResourceLimits");
        }
    );
}
