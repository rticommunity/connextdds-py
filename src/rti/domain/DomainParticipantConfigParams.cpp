#include "PyConnext.hpp"
#include <rti/domain/DomainParticipantConfigParams.hpp>

using namespace rti::domain;

template<>
void pyrti::init_class_defs(py::class_<DomainParticipantConfigParams>& cls) {
    cls
        .def(
            py::init<
                int32_t,
                const std::string&,
                const std::string&,
                const std::string&,
                const std::string&,
                const std::string&
            >(),
            py::arg_v("domain_id", DomainParticipantConfigParams::DOMAIN_ID_USE_XML_CONFIG, "DomainParticipantConfigParams.DOMAIN_ID_USE_XML_CONFIG"),
            py::arg_v("participant_name", DomainParticipantConfigParams::ENTITY_NAME_USE_XML_CONFIG, "DomainParticipantConfigParams.ENTITY_NAME_USE_XML_CONFIG"),
            py::arg_v("qos_library_name", DomainParticipantConfigParams::QOS_ELEMENT_NAME_USE_XML_CONFIG, "DomainParticipantConfigParams.QOS_ELEMENT_NAME_USE_XML_CONFIG"),
            py::arg_v("qos_profile_name", DomainParticipantConfigParams::QOS_ELEMENT_NAME_USE_XML_CONFIG, "DomainParticipantConfigParams.QOS_ELEMENT_NAME_USE_XML_CONFIG"),
            py::arg_v("domain_entity_qos_library_name", DomainParticipantConfigParams::QOS_ELEMENT_NAME_USE_XML_CONFIG, "DomainParticipantConfigParams.QOS_ELEMENT_NAME_USE_XML_CONFIG"),
            py::arg_v("domain_entity_qos_profile_name", DomainParticipantConfigParams::QOS_ELEMENT_NAME_USE_XML_CONFIG, "DomainParticipantConfigParams.QOS_ELEMENT_NAME_USE_XML_CONFIG"),
            "Create a DomainParticipantConfigParams object with the specified values."
        )
        .def_property(
            "domain_id",
            (int32_t (DomainParticipantConfigParams::*)() const) &DomainParticipantConfigParams::domain_id,
            (DomainParticipantConfigParams& (DomainParticipantConfigParams::*)(int32_t)) &DomainParticipantConfigParams::domain_id,
            "The domain id from which the DomainParticipant is created."
        )
        .def_property(
            "participant_name",
            (std::string (DomainParticipantConfigParams::*)() const) &DomainParticipantConfigParams::participant_name,
            (DomainParticipantConfigParams& (DomainParticipantConfigParams::*)(const std::string&)) &DomainParticipantConfigParams::participant_name,
            "The name assigned to the DomainParticipant."
        )
        .def_property(
            "participant_qos_library_name",
            (std::string (DomainParticipantConfigParams::*)() const) &DomainParticipantConfigParams::participant_qos_library_name,
            (DomainParticipantConfigParams& (DomainParticipantConfigParams::*)(const std::string&)) &DomainParticipantConfigParams::participant_qos_library_name,
            "The name of the library containing the DomainParticipant's QoS."
        )
        .def_property(
            "participant_qos_profile_name",
            (std::string (DomainParticipantConfigParams::*)() const) &DomainParticipantConfigParams::participant_qos_profile_name,
            (DomainParticipantConfigParams& (DomainParticipantConfigParams::*)(const std::string&)) &DomainParticipantConfigParams::participant_qos_profile_name,
            "The name of the profile containing the DomainParticipant's QoS."
        )
        .def_property(
            "domain_entity_qos_library_name",
            (std::string (DomainParticipantConfigParams::*)() const) &DomainParticipantConfigParams::domain_entity_qos_library_name,
            (DomainParticipantConfigParams& (DomainParticipantConfigParams::*)(const std::string&)) &DomainParticipantConfigParams::domain_entity_qos_library_name,
            "The QoS library name containing the QoS profile from which the "
            "all the entities defined under the participant configuraton are "
            "created."
        )
        .def_property(
            "domain_entity_qos_profile_name",
            (std::string (DomainParticipantConfigParams::*)() const) &DomainParticipantConfigParams::domain_entity_qos_profile_name,
            (DomainParticipantConfigParams& (DomainParticipantConfigParams::*)(const std::string&)) &DomainParticipantConfigParams::domain_entity_qos_profile_name,
            "The QoS profile name from which the all the entities defined "
            "under the participant configuraton are created."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        )
        .def_property_readonly_static(
            "DOMAIN_ID_USE_XML_CONFIG",
            []() {
                return DomainParticipantConfigParams::DOMAIN_ID_USE_XML_CONFIG;
            },
            "Special value to be used to indicate that a participant should "
            "be created using the domain ID specified in the participant "
            "configuration."
        )
        .def_property_readonly_static(
            "ENTITY_NAME_USE_XML_CONFIG",
            []() {
                return DomainParticipantConfigParams::ENTITY_NAME_USE_XML_CONFIG;
            },
            "Special value to be used to indicate that a participant should "
            "be created with an autogenerated entity name."
        )
        .def_property_readonly_static(
            "QOS_ELEMENT_NAME_USE_XML_CONFIG",
            []() {
                return DomainParticipantConfigParams::QOS_ELEMENT_NAME_USE_XML_CONFIG;
            },
            "Special value to be used to indicate that entities should be "
            "created from the QoS profile specified in the participant "
            "configuration."
        );
}

template<>
void pyrti::process_inits<DomainParticipantConfigParams>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<DomainParticipantConfigParams>(m, "DomainParticipantConfigParams");
        }
    );
}
