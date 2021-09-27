/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */

#include "PyConnext.hpp"
#include <dds/topic/BuiltinTopic.hpp>
#include "PySeq.hpp"
#include "PyInitType.hpp"
#include "PyInitOpaqueTypeContainers.hpp"

#if rti_connext_version_lt(6, 1, 0, 0)
#define SBTD_DATA_TAG(s) s->data_tags()
#else
#define SBTD_DATA_TAG(s) s.data_tag()
#endif

using namespace dds::topic;

INIT_OPAQUE_TYPE_CONTAINERS(dds::topic::SubscriptionBuiltinTopicData);

namespace pyrti {

template<>
void init_dds_typed_topic_template(
        py::class_<
                PyTopic<dds::topic::SubscriptionBuiltinTopicData>,
                PyITopicDescription<dds::topic::SubscriptionBuiltinTopicData>,
                PyIAnyTopic,
                std::unique_ptr<PyTopic<dds::topic::SubscriptionBuiltinTopicData>, no_gil_delete<PyTopic<dds::topic::SubscriptionBuiltinTopicData>>>>& cls)
{
    init_dds_typed_topic_base_template(cls);
}

template<>
void init_class_defs(py::class_<SubscriptionBuiltinTopicData>& cls)
{
    cls.def(py::init<>(), "Create a default SubscriptionBuiltinTopicData.")
            .def_property_readonly(
                    "key",
                    &SubscriptionBuiltinTopicData::key,
                    "The DCPS key to distinguish entries.")
            .def_property_readonly(
                    "participant_key",
                    &SubscriptionBuiltinTopicData::participant_key,
                    "The DCPS key of the DomainParticipant to which the "
                    "DataWriter belongs.")
            .def_property_readonly(
                    "topic_name",
                    [](const SubscriptionBuiltinTopicData& data) {
                        return data.topic_name().to_std_string();
                    },
                    "The name of the related Topic.")
            .def_property_readonly(
                    "type_name",
                    [](const SubscriptionBuiltinTopicData& data) {
                        return data.type_name().to_std_string();
                    },
                    "The name of the type attached to the Topic.")
            .def_property_readonly(
                    "durability",
                    &SubscriptionBuiltinTopicData::durability,
                    "The Durability policy of the corresponding DataWriter.")
            .def_property_readonly(
                    "deadline",
                    &SubscriptionBuiltinTopicData::deadline,
                    "The Deadline policy of the corresponding DataWriter.")
            .def_property_readonly(
                    "latency_budget",
                    &SubscriptionBuiltinTopicData::latency_budget,
                    "The LatencyBudget policy of the corresponding DataWriter.")
            .def_property_readonly(
                    "liveliness",
                    &SubscriptionBuiltinTopicData::liveliness,
                    "The Liveliness policy of the corresponding DataWriter.")
            .def_property_readonly(
                    "reliability",
                    &SubscriptionBuiltinTopicData::reliability,
                    "The Reliability policy of the corresponding DataWriter.")
            .def_property_readonly(
                    "ownership",
                    &SubscriptionBuiltinTopicData::ownership,
                    "The Ownership policy of the corresponding DataWriter.")
            .def_property_readonly(
                    "destination_order",
                    &SubscriptionBuiltinTopicData::destination_order,
                    "The DestinationOrder policy of the corresponding "
                    "DataWriter.")
            .def_property_readonly(
                    "user_data",
                    &SubscriptionBuiltinTopicData::user_data,
                    "The UserData policy of the corresponding DataWriter.")
            .def_property_readonly(
                    "time_based_filter",
                    &SubscriptionBuiltinTopicData::time_based_filter,
                    "The TimeBasedFilter policy of the corresponding "
                    "DataWriter.")
            .def_property_readonly(
                    "presentation",
                    &SubscriptionBuiltinTopicData::presentation,
                    "The Presentation policy of the corresponding DataWriter.")
            .def_property_readonly(
                    "partition",
                    &SubscriptionBuiltinTopicData::partition,
                    "The Partition policy of the corresponding DataWriter.")
            .def_property_readonly(
                    "topic_data",
                    &SubscriptionBuiltinTopicData::topic_data,
                    "The TopicData policy of the corresponding DataWriter.")
            .def_property_readonly(
                    "group_data",
                    &SubscriptionBuiltinTopicData::group_data,
                    "The GroupData policy of the corresponding DataWriter.")
#if rti_connext_version_gte(6, 0, 0, 0)
            .def_property_readonly(
                    "representation",
                    &SubscriptionBuiltinTopicData::representation,
                    "The Representation policy of the corresponding "
                    "DataWriter.")
            // BUG: data_tag() is not compatible with default delegate
            .def_property_readonly(
                    "data_tag",
                    [](const SubscriptionBuiltinTopicData& s) {
                        return SBTD_DATA_TAG(s);
                    },
                    "The DataTag policy of the corresponding DataWriter.")
#endif
            .def_property_readonly(
                    "type",
                    [](const SubscriptionBuiltinTopicData& p) {
                        dds::core::optional<py::object> retval;
                        auto dw_type = p->type();
                        if (has_value(dw_type)) {
                            retval = py_cast_type(get_value(dw_type));
                        }
                        return retval;
                    },
                    py::return_value_policy::move,
                    "The type.")
            .def_property_readonly(
                    "subscriber_key",
                    [](const SubscriptionBuiltinTopicData& p) {
                        return p->subscriber_key();
                    },
                    "The DCPS key of the Publisher to which the DataWriter "
                    "belongs.")
            .def_property_readonly(
                    "property",
                    [](const SubscriptionBuiltinTopicData& p) {
                        return p->property();
                    },
                    "The propagated name-value properties of the corresponding "
                    "DataWriter.")
            .def_property_readonly(
                    "unicast_locators",
                    [](const SubscriptionBuiltinTopicData& p) {
                        return p->unicast_locators();
                    },
                    "The custom unicast locators that the endpoint can "
                    "specify.")
            .def_property_readonly(
                    "multicast_locators",
                    [](const SubscriptionBuiltinTopicData& p) {
                        return p->multicast_locators();
                    },
                    "The custom multicast locators that the endpoint can "
                    "specify.")
            .def_property_readonly(
                    "content_filter_property",
                    [](const SubscriptionBuiltinTopicData& p) {
                        return p->content_filter_property();
                    },
                    "The custom multicast locators that the endpoint can "
                    "specify.")
            .def_property_readonly(
                    "virtual_guid",
                    [](const SubscriptionBuiltinTopicData& p) {
                        return p->virtual_guid();
                    },
                    "The virtual Guid associated to the DataWriter.")
            .def_property_readonly(
                    "rtps_protocol_version",
                    [](const SubscriptionBuiltinTopicData& p) {
                        return p->rtps_protocol_version();
                    },
                    "The version number of the RTPS wire protocol used.")
            .def_property_readonly(
                    "rtps_vendor_id",
                    [](const SubscriptionBuiltinTopicData& p) {
                        return p->rtps_vendor_id();
                    },
                    "The ID of the vendor implementing the RTPS wire protocol.")
            .def_property_readonly(
                    "product_version",
                    [](const SubscriptionBuiltinTopicData& p) {
                        return p->product_version();
                    },
                    "The current version for RTI Connext.")
            .def_property_readonly(
                    "disable_positive_acks",
                    [](const SubscriptionBuiltinTopicData& p) {
                        return p->disable_positive_acks();
                    },
                    "See whether or not a matching DataReader will send "
                    "positive "
                    "acknowledgments for reliability.")
            .def_property_readonly(
                    "subscription_name",
                    [](const SubscriptionBuiltinTopicData& p) {
                        return p->subscription_name();
                    },
                    "The subscription name and role name.")
            .def_property_readonly(
                    "service",
                    [](const SubscriptionBuiltinTopicData& p) {
                        return p->service();
                    },
                    "The Service policy")
            .def_property_readonly_static(
                    "builtin_topic_name",
                    [](py::object&) {
                        return dds::topic::subscription_topic_name();
                    },
                    "Subscription builtin topic name.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<SubscriptionBuiltinTopicData>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m, &l]() mutable {
        return init_type_class<SubscriptionBuiltinTopicData>(
                m,
                l,
                "SubscriptionBuiltinTopicData");
    });
}

}  // namespace pyrti
