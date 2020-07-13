#include "PyConnext.hpp"
#include <dds/sub/SampleInfo.hpp>

using namespace dds::sub;

namespace pyrti {

template<>
void init_class_defs(py::class_<SampleInfo>& cls)
{
    cls.def_property_readonly(
               "source_timestamp",
               &SampleInfo::source_timestamp,
               "The DataWriter's write timestamp.")
            .def_property_readonly(
                    "state",
                    &SampleInfo::state,
                    "Get the DataState of the sample.")
            .def_property_readonly(
                    "generation_count",
                    &SampleInfo::generation_count,
                    "The GenerationCount of the sample.")
            .def_property_readonly(
                    "rank",
                    &SampleInfo::rank,
                    "Get the Rank of the sample.")
            .def_property_readonly(
                    "valid",
                    &SampleInfo::valid,
                    "Indicates whether the DataSample contains data or else it "
                    "is "
                    "only used to communicate a change in the InstanceState of "
                    "the "
                    "instance.")
            .def_property_readonly(
                    "instance_handle",
                    &SampleInfo::instance_handle,
                    "Identifies locally the corresponding instance.")
            .def_property_readonly(
                    "publication_handle",
                    &SampleInfo::publication_handle,
                    "Identifies locally the DataWriter that modified the "
                    "instance.")
            .def_property_readonly(
                    "reception_timestamp",
                    [](const SampleInfo& s) {
                        return s->reception_timestamp();
                    },
                    "The timestamp when the sample was committed by a "
                    "DataReader.")
            .def_property_readonly(
                    "publication_sequence_number",
                    [](const SampleInfo& s) {
                        return s->publication_sequence_number();
                    },
                    "Publication sequence number assigned when the DDS sample "
                    "was "
                    "written by the DataWriter.")
            .def_property_readonly(
                    "reception_sequence_number",
                    [](const SampleInfo& s) {
                        return s->reception_sequence_number();
                    },
                    "Reception sequence number assigned when the DDS sample "
                    "was "
                    "committed by the DataReader.")
            .def_property_readonly(
                    "original_publication_virtual_guid",
                    [](const SampleInfo& s) {
                        return s->original_publication_virtual_guid();
                    },
                    "Original publication virtual GUID."
                    "\n\n"
                    "If the Publisher’s access_scope is GROUP, this field "
                    "contains "
                    "the Publisher virtual GUID that uniquely identifies the "
                    "DataWriter "
                    "group.")
            .def_property_readonly(
                    "original_publication_virtual_sequence_number",
                    [](const SampleInfo& s) {
                        return s->original_publication_virtual_sequence_number();
                    },
                    "Original publication virtual sequence number."
                    "\n\n"
                    "If the Publisher’s access_scope is GROUP, this field "
                    "contains "
                    "the Publisher virtual sequence number that uniquely "
                    "identifies a "
                    "DDS sample within the DataWriter group.")
            .def_property_readonly(
                    "original_publication_virtual_sample_identity",
                    [](const SampleInfo& s) {
                        return s->original_publication_virtual_sample_identity();
                    },
                    "Retrieves the information provided by "
                    "original_publication_virtual_guid and "
                    "original_publication_virtual_sequence_number "
                    "combined in a SampleIdentity instance.")
            .def_property_readonly(
                    "related_original_publication_virtual_guid",
                    [](const SampleInfo& s) {
                        return s->related_original_publication_virtual_guid();
                    },
                    "The original publication virtual GUID of a related "
                    "sample.")
            .def_property_readonly(
                    "related_original_publication_virtual_sequence_number",
                    [](const SampleInfo& s) {
                        return s->related_original_publication_virtual_sequence_number();
                    },
                    "The original publication virtual sequence number of a "
                    "related "
                    "sample.")
            .def_property_readonly(
                    "related_original_publication_virtual_sample_identity",
                    [](const SampleInfo& s) {
                        return s->related_original_publication_virtual_sample_identity();
                    },
                    "Retrieves the information provided by "
                    "related_original_publication_virtual_guid and "
                    "related_original_publication_virtual_sequence_number "
                    "combined "
                    "in a SampleIdentity instance.")
            .def_property_readonly(
                    "flag",
                    [](const SampleInfo& s) { return s->flag(); },
                    "Flags associated with the sample.")
            .def_property_readonly(
                    "source_guid",
                    [](const SampleInfo& s) { return s->source_guid(); },
                    "The application logical data source associated with the "
                    "sample.")
            .def_property_readonly(
                    "related_source_guid",
                    [](const SampleInfo& s) {
                        return s->related_source_guid();
                    },
                    "The application logical data source that is related to "
                    "the "
                    "sample.")
            .def_property_readonly(
                    "related_subscription_guid",
                    [](const SampleInfo& s) {
                        return s->related_subscription_guid();
                    },
                    "The related_reader_guid associated with the sample.")
            .def_property_readonly(
                    "topic_query_guid",
                    [](const SampleInfo& s) { return s->topic_query_guid(); },
                    "The GUID of the TopicQuery that is related to the sample.")
#if rti_connext_version_gte(6, 0, 0)
            .def_property_readonly(
                    "encapsulation_id",
                    [](const SampleInfo& s) { return s->encapsulation_id(); },
                    "The encapsulation kind.")
#endif
            ;
}

template<>
void process_inits<SampleInfo>(py::module& m, ClassInitList& l)
{
    l.push_back(
            [m]() mutable { return init_class<SampleInfo>(m, "SampleInfo"); });
}

}  // namespace pyrti
