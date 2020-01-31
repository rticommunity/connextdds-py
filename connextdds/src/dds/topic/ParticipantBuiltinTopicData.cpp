#include "PyConnext.hpp"
#include <dds/topic/BuiltinTopic.hpp>
#include "PyInitType.hpp"
#include "PyInitOpaqueTypeContainers.hpp"

namespace py = pybind11;

using namespace dds::topic;

INIT_OPAQUE_TYPE_CONTAINERS(dds::topic::ParticipantBuiltinTopicData);

namespace pyrti {

template<>
void init_dds_typed_topic_template(py::class_<PyTopic<dds::topic::ParticipantBuiltinTopicData>, PyITopicDescription<dds::topic::ParticipantBuiltinTopicData>, PyIAnyTopic>& cls) {
    init_dds_typed_topic_base_template(cls);
}

template<>
void init_class_defs(py::class_<ParticipantBuiltinTopicData>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a default ParticipantBuiltinTopicData."
        )
        .def_property_readonly(
            "key",
            &ParticipantBuiltinTopicData::key,
            "Get the DCPS key to distinguish entries."
        )
        .def_property_readonly(
            "user_data",
            &ParticipantBuiltinTopicData::user_data,
            "Get the UserData policy of the corresponding DomainParticipant."
        )
        .def_property_readonly(
            "property",
            [](const ParticipantBuiltinTopicData& p) {
                return p->property();
            },
            "Get the UserData policy of the corresponding DomainParticipant."
        )
        .def_property_readonly(
            "rtps_protocol_version",
            [](const ParticipantBuiltinTopicData& p) {
                return p->rtps_protocol_version();
            },
            "Get the version number of the RTPS wire protocol used."
        )
        .def_property_readonly(
            "rtps_vendor_id",
            [](const ParticipantBuiltinTopicData& p) {
                return p->rtps_vendor_id();
            },
            "Get the ID of the vendor implementing the RTPS wire protocol."
        )
        .def_property_readonly(
            "dds_builtin_endpoints",
            [](const ParticipantBuiltinTopicData& p) {
                return p->dds_builtin_endpoints();
            },
            "Bitmap of builtin endpoints supported by the participant."
        )
        .def_property_readonly(
            "default_unicast_locators",
            [](const ParticipantBuiltinTopicData& p) {
                return p->default_unicast_locators();
            },
            "Get the unicast locators used when individual entities do not "
            "specify unicast locators."
        )
        .def_property_readonly(
            "product_version",
            [](const ParticipantBuiltinTopicData& p) {
                return p->product_version();
            },
            "Get the current version for RTI Connext."
        )
        .def_property_readonly(
            "participant_name",
            [](const ParticipantBuiltinTopicData& p) {
                return p->participant_name();
            },
            "Get the participant name and role name."
        )
        .def_property_readonly(
            "domain_id",
            [](const ParticipantBuiltinTopicData& p) {
                return p->domain_id();
            },
            "Get the domain ID associated with the discovered "
            "DomainParticipant."
        )
        .def_property_readonly(
            "transport_info",
            [](const ParticipantBuiltinTopicData& p) {
                return p->transport_info();
            },
            "Get the sequence of TransportInfo containing information about "
            "each of the installed transports of the discovered "
            "DomainParticipant."
        )
        .def_property_readonly(
            "reachability_lease_duration",
            [](const ParticipantBuiltinTopicData& p) {
                return p->reachability_lease_duration();
            },
            "The reachability lease duration."
        )
        .def_property_readonly(
            "vendor_builtin_endpoints",
            [](const ParticipantBuiltinTopicData& p) {
                return p->vendor_builtin_endpoints();
            },
            "Bitmap of vendor-specific builtin endpoints supported by the "
            "participant."
        )
        .def_property_readonly_static(
            "topic_name",
            [](py::object&) {
                return dds::topic::participant_topic_name();
            },
            "Participant builtin topic name."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );

    init_type<ParticipantBuiltinTopicData>(cls);
}

template<>
void process_inits<ParticipantBuiltinTopicData>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<ParticipantBuiltinTopicData>(m, "ParticipantBuiltinTopicData");
        }
    );
}

}
