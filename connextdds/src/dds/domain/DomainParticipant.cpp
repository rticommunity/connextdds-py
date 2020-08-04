#include "PyAnyDataReader.hpp"
#include "PyAnyDataWriter.hpp"
#include "PyConnext.hpp"
#include "PyDataReader.hpp"
#include "PyDomainParticipantListener.hpp"
#include "PyEntity.hpp"
#include "PySeq.hpp"
#include <rti/rti.hpp>

using namespace dds::domain;

namespace pyrti {

PyDomainParticipant::PyDomainParticipant(
    int32_t d, const dds::domain::qos::DomainParticipantQos &q,
    PyDomainParticipantListener *l, const dds::core::status::StatusMask &m)
    : dds::domain::DomainParticipant(d, q, l, m) {
  if (nullptr != l)
    py::cast(l).inc_ref();
}

PyDomainParticipant::~PyDomainParticipant() {
  if (*this != dds::core::null) {
    if (this->delegate().use_count() <= 2 && !this->delegate()->closed() &&
        nullptr != this->listener()) {
      py::object listener = py::cast(this->listener());
      this->listener(nullptr, dds::core::status::StatusMask::none());
      listener.dec_ref();
    }
  }
}

void PyDomainParticipant::py_close() {
  if (nullptr != this->listener()) {
    py::object listener = py::cast(this->listener());
    this->listener(nullptr, dds::core::status::StatusMask::none());
    listener.dec_ref();
  }
  this->close();
}

template <typename ParticipantFwdIterator>
uint32_t find_participants(ParticipantFwdIterator begin) {
  DDS_DomainParticipantSeq native_participants = DDS_SEQUENCE_INITIALIZER;
  // Ensure sequence destruction
  rti::core::detail::NativeSequenceAdapter<DDS_DomainParticipant>
      participants_adapter(native_participants);

  DDS_ReturnCode_t retcode = DDS_DomainParticipantFactory_get_participants(
      DDS_DomainParticipantFactory_get_instance(), &native_participants);
  rti::core::check_return_code(retcode, "get native participants");

  rti::core::detail::create_from_native_entity<
      PyDomainParticipant, DDS_DomainParticipant, ParticipantFwdIterator>(
      begin, participants_adapter);

  return participants_adapter.size();
}

template <>
void init_class_defs(py::class_<PyDomainParticipant, PyIEntity> &cls) {
  cls.def(py::init<int32_t>(), py::arg("domain_id"),
          py::call_guard<py::gil_scoped_release>(),
          "Create a new DomainParticipant with default QoS.")
      .def(py::init([](int32_t id, const qos::DomainParticipantQos &q,
                       dds::core::optional<PyDomainParticipantListener *> l,
                       const dds::core::status::StatusMask &m) {
             auto listener = has_value(l) ? get_value(l) : nullptr;
             return PyDomainParticipant(id, q, listener, m);
           }),
           py::arg("domain_id"), py::arg("qos"),
           py::arg("listener") = py::none(),
           py::arg_v("mask", dds::core::status::StatusMask::all(),
                     "StatusMask.all()"),
           py::call_guard<py::gil_scoped_release>(),
           "Create a new DomainParticipant")
      .def(py::init([](PyIEntity &e) {
             auto entity = e.get_entity();
             return dds::core::polymorphic_cast<PyDomainParticipant>(entity);
           }),
           py::arg("entity"), py::call_guard<py::gil_scoped_release>(),
           "Downcast an IEntity to a DomainParticipant.")
      .def_property_readonly(
          "listener",
          [](const PyDomainParticipant &dp) {
            dds::core::optional<PyDomainParticipantListener *> l;
            auto ptr =
                dynamic_cast<PyDomainParticipantListener *>(dp.listener());
            if (nullptr != ptr)
              l = ptr;
            return l;
          },
          "Get the listener.")
      .def(
          "bind_listener",
          [](PyDomainParticipant &dp,
             dds::core::optional<PyDomainParticipantListener *> l,
             const dds::core::status::StatusMask &m) {
            auto listener = has_value(l) ? get_value(l) : nullptr;
            if (nullptr != listener) {
              py::cast(listener).inc_ref();
            }
            if (nullptr != dp.listener()) {
              py::cast(dp.listener()).dec_ref();
            }
            dp.listener(listener, m);
          },
          py::arg("listener"), py::arg("event_mask"),
          "Bind the listener and event mask to the DomainParticipant.")
      .def_property(
          "qos",
          (qos::DomainParticipantQos(PyDomainParticipant::*)() const) &
              PyDomainParticipant::qos,
          (void (PyDomainParticipant::*)(const qos::DomainParticipantQos &)) &
              PyDomainParticipant::qos,
          "Get the domain participant's QoS."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def(
          "__lshift__",
          [](PyDomainParticipant &dp,
             const dds::domain::qos::DomainParticipantQos &q) {
            return PyDomainParticipant(dp << q);
          },
          py::is_operator(), "Set the domain participant's QoS.")
      .def(
          "__rshift__",
          [](PyDomainParticipant &dp,
             dds::domain::qos::DomainParticipantQos &q) {
            auto participant = dp >> q;
            return PyDomainParticipant(participant);
          },
          py::is_operator(), "Get the domain participant's QoS.")
      .def_property_readonly("domain_id", &PyDomainParticipant::domain_id,
                             "The unique domain identifier.")
      .def("assert_liveliness", &PyDomainParticipant::assert_liveliness,
           "Manually assert the liveliness of the DomainParticipant.")
      .def("contains_entity", &PyDomainParticipant::contains_entity,
           py::arg("handle"),
           "Check whether or not the given handle represents an Entity that "
           "was created from the DomainParticipant.")
      .def_property_readonly("current_time", &PyDomainParticipant::current_time,
                             "Get the current time.")
      .def_property(
          "default_publisher_qos",
          (dds::pub::qos::PublisherQos(PyDomainParticipant::*)() const) &
              PyDomainParticipant::default_publisher_qos,
          [](PyDomainParticipant &dp, const dds::pub::qos::PublisherQos &q) {
            return PyDomainParticipant(dp.default_publisher_qos(q));
          },
          "The default PublisherQos."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "default_subscriber_qos",
          (dds::sub::qos::SubscriberQos(PyDomainParticipant::*)() const) &
              PyDomainParticipant::default_subscriber_qos,
          [](PyDomainParticipant &dp, const dds::sub::qos::SubscriberQos &q) {
            return PyDomainParticipant(dp.default_subscriber_qos(q));
          },
          "The default SubscriberQos."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "default_topic_qos",
          (dds::topic::qos::TopicQos(PyDomainParticipant::*)() const) &
              PyDomainParticipant::default_topic_qos,
          [](PyDomainParticipant &dp, const dds::topic::qos::TopicQos &q) {
            return PyDomainParticipant(dp.default_topic_qos(q));
          },
          "The default TopicQos."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "default_datawriter_qos",
          [](const PyDomainParticipant &dp) {
            return dp->default_datawriter_qos();
          },
          [](PyDomainParticipant &dp, const dds::pub::qos::DataWriterQos &qos) {
            return dp->default_datawriter_qos(qos);
          },
          "The default DataWriterQos."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def_property(
          "default_datareader_qos",
          [](const PyDomainParticipant &dp) {
            return dp->default_datareader_qos();
          },
          [](PyDomainParticipant &dp, const dds::sub::qos::DataReaderQos &qos) {
            return dp->default_datareader_qos(qos);
          },
          "The default DataReaderQos."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def(
          "register_contentfilter",
          [](PyDomainParticipant &dp, rti::topic::ContentFilterBase *cf,
             const std::string &fn) {
            dds::core::smart_ptr_traits<rti::topic::ContentFilterBase>::ref_type
                ptr(cf, [](rti::topic::ContentFilterBase *filt) {});
            rti::topic::CustomFilter<rti::topic::ContentFilterBase> custom(ptr);
            dp->register_contentfilter(custom, fn);
            py::cast(cf).inc_ref();
          },
          py::arg("filter"), py::arg("name"),
          "Register a content filter which can be used to create a "
          "ContentFiltertedTopic.")
      .def(
          "unregister_contentfilter",
          [](PyDomainParticipant &dp, const std::string &fn) {
            auto filter =
                rti::topic::find_content_filter<rti::topic::ContentFilterBase>(
                    dp, fn);
            if (dds::core::null != filter) {
              dp->unregister_contentfilter(fn);
              py::cast(filter.get()).dec_ref();
            }
          },
          py::arg("name"),
          "Unregister content filter previously registered to this "
          "DomainParticipant.")
      .def(
          "find_contentfilter",
          [](PyDomainParticipant &dp, const std::string &fn) {
            dds::core::optional<rti::topic::ContentFilterBase *> retval;
            auto filter =
                rti::topic::find_content_filter<rti::topic::ContentFilterBase>(
                    dp, fn);
            if (filter != dds::core::null) {
              retval = rti::topic::find_content_filter<
                           rti::topic::ContentFilterBase>(dp, fn)
                           .get();
            }
            return retval;
          },
          py::arg("name"),
          "Find content filter previously registered to this "
          "DomainParticipant.")
      .def(
          "register_type",
          [](PyDomainParticipant &dp, const std::string &n,
             const dds::core::xtypes::DynamicType &t,
             const rti::core::xtypes::DynamicDataTypeSerializationProperty
                 &sp) { rti::domain::register_type(dp, n, t, sp); },
          py::arg("name"), py::arg("type"),
          py::arg_v(
              "serialization_property",
              rti::core::xtypes::DynamicDataTypeSerializationProperty::DEFAULT,
              "DynamicDataTypeSerializationProperty.DEFAULT"),
          "Registers a DynamicType with specific serialization properties.")
      .def(
          "unregister_type",
          [](PyDomainParticipant &dp, const std::string &name) {
            return dp->unregister_type(name);
          },
          py::arg("name"),
          "Unregister a type previously registered to this DomainParticipant.")
      .def(
          "is_type_registered",
          [](const PyDomainParticipant &dp, const std::string &name) {
            return dp->is_type_registered(name);
          },
          py::arg("name"),
          "Check if a type has been registered to this DomainParticipant.")
      .def(
          "add_peer",
          [](PyDomainParticipant &dp, const std::string &peer_desc_str) {
            dp->add_peer(peer_desc_str);
          },
          py::arg("peer"), "Attempt to contact an additional peer participant.")
      .def(
          "add_peers",
          [](PyDomainParticipant &dp,
             const std::vector<std::string> &peer_list) {
            for (auto &p : peer_list) {
              dp->add_peer(p);
            }
          },
          py::arg("peers"), "Add a sequence of peers to be contacted.")
      .def(
          "remove_peer",
          [](PyDomainParticipant &dp, const std::string &peer_desc_str) {
            dp->remove_peer(peer_desc_str);
          },
          py::arg("peer"),
          "Remove a peer participant from this list that this "
          "DomainParticipant will attempt to communicate with.")
      .def(
          "remove_peers",
          [](PyDomainParticipant &dp,
             const std::vector<std::string> &peer_list) {
            for (auto &p : peer_list) {
              dp->remove_peer(p);
            }
          },
          py::arg("peers"), "Remove a sequence of peers from the contact list.")
      .def(
          "resume_endpoint_discovery",
          [](PyDomainParticipant &dp, const dds::core::InstanceHandle &rph) {
            dp->resume_endpoint_discovery(rph);
          },
          py::arg("handle"),
          "Initiates endpoint discovery with the remote DomainParticipant "
          "identified by its InstanceHandle.")
      .def(
          "delete_durable_subscription",
          [](PyDomainParticipant &dp, const rti::core::EndpointGroup &group) {
            dp->delete_durable_subscription(group);
          },
          py::arg("group"),
          "Deletes an existing Durable Subscription on all Persistence "
          "Services.")
      .def(
          "register_durable_subscription",
          [](PyDomainParticipant &dp, const rti::core::EndpointGroup &group,
             const std::string &topic_name) {
            dp->register_durable_subscription(group, topic_name);
          },
          py::arg("group"), py::arg("topic_name"),
          "Registers a Durable Subscription on the specified Topic on all "
          "Persistence Services")
      .def_property_readonly(
          "participant_protocol_status",
          [](PyDomainParticipant &dp) {
            return dp->participant_protocol_status();
          },
          "Get the protocol status for this participant")
      .def_property_static(
          "participant_factory_qos",
          [](py::object &) {
            return PyDomainParticipant::participant_factory_qos();
          },
          [](py::object &,
             const dds::domain::qos::DomainParticipantFactoryQos &qos) {
            PyDomainParticipant::participant_factory_qos(qos);
          },
          "Get a copy of or set the DomainParticipantFactoryQos.")
      .def_static("finalize_participant_factory",
                  &PyDomainParticipant::finalize_participant_factory,
                  "Finalize the DomainParticipantFactory")
      .def_property_static(
          "default_participant_qos",
          [](py::object &) {
            return PyDomainParticipant::default_participant_qos();
          },
          [](py::object &, const dds::domain::qos::DomainParticipantQos &qos) {
            PyDomainParticipant::default_participant_qos(qos);
          },
          "Get a copy of or set the default DomainParticipantQos.")
      .def(
          "__enter__", [](PyDomainParticipant &dp) { return dp; },
          "Enter a context for this Domain Participant, to be cleaned up on "
          "exiting context")
      .def(
          "__exit__",
          [](PyDomainParticipant &dp, py::object, py::object, py::object) {
            if (!dp->closed())
              dp.close();
          },
          "Exit the context for this Domain Participant, cleaning up "
          "resources.")
      .def_static(
          "find",
          [](const std::string &n) {
            dds::core::optional<PyDomainParticipant> retval;
            auto participant = rti::domain::find_participant_by_name(n);
            if (participant != dds::core::null) {
              retval = PyDomainParticipant(participant);
            }
            return retval;
          },
          py::arg("name"), "Find a local DomainParticipant by its name.")
      .def_static(
          "find",
          []() {
            std::vector<PyDomainParticipant> v;
            find_participants(std::back_inserter(v));
            return v;
          },
          "Find all local DomainParticipants.")
      .def_static(
          "find",
          [](int32_t domain_id) {
            dds::core::optional<PyDomainParticipant> retval;
            auto participant = dds::domain::find(domain_id);
            if (participant != dds::core::null) {
              retval = PyDomainParticipant(participant);
            }
            return retval;
          },
          py::arg("domain_id"),
          "Find a local DomainParticipant with the given domain ID. If more "
          "than one DomainParticipant on the same domain exists in the "
          "application, it is not specified which will be returned.")
      .def(
          "find_publisher",
          [](PyDomainParticipant &dp, const std::string &name) {
            return PyPublisher(rti::pub::find_publisher(dp, name));
          },
          py::arg("name"),
          "Lookup a Publisher within the DomainParticipant by its entity name.")
      .def(
          "find_publishers",
          [](const PyDomainParticipant &dp) {
            std::vector<PyPublisher> v;
            rti::pub::find_publishers(dp, std::back_inserter(v));
            return v;
          },
          "Find all Publishers within the DomainParticipant.")
      .def_property_readonly(
          "implicit_publisher",
          [](PyDomainParticipant &dp) {
            auto retval = PyPublisher(rti::pub::implicit_publisher(dp));
            dp.py_add_prop(py::cast(retval));
            return retval;
          },
          "Get the implicit Publisher for the DomainParticipant.")
      .def_property_readonly(
          "builtin_subscriber",
          [](PyDomainParticipant &dp) {
            auto retval = PySubscriber(dds::sub::builtin_subscriber(dp));
            dp.py_add_prop(py::cast(retval));
            return retval;
          },
          "Get the built-in subscriber for the DomainParticipant.")
      .def(
          "find_subscriber",
          [](PyDomainParticipant &dp, const std::string &name) {
            return PySubscriber(rti::sub::find_subscriber(dp, name));
          },
          py::arg("name"),
          "Find a Subscriber in the DomainParticipant by its entity name.")
      .def(
          "find_subscribers",
          [](PyDomainParticipant &dp) {
            std::vector<PySubscriber> v;
            rti::sub::find_subscribers(dp, std::back_inserter(v));
            return v;
          },
          "Find all subscribers within the DomainParticipant.")
      .def_property_readonly(
          "implicit_subscriber",
          [](PyDomainParticipant &dp) {
            auto retval = PySubscriber(rti::sub::implicit_subscriber(dp));
            dp.py_add_prop(py::cast(retval));
            return retval;
          },
          "Get the implicit Subscriber for the DomainParticipant.")
      .def(
          "ignore_participant",
          [](const PyDomainParticipant &dp,
             const dds::core::InstanceHandle &h) {
            dds::domain::ignore(dp, h);
          },
          py::arg("handle"), "Ignore a DomainParticipant given it's handle.")
      .def("ignore_participants",
           [](PyDomainParticipant &dp,
              std::vector<dds::core::InstanceHandle> &v) {
             dds::domain::ignore(dp, v.begin(), v.end());
           })
      .def(
          "ignore_topic",
          [](PyDomainParticipant &dp, const dds::core::InstanceHandle &handle) {
            dds::topic::ignore(dp, handle);
          },
          py::arg("handle"), "Ignore a Topic matching the provided handle.")
      .def(
          "ignore_topics",
          [](PyDomainParticipant &dp,
             std::vector<dds::core::InstanceHandle> &v) {
            dds::topic::ignore(dp, v.begin(), v.end());
          },
          py::arg("handles"),
          "Ignore a list of Topics specified by their handles.")
      .def(
          "ignore_datawriter",
          [](PyDomainParticipant &dp, const dds::core::InstanceHandle &handle) {
            dds::pub::ignore(dp, handle);
          },
          py::arg("handle"),
          "Ignore a DataWriter matching the provided handle.")
      .def(
          "ignore_datawriters",
          [](PyDomainParticipant &dp,
             std::vector<dds::core::InstanceHandle> &v) {
            dds::pub::ignore(dp, v.begin(), v.end());
          },
          py::arg("handles"),
          "Ignore a list of DataWriters specified by their handles.")
      .def(
          "ignore_datareader",
          [](PyDomainParticipant &dp, const dds::core::InstanceHandle &handle) {
            dds::sub::ignore(dp, handle);
          },
          py::arg("handle"),
          "Ignore a DataReader matching the provided handle.")
      .def(
          "ignore_datareaders",
          [](PyDomainParticipant &dp,
             std::vector<dds::core::InstanceHandle> &v) {
            dds::sub::ignore(dp, v.begin(), v.end());
          },
          py::arg("handles"),
          "Ignore a list of DataReaders specified by their handles.")
      .def(
          "find_topics",
          [](PyDomainParticipant &dp) {
            std::vector<PyAnyTopic> v;
            rti::topic::find_topics(dp, std::back_inserter(v));
            return v;
          },
          "Find all Topics in the DomainParticipant.")
      .def(
          "discovered_topics",
          [](const PyDomainParticipant &dp) {
            std::vector<dds::core::InstanceHandle> v;
            dds::topic::discover_any_topic(dp, std::back_inserter(v));
            return v;
          },
          "Get all Topic handles discovered by this DomainParticipant.")
      .def(
          "discovered_topic_data",
          [](const PyDomainParticipant &dp,
             const dds::core::InstanceHandle &handle) {
            return dds::topic::discover_topic_data(dp, handle);
          },
          py::arg("handle"),
          "Get information about a discovered topic using its handle.")
      .def(
          "discovered_topic_data",
          [](const PyDomainParticipant &dp, const PyIEntity &topic) {
            return dds::topic::discover_topic_data(dp,
                                                   topic.py_instance_handle());
          },
          py::arg("topic"), "Get information about a discovered topic.")
      .def(
          "discovered_topic_data",
          [](const PyDomainParticipant &dp,
             const std::vector<dds::core::InstanceHandle> &handles) {
            std::vector<dds::topic::TopicBuiltinTopicData> v;
            for (auto &h : handles) {
              v.push_back(dds::topic::discover_topic_data(dp, h));
            }
            return v;
          },
          py::arg("handles"), "Get information about a discovered topic.")
      .def(
          "discovered_topic_data",
          [](const PyDomainParticipant &dp) {
            std::vector<dds::topic::TopicBuiltinTopicData> v;
            dds::topic::discover_topic_data(dp, std::back_inserter(v));
            return v;
          },
          "Get information about all discovered topics.")
      .def(
          "find_datawriter",
          [](PyDomainParticipant &dp, const std::string name) {
            auto dw =
                rti::pub::find_datawriter_by_name<dds::pub::AnyDataWriter>(
                    dp, name);
            return PyAnyDataWriter(dw);
          },
          py::arg("name"), "Find a DataWriter by its name.")
      .def(
          "find_datareader",
          [](PyDomainParticipant &dp, const std::string name) {
            auto dr =
                rti::sub::find_datareader_by_name<dds::sub::AnyDataReader>(
                    dp, name);
            return PyAnyDataReader(dr);
          },
          py::arg("name"), "Find a DataReader by its name.")
      .def(
          "find_registered_content_filters",
          [](PyDomainParticipant &dp) {
            std::vector<std::string> v;
            rti::topic::find_registered_content_filters(dp,
                                                        std::back_inserter(v));
            return v;
          },
          "Retrieve a list of all registered content filter names.")
      .def_property_readonly(
          "participant_reader",
          [](PyDomainParticipant &dp) {
            std::vector<PyDataReader<dds::topic::ParticipantBuiltinTopicData>>
                v;
            dds::sub::find<
                PyDataReader<dds::topic::ParticipantBuiltinTopicData>>(
                dds::sub::builtin_subscriber(dp),
                dds::topic::participant_topic_name(), std::back_inserter(v));
            if (v.size() == 0)
              throw dds::core::Error(
                  "Unable to retrieve built-in topic reader.");
            dp.py_add_prop(py::cast(v[0]));
            return v[0];
          },
          "Get the DomainParticipant built-in topic reader.")
      .def_property_readonly(
          "publication_reader",
          [](PyDomainParticipant &dp) {
            std::vector<PyDataReader<dds::topic::PublicationBuiltinTopicData>>
                v;
            dds::sub::find<
                PyDataReader<dds::topic::PublicationBuiltinTopicData>>(
                dds::sub::builtin_subscriber(dp),
                dds::topic::publication_topic_name(), std::back_inserter(v));
            if (v.size() == 0)
              throw dds::core::Error(
                  "Unable to retrieve built-in topic reader.");
            dp.py_add_prop(py::cast(v[0]));
            return v[0];
          },
          "Get the publication built-in topic reader.")
      .def_property_readonly(
          "subscription_reader",
          [](PyDomainParticipant &dp) {
            std::vector<PyDataReader<dds::topic::SubscriptionBuiltinTopicData>>
                v;
            dds::sub::find<
                PyDataReader<dds::topic::SubscriptionBuiltinTopicData>>(
                dds::sub::builtin_subscriber(dp),
                dds::topic::subscription_topic_name(), std::back_inserter(v));
            if (v.size() == 0)
              throw dds::core::Error(
                  "Unable to retrieve built-in topic reader.");
            dp.py_add_prop(py::cast(v[0]));
            return v[0];
          },
          "Get the subscription built-in topic reader.")
      .def_property_readonly(
          "topic_reader",
          [](PyDomainParticipant &dp) {
            std::vector<PyDataReader<dds::topic::TopicBuiltinTopicData>> v;
            dds::sub::find<PyDataReader<dds::topic::TopicBuiltinTopicData>>(
                dds::sub::builtin_subscriber(dp),
                dds::topic::topic_topic_name(), std::back_inserter(v));
            if (v.size() == 0)
              throw dds::core::Error(
                  "Unable to retrieve built-in topic reader.");
            dp.py_add_prop(py::cast(v[0]));
            return v[0];
          },
          "Get the topic built-in topic reader.")
      .def_property_readonly(
          "service_request_reader",
          [](PyDomainParticipant &dp) {
            std::vector<PyDataReader<rti::topic::ServiceRequest>> v;
            dds::sub::find<PyDataReader<rti::topic::ServiceRequest>>(
                dds::sub::builtin_subscriber(dp),
                rti::topic::service_request_topic_name(),
                std::back_inserter(v));
            if (v.size() == 0)
              throw dds::core::Error(
                  "Unable to retrieve built-in topic reader.");
            dp.py_add_prop(py::cast(v[0]));
            return v[0];
          },
          "Get the ServiceRequest built-in topic reader.")
      .def(
          "discovered_participants",
          [](PyDomainParticipant &dp) {
            return rti::domain::discovered_participants(dp);
          },
          "Retrieves the instance handles of other DomainParticipants "
          "discovered by this one.")
      .def(
          "discovered_participant_data",
          [](PyDomainParticipant &dp, const dds::core::InstanceHandle &handle) {
            return rti::domain::discovered_participant_data(dp, handle);
          },
          py::arg("handle"),
          "Retrieve DomainParticipant information by handle.")
      .def(
          "discovered_participant_data",
          [](PyDomainParticipant &dp,
             const std::vector<dds::core::InstanceHandle> &handles) {
            std::vector<dds::topic::ParticipantBuiltinTopicData> v;
            for (auto &h : handles) {
              v.push_back(rti::domain::discovered_participant_data(dp, h));
            }
            return v;
          },
          py::arg("handles"),
          "Retrieve DomainParticipant information with a sequence of "
          "handles.")
      .def(
          "publication_data",
          [](PyDomainParticipant &dp, const dds::core::InstanceHandle &handle) {
            DDS_PublicationBuiltinTopicData pbitd;
            DDS_DomainParticipant_get_publication_data(
                dp.delegate()->native_participant(), &pbitd,
                &handle.delegate().native());
            dds::topic::PublicationBuiltinTopicData *ptr =
                reinterpret_cast<dds::topic::PublicationBuiltinTopicData *>(
                    &pbitd);
            return *ptr;
          },
          py::arg("handle"), "Retrieve publication data by handle.")
      .def(
          "publication_data",
          [](PyDomainParticipant &dp,
             const std::vector<dds::core::InstanceHandle> &handles) {
            std::vector<dds::topic::PublicationBuiltinTopicData> v;
            for (auto &handle : handles) {
              DDS_PublicationBuiltinTopicData pbitd;
              DDS_DomainParticipant_get_publication_data(
                  dp.delegate()->native_participant(), &pbitd,
                  &handle.delegate().native());
              dds::topic::PublicationBuiltinTopicData *ptr =
                  reinterpret_cast<dds::topic::PublicationBuiltinTopicData *>(
                      &pbitd);
              v.push_back(*ptr);
            }
            return v;
          },
          py::arg("handles"),
          "Retrieve publication data for a sequence of handles.")
      .def(
          "subscription_data",
          [](PyDomainParticipant &dp, const dds::core::InstanceHandle &handle) {
            DDS_SubscriptionBuiltinTopicData sbitd;
            DDS_DomainParticipant_get_subscription_data(
                dp.delegate()->native_participant(), &sbitd,
                &handle.delegate().native());
            dds::topic::SubscriptionBuiltinTopicData *ptr =
                reinterpret_cast<dds::topic::SubscriptionBuiltinTopicData *>(
                    &sbitd);
            return *ptr;
          },
          py::arg("handle"), "Retrieve subscription data by handle.")
      .def(
          "subscription_data",
          [](PyDomainParticipant &dp,
             const std::vector<dds::core::InstanceHandle> &handles) {
            std::vector<dds::topic::SubscriptionBuiltinTopicData> v;
            for (auto &handle : handles) {
              DDS_SubscriptionBuiltinTopicData sbitd;
              DDS_DomainParticipant_get_subscription_data(
                  dp.delegate()->native_participant(), &sbitd,
                  &handle.delegate().native());
              dds::topic::SubscriptionBuiltinTopicData *ptr =
                  reinterpret_cast<dds::topic::SubscriptionBuiltinTopicData *>(
                      &sbitd);
              v.push_back(*ptr);
            }
            return v;
          },
          py::arg("handles"),
          "Retrieve subscription data for a sequence of handles.")
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.")
      .doc() =
      "Container for all Entity objects.\n"
      "* It acts as a container for all other Entity objects.\n"
      "* It acts as a factory for the Publisher, Subscriber, Topic and Entity "
      "objects.\n"
      "* It represents the participation of the application on a communication "
      "plane that isolates applications running on the same set of physical "
      "computers from each other. A domain establishes a virtual network "
      "linking all applications that share the same domainId and isolating "
      "them from applications running on different domains. In this way, "
      "several independent distributed applications can coexist in the same "
      "physical network without interfering, or even being aware of each "
      "other.\n"
      "* It provides administration services in the domain, offering "
      "operations that allow the application to ignore locally any information "
      "about a given participant (DomainParticipant.ignore), publication "
      "(ignore_publication), subscription (ignore_subscription) or topic "
      "(ignore_topic).";
}

template <>
void process_inits<DomainParticipant>(py::module &m, ClassInitList &l) {
  l.push_back([m]() mutable {
    return init_class_with_seq<PyDomainParticipant, PyIEntity>(
        m, "DomainParticipant");
  });
}

} // namespace pyrti
