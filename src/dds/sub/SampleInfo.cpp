#include "PyConnext.hpp"
#include <dds/sub/SampleInfo.hpp>

using namespace dds::sub;

template<>
void pyrti::init_class_defs(py::class_<SampleInfo>& cls) {
    cls
        .def_property_readonly(
            "source_timestamp",
            &SampleInfo::source_timestamp,
            "The DataWriter's write timestamp."
        )
        .def_property_readonly(
            "state",
            &SampleInfo::state,
            "Get the DataState of the sample."
        )
        .def_property_readonly(
            "generation_count",
            &SampleInfo::generation_count,
            "The GenerationCount of the sample."
        )
        .def_property_readonly(
            "rank",
            &SampleInfo::rank,
            "Get the Rank of the sample."
        )
        .def_property_readonly(
            "valid",
            &SampleInfo::valid,
            "Indicates whether the DataSample contains data or else it is "
            "only used to communicate a change in the InstanceState of the "
            "instance."
        )
        .def_property_readonly(
            "instance_handle",
            &SampleInfo::instance_handle,
            "Identifies locally the corresponding instance."
        )
        .def_property_readonly(
            "publication_handle",
            &SampleInfo::publication_handle,
            "Identifies locally the DataWriter that modified the instance."
        )
        .def_property_readonly(
            "reception_timestamp",
            [](const SampleInfo& s) {
                return s->reception_timestamp();
            },
            "The timestamp when the sample was committed by a DataReader."
        )
        .def_property_readonly(
            "publication_sequence_number",
            [](const SampleInfo& s) {
                return s->publication_sequence_number();
            }
        )
        .def_property_readonly(
            "reception_sequence_number",
            [](const SampleInfo& s) {
                return s->reception_sequence_number();
            }
        )
        .def_property_readonly(
            "original_publication_virtual_guid",
            [](const SampleInfo& s) {
                return s->original_publication_virtual_guid();
            }
        )
        .def_property_readonly(
            "original_publication_virtual_sequence_number",
            [](const SampleInfo& s) {
                return s->original_publication_virtual_sequence_number();
            }
        )
        .def_property_readonly(
            "original_publication_virtual_sample_identity",
            [](const SampleInfo& s) {
                return s->original_publication_virtual_sample_identity();
            }
        )
        .def_property_readonly(
            "related_original_publication_virtual_guid",
            [](const SampleInfo& s) {
                return s->related_original_publication_virtual_guid();
            }
        )
        .def_property_readonly(
            "related_original_publication_virtual_sequence_number",
            [](const SampleInfo& s) {
                return s->related_original_publication_virtual_sequence_number();
            }
        )
        .def_property_readonly(
            "related_original_publication_virtual_sample_identity",
            [](const SampleInfo& s) {
                return s->related_original_publication_virtual_sample_identity();
            }
        )
        .def_property_readonly(
            "flag",
            [](const SampleInfo& s) {
                return s->flag();
            }
        )
        .def_property_readonly(
            "source_guid",
            [](const SampleInfo& s) {
                return s->source_guid();
            }
        )
        .def_property_readonly(
            "related_source_guid",
            [](const SampleInfo& s) {
                return s->related_source_guid();
            }
        )
        .def_property_readonly(
            "related_subscription_guid",
            [](const SampleInfo& s) {
                return s->related_subscription_guid();
            }
        )
        .def_property_readonly(
            "topic_query_guid",
            [](const SampleInfo& s) {
                return s->topic_query_guid();
            }
        )
        .def_property_readonly(
            "encapsulation_id",
            [](const SampleInfo& s) {
                return s->encapsulation_id();
            }
        );
}

template<>
void pyrti::process_inits<SampleInfo>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable{
            return pyrti::init_class<SampleInfo>(m, "SampleInfo");
        }
    );
}