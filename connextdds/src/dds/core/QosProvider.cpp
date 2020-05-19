#include "PyConnext.hpp"
#include <dds/core/QosProvider.hpp>
#include "PyDynamicTypeMap.hpp"

using namespace dds::core;
using namespace dds::domain::qos;
using namespace dds::topic::qos;
using namespace dds::pub::qos;
using namespace dds::sub::qos;

namespace pyrti {

template<>
void init_class_defs(py::class_<QosProvider>& cls) {
    cls
        .def(
            py::init<const std::string&, const std::string&>(),
            py::arg("uri"),
            py::arg("profile"),
            "Creates a QosProvider fetching QoS configuration from the specified URI."
        )
        .def(
            py::init<const std::string>(),
            py::arg("uri"),
            "Create a QosProvider fetching QoS configuration from the specified URI."
        )
        .def_property_readonly(
            "participant_qos",
            (DomainParticipantQos (QosProvider::*)()) &QosProvider::participant_qos,
            "Get a copy of the DomainParticipantQos currently associated with the QosProvider."
        )
        .def(
            "participant_qos_profile",
            (DomainParticipantQos (QosProvider::*)(const std::string&)) &QosProvider::participant_qos,
            py::arg("profile_name"),
            "Set the profile for the DomainParticipantQos associated with this QosProvider."
        )
        .def_property_readonly(
            "topic_qos",
            (TopicQos (QosProvider::*)()) &QosProvider::topic_qos,
            "Get a copy of the TopicQos currently associated with the QosProvider."
        )
        .def(
            "topic_qos_profile",
            (TopicQos (QosProvider::*)(const std::string&)) &QosProvider::topic_qos,
            py::arg("profile_name"),
            "Set the profile for the TopicQos associated with this QosProvider."
        )
        .def_property_readonly(
            "subscriber_qos",
            (SubscriberQos (QosProvider::*)()) &QosProvider::subscriber_qos,
            "Get a copy of the SubscriberQos currently associated with this QosProvider."
        )
        .def(
            "subscriber_qos_profile",
            (SubscriberQos (QosProvider::*)(const std::string&)) &QosProvider::subscriber_qos,
            py::arg("profile"),
            "Set the profile for the SubscriberQos associated with the QosProvider."
        )
        .def_property_readonly(
            "datareader_qos",
            (DataReaderQos (QosProvider::*)()) &QosProvider::datareader_qos,
            "Get a copy of the DataReaderQos currently associated with the QosProvider."
        )
        .def(
            "datareader_qos_profile",
            (DataReaderQos (QosProvider::*)(const std::string&)) &QosProvider::datareader_qos,
            py::arg("profile"),
            "Set the profile for the DataReaderQos associated with this QosProvider."
        )
        .def_property_readonly(
            "publisher_qos",
            (PublisherQos (QosProvider::*)()) &QosProvider::publisher_qos,
            "Get a copy of the PublisherQos currently associated with the QosProvider."
        )
        .def(
            "publisher_qos_profile",
            (PublisherQos (QosProvider::*)(const std::string&)) &QosProvider::publisher_qos,
            py::arg("profile"),
            "Set the profile for the PublisherQos associated with this QosProvider."
        )
        .def_property_readonly(
            "datawriter_qos",
            (DataWriterQos (QosProvider::*)()) &QosProvider::datawriter_qos,
            "Get a copy of the DataWriterQos currently associated with the QosProvider."
        )
        .def(
            "datawriter_qos_profile",
            (DataWriterQos (QosProvider::*)(const std::string&)) & QosProvider::datawriter_qos,
            py::arg("profile"),
            "Set the profile for the DataWriterQos associated with this QosProvider."
        )
        .def_property_static(
            "default_provider_params",
            [](QosProvider& qp) {
                return qp->default_provider_params();
            },
            [](QosProvider& qp, const rti::core::QosProviderParams& params) {
                qp->default_provider_params(params);
            },
            "Get a copy of or set the default QosProviderParams."
        )
        .def(
            "topic_name_qos",
            [](QosProvider& qp, const std::string& profile, const std::string& topic) {
                return qp->topic_qos_w_topic_name(profile, topic);
            },
            py::arg("profile_name"),
            py::arg("topic_name"),
            "Set the TopicQos for a given Topic name."
        )
        .def(
            "topic_name_qos",
            [](QosProvider& qp, const std::string& topic) {
                return qp->topic_qos_w_topic_name(topic);
            },
            py::arg("topic_name"),
            "Get the TopicQos associated with a given Topic name."
        )
        .def(
            "topic_datareader_qos",
            [](QosProvider& qp, const std::string& profile, const std::string& topic) {
                return qp->datareader_qos_w_topic_name(profile, topic);
            },
            py::arg("profile_name"),
            py::arg("topic_name"),
            "Set the DataReaderQos for a given Topic name."
        )
        .def(
            "topic_datareader_qos",
            [](QosProvider& qp, const std::string& topic) {
                return qp->datareader_qos_w_topic_name(topic);
            },
            py::arg("topic_name"),
            "Get the DataReaderQos associated with a given Topic name."
        )
        .def(
            "topic_datawriter_qos",
            [](QosProvider& qp, const std::string& profile, const std::string& topic) {
                return qp->datawriter_qos_w_topic_name(profile, topic);
            },
            py::arg("profile_name"),
            py::arg("topic_name"),
            "Set the DataWriterQos for a given Topic name."
        )
        .def(
            "topic_datawriter_qos",
            [](QosProvider& qp, const std::string& topic) {
                return qp->datawriter_qos_w_topic_name(topic);
            },
            py::arg("topic_name"),
            "Get the DataWriterQos associated with a given Topic name."
        )
        .def_property(
            "default_library",
            [](QosProvider& qp) {
                rti::core::optional_value<std::string> dl = qp->default_library();
                if (dl.is_set()) return py::cast(dl.get());
                else return py::cast(nullptr);
            },
            [](QosProvider& qp, const std::string& library) {
                qp->default_library(library);
            },
            "The default library associated with this QosProvider (None if not set)."
            )
        .def_property(
            "default_profile",
            [](QosProvider& qp) {
                rti::core::optional_value<std::string> dp = qp->default_profile();
                if (dp.is_set()) return py::cast(dp.get());
                else return py::cast(nullptr);
            },
            [](QosProvider& qp, const std::string& profile) {
                qp->default_library(profile);
            },
            "The default profile associated with this QosProvider (None if not set)."
            )
        .def_property(
            "default_library",
            [](QosProvider& qp) {
                rti::core::optional_value<std::string> dl = qp->default_library();
                if (dl.is_set()) return py::cast(dl.get());
                else return py::cast(nullptr);
            },
            [](QosProvider& qp, const std::string& library) {
                qp->default_library(library);
            },
            "The default library associated with this QosProvider (None if not set)."
            )
        .def_property_readonly(
            "default_library_profile",
            [](const QosProvider& qp) {
                rti::core::optional_value<std::string> dpl = qp->default_profile_library();
                if (dpl.is_set()) return py::cast(dpl.get());
                else return py::cast(nullptr);
            },
            "The library of the default profile associated with this QosProvider (None if not set)."
            )
        .def(
            "qos_profile_libraries",
            [](const QosProvider& qp) {
                return qp->qos_profile_libraries();
            },
            "Get a list of the QoS profile libraries loaded by the QosProvider."
        )
        .def(
            "qos_profiles",
            [](const QosProvider& qp, const std::string& lib) {
                return qp->qos_profiles(lib);
            },
            py::arg("library"),
            "Get a list of the QoS profiles loaded the specified library of the QosProvider."
        )
    
        .def_property_readonly(
            "profiles_loaded",
            [](const QosProvider& qp) {
                return qp->profiles_loaded();
            },
            "Check if the profiles from this QosProvider have been loaded."
        )
        .def(
            "type",
            [](const QosProvider& qp, const std::string& type_lib_name, const std::string& type_name) {
                auto dt = qp->type(type_lib_name, type_name);
                PyDynamicTypeMap::add(type_name, dt);
                return py_cast_type(dt);
            },
            py::arg("library"),
            py::arg("name"),
            "Get a DynamicType from a type library in the QosProvider."
        )
#if rti_connext_version_gte(6, 0, 0)
        .def(
            "type",
            [](const QosProvider& qp, const std::string& type_name) {
                auto dt = qp->type(type_name);
                PyDynamicTypeMap::add(type_name, dt);
                return py_cast_type(dt);
            },
            py::arg("name"),
            "Get a DynamicType from the QosProvider."
        )
#endif
        .def(
            "type_libraries",
            [](const QosProvider& qp) {
                return qp->type_libraries();
            },
            "Get a list of the type libraries loaded by this QosProvider."
        )
        .def_property(
            "provider_params",
            [](const QosProvider& qp) {
                return qp->provider_params();
            },
            [](QosProvider& qp, const rti::core::QosProviderParams& params) {
                qp->provider_params(params);
            },
            "Get a copy of or set the QosProviderParams for this QosProvider."
        )
        .def(
            "load_profiles",
            [](QosProvider& qp) {
                qp ->load_profiles();
            },
            "Load the XML QoS profiles from this QosProvider."
        )
        .def(
            "reload_profiles",
            [](QosProvider& qp) {
                qp ->reload_profiles();
            },
            "Reload the XML QoS profiles from this QosProvider."
        )
        .def(
            "unload_profiles",
            [](QosProvider& qp) {
                qp ->unload_profiles();
            },
            "Unload the XML QoS profiles from this QosProvider."
        )
        .def(
            "create_participant_from_config",
            [](QosProvider& qp, const std::string& config, const rti::domain::DomainParticipantConfigParams& params) {
                return qp->create_participant_from_config(config, params);
            },
            py::arg("config"),
            py::arg_v("params", rti::domain::DomainParticipantConfigParams(), "DomainParticipantConfigParams()"),
            "Create a DomainParticipant given a configuration name from a "
            "description provided in an XML configuration file that has been "
            "loaded by this QosProvider."
        )
        .def_static(
            "default",
            &QosProvider::Default,
            "Get the default QosProvider."
        )
        .def_static(
            "reset_default",
            &QosProvider::reset_default,
            "Reset the settings of the default QosProvider."
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
void process_inits<QosProvider>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<QosProvider>(m, "QosProvider");
        }
    );
}

}
