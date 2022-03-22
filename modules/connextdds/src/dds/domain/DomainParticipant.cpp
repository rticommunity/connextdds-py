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
#include "PySeq.hpp"
#include "PyEntity.hpp"
#include "PyAnyDataWriter.hpp"
#include "PyAnyDataReader.hpp"
#include "PyDataReader.hpp"
#include "PyDomainParticipantListener.hpp"
#include <rti/rti.hpp>

using namespace dds::domain;

namespace pyrti {

inline DomainParticipantListenerPtr get_dp_listener(const dds::domain::DomainParticipant& dp) {
    return get_listener<dds::domain::DomainParticipant, DomainParticipantListenerPtr>(dp);
}

inline void set_dp_listener(
        dds::domain::DomainParticipant& dp,
        PyDomainParticipantListenerPtr l) {
    set_listener<dds::domain::DomainParticipant, PyDomainParticipantListenerPtr>(dp, l);
}

inline void set_dp_listener(
        dds::domain::DomainParticipant& dp,
        PyDomainParticipantListenerPtr l,
        const dds::core::status::StatusMask& m) {
    set_listener<dds::domain::DomainParticipant, PyDomainParticipantListenerPtr>(dp, l, m);
}

inline PyDomainParticipantListenerPtr downcast_dp_listener_ptr(DomainParticipantListenerPtr l) {
    return downcast_listener_ptr<PyDomainParticipantListenerPtr, DomainParticipantListenerPtr>(l);
}

std::recursive_mutex PyDomainParticipant::_property_lock;


PyDomainParticipant::PyDomainParticipant(
        int32_t d,
        const dds::domain::qos::DomainParticipantQos& q,
        PyDomainParticipantListenerPtr l,
        const dds::core::status::StatusMask& m)
        : dds::domain::DomainParticipant(d, q, l, m)
{
    if (nullptr != l) {
        py::gil_scoped_acquire acquire;
        py::cast(l).inc_ref();
    }
}


PyDomainParticipant::~PyDomainParticipant()
{
    if (*this != dds::core::null) {
        if (this->delegate().use_count() <= LISTENER_USE_COUNT_MIN && !this->delegate()->closed()) {
            auto listener_ptr = get_dp_listener(*this);
            if (nullptr != listener_ptr) {
                PyDomainParticipantListenerPtr null_listener = nullptr;
                set_dp_listener(*this, null_listener, dds::core::status::StatusMask::none());
                {
                    py::gil_scoped_acquire acquire;
                    py::cast(listener_ptr).dec_ref();
                }
            }
        }
    }
    {
        py::gil_scoped_acquire acquire;
        this->_properties.clear();
    }
}


void PyDomainParticipant::py_close()
{
    auto listener_ptr = get_dp_listener(*this);
    if (nullptr != listener_ptr) {
        PyDomainParticipantListenerPtr null_listener = nullptr;
        set_dp_listener(*this, null_listener, dds::core::status::StatusMask::none());
        {
            py::gil_scoped_acquire acquire;
            py::cast(listener_ptr).dec_ref();
        }
    }
    this->close();
}


template<typename T, typename F>
PyDataReader<T>& PyDomainParticipant::py_builtin_reader(
        PyDomainParticipant::Property key,
        F topic_name_func)
{
    std::lock_guard<std::recursive_mutex> lock(PyDomainParticipant::_property_lock);
    if (this->_properties.count(key)) {
        py::gil_scoped_acquire acquire;
        auto& dr = py::cast<PyDataReader<T>&>(this->_properties[key]);
        if (!dr->closed()) return dr;
    }
    std::vector<PyDataReader<T>> v;
    dds::sub::find<PyDataReader<T>>(
            dds::sub::builtin_subscriber(*this),
            topic_name_func(),
            std::back_inserter(v));
    if (v.size() == 0)
        throw dds::core::Error(
                "Unable to retrieve built-in topic "
                "reader.");
    auto dr = v[0];
    {
        py::gil_scoped_acquire acquire;
        auto obj = py::cast(dr);
        this->_properties[key] = obj;
        return py::cast<PyDataReader<T>&>(obj);
    }
}


template<typename T, typename F>
T& PyDomainParticipant::py_entity_property(
        PyDomainParticipant::Property key,
        F getter) 
{
    std::lock_guard<std::recursive_mutex> lock(PyDomainParticipant::_property_lock);
    if (this->_properties.count(key)) {
        py::gil_scoped_acquire acquire;
        auto& prop = py::cast<T&>(this->_properties[key]);
        if (!prop->closed()) return prop;
    }
    T entity = T(getter(*this));
    {
        py::gil_scoped_acquire acquire;
        auto obj = py::cast(entity);
        this->_properties[key] = obj;
        return py::cast<T&>(obj);
    }
}


template<typename ParticipantFwdIterator>
static
uint32_t find_participants(ParticipantFwdIterator begin)
{
    DDS_DomainParticipantSeq native_participants = DDS_SEQUENCE_INITIALIZER;
    // Ensure sequence destruction
    rti::core::detail::NativeSequenceAdapter<DDS_DomainParticipant>
            participants_adapter(native_participants);

    DDS_ReturnCode_t retcode = DDS_DomainParticipantFactory_get_participants(
            DDS_DomainParticipantFactory_get_instance(),
            &native_participants);
    rti::core::check_return_code(retcode, "get native participants");

    rti::core::detail::create_from_native_entity<
            PyDomainParticipant,
            DDS_DomainParticipant,
            ParticipantFwdIterator>(begin, participants_adapter);

    return participants_adapter.size();
}


template<>
void init_class_defs(
        py::class_<
            PyDomainParticipant,
            PyIEntity,
            std::unique_ptr<PyDomainParticipant, no_gil_delete<PyDomainParticipant>>>& cls)
{
    cls.def(py::init<int32_t>(),
            py::arg("domain_id"),
            py::call_guard<py::gil_scoped_release>(),
            "Create a new DomainParticipant with default QoS.")
            .def(py::init([](int32_t id,
                             const qos::DomainParticipantQos& q,
                             PyDomainParticipantListenerPtr listener,
                             const dds::core::status::StatusMask& m) {
                     return PyDomainParticipant(id, q, listener, m);
                 }),
                 py::arg("domain_id"),
                 py::arg("qos"),
                 py::arg("listener") = py::none(),
                 py::arg_v(
                         "mask",
                         dds::core::status::StatusMask::all(),
                         "StatusMask.ALL"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a new DomainParticipant")
            .def(py::init([](PyIEntity& e) {
                     auto entity = e.get_entity();
                     return dds::core::polymorphic_cast<PyDomainParticipant>(
                             entity);
                 }),
                 py::arg("entity"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Downcast an IEntity to a DomainParticipant.")
            .def_property(
                    "listener",
                    [](const PyDomainParticipant& dp) {
                        py::gil_scoped_release guard;
                        return downcast_dp_listener_ptr(get_dp_listener(dp));
                    },
                    [](PyDomainParticipant& dp,
                       PyDomainParticipantListenerPtr listener) {
                        py::gil_scoped_release guard;
                        if (nullptr != listener) {
                            py::gil_scoped_acquire acquire;
                            py::cast(listener).inc_ref();
                        }
                        if (nullptr != get_dp_listener(dp)) {
                            py::gil_scoped_acquire acquire;
                            py::cast(get_dp_listener(dp)).dec_ref();
                        }
                        set_dp_listener(dp, listener);
                    },
                    "Get the listener.")
            .def(
                    "set_listener",
                    [](PyDomainParticipant& dp,
                       PyDomainParticipantListenerPtr listener,
                       const dds::core::status::StatusMask& m) {
                        if (nullptr != listener) {
                            py::gil_scoped_acquire acquire;
                            py::cast(listener).inc_ref();
                        }
                        if (nullptr != get_dp_listener(dp)) {
                            py::gil_scoped_acquire acquire;
                            py::cast(get_dp_listener(dp)).dec_ref();
                        }
                        set_dp_listener(dp, listener, m);
                    },
                    py::arg("listener"),
                    py::arg("event_mask"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Bind the listener and event mask to the "
                    "DomainParticipant.")
            .def_property(
                    "qos",
                    [](const PyDomainParticipant& dp) {
                        py::gil_scoped_release guard;
                        return dp.qos();
                    },
                    [](PyDomainParticipant& dp,
                       const qos::DomainParticipantQos& qos) {
                        py::gil_scoped_release guard;
                        dp.qos(qos);
                    },
                    "Get the domain participant's QoS."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def(
                    "__lshift__",
                    [](PyDomainParticipant& dp,
                       const dds::domain::qos::DomainParticipantQos& q)
                            -> PyDomainParticipant& {
                        PyDomainParticipant(dp << q);
                        return dp;
                    },
                    py::is_operator(),
                    py::call_guard<py::gil_scoped_release>(),
                    "Set the domain participant's QoS.")
            .def(
                    "__rshift__",
                    [](PyDomainParticipant& dp,
                       dds::domain::qos::DomainParticipantQos& q)
                            -> PyDomainParticipant& {
                        dp >> q;
                        return dp;
                    },
                    py::is_operator(),
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the domain participant's QoS.")
            .def_property_readonly(
                    "domain_id",
                    [](const PyDomainParticipant& dp) {
                        py::gil_scoped_release guard;
                        return dp.domain_id();
                    },
                    "The unique domain identifier.")
            .def("assert_liveliness",
                 &PyDomainParticipant::assert_liveliness,
                 py::call_guard<py::gil_scoped_release>(),
                 "Manually assert the liveliness of the DomainParticipant.")
            .def("contains_entity",
                 &PyDomainParticipant::contains_entity,
                 py::arg("handle"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Check whether or not the given handle represents an Entity "
                 "that was created from the DomainParticipant.")
            .def_property_readonly(
                    "current_time",
                    [](PyDomainParticipant& dp) {
                        py::gil_scoped_release guard;
                        return dp.current_time();
                    },
                    "Get the current time.")
            .def_property(
                    "default_publisher_qos",
                    [](const PyDomainParticipant& dp) {
                        py::gil_scoped_release guard;
                        return dp.default_publisher_qos();
                    },
                    [](PyDomainParticipant& dp,
                       const dds::pub::qos::PublisherQos& qos) {
                        py::gil_scoped_release guard;
                        dp.default_publisher_qos(qos);
                    },
                    "The default PublisherQos."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property(
                    "default_subscriber_qos",
                    [](const PyDomainParticipant& dp) {
                        py::gil_scoped_release guard;
                        return dp.default_subscriber_qos();
                    },
                    [](PyDomainParticipant& dp,
                       const dds::sub::qos::SubscriberQos& qos) {
                        py::gil_scoped_release guard;
                        dp.default_subscriber_qos(qos);
                    },
                    "The default SubscriberQos."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property(
                    "default_topic_qos",
                    [](const PyDomainParticipant& dp) {
                        py::gil_scoped_release guard;
                        return dp.default_topic_qos();
                    },
                    [](PyDomainParticipant& dp,
                       const dds::topic::qos::TopicQos& qos) {
                        py::gil_scoped_release guard;
                        return dp.default_topic_qos(qos);
                    },
                    "The default TopicQos."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property(
                    "default_datawriter_qos",
                    [](const PyDomainParticipant& dp) {
                        py::gil_scoped_release guard;
                        return dp->default_datawriter_qos();
                    },
                    [](PyDomainParticipant& dp,
                       const dds::pub::qos::DataWriterQos& qos) {
                        py::gil_scoped_release guard;
                        return dp->default_datawriter_qos(qos);
                    },
                    "The default DataWriterQos."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property(
                    "default_datareader_qos",
                    [](const PyDomainParticipant& dp) {
                        py::gil_scoped_release guard;
                        return dp->default_datareader_qos();
                    },
                    [](PyDomainParticipant& dp,
                       const dds::sub::qos::DataReaderQos& qos) {
                        py::gil_scoped_release guard;
                        return dp->default_datareader_qos(qos);
                    },
                    "The default DataReaderQos."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def(
                    "register_contentfilter",
                    [](PyDomainParticipant& dp,
                       rti::topic::ContentFilterBase* cf,
                       const std::string& fn) {
                        dds::core::smart_ptr_traits<
                                rti::topic::ContentFilterBase>::ref_type
                                ptr(cf,
                                    [](rti::topic::ContentFilterBase* filt) {});
                        rti::topic::CustomFilter<rti::topic::ContentFilterBase>
                                custom(ptr);
                        dp->register_contentfilter(custom, fn);
                        {
                            py::gil_scoped_acquire acquire;
                            py::cast(cf).inc_ref();
                        }
                    },
                    py::arg("filter"),
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Register a content filter which can be used to create a "
                    "ContentFiltertedTopic.")
            .def(
                    "unregister_contentfilter",
                    [](PyDomainParticipant& dp, const std::string& fn) {
                        auto filter = rti::topic::find_content_filter<
                                rti::topic::ContentFilterBase>(dp, fn);
                        if (dds::core::null != filter) {
                            dp->unregister_contentfilter(fn);
                            {
                                py::gil_scoped_acquire acquire;
                                py::cast(filter.get()).dec_ref();
                            }
                        }
                    },
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Unregister content filter previously registered to this "
                    "DomainParticipant.")
            .def(
                    "find_contentfilter",
                    [](PyDomainParticipant& dp, const std::string& fn) {
                        dds::core::optional<rti::topic::ContentFilterBase*>
                                retval;
                        auto filter = rti::topic::find_content_filter<
                                rti::topic::ContentFilterBase>(dp, fn);
                        if (filter != dds::core::null) {
                            retval = rti::topic::find_content_filter<
                                             rti::topic::ContentFilterBase>(
                                             dp,
                                             fn)
                                             .get();
                        }
                        return retval;
                    },
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Find content filter previously registered to this "
                    "DomainParticipant.")
            .def(
                    "register_type",
                    [](PyDomainParticipant& dp,
                       const std::string& n,
                       const dds::core::xtypes::DynamicType& t,
                       const rti::core::xtypes::
                               DynamicDataTypeSerializationProperty& sp) {
                        rti::domain::register_type(dp, n, t, sp);
                    },
                    py::arg("name"),
                    py::arg("type"),
                    py::arg_v(
                            "serialization_property",
                            rti::core::xtypes::
                                    DynamicDataTypeSerializationProperty(),
                            "DynamicDataTypeSerializationProperty.DEFAULT"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Registers a DynamicType with specific serialization "
                    "properties.")
            .def(
                    "unregister_type",
                    [](PyDomainParticipant& dp, const std::string& name) {
                        dp->unregister_type(name);
                    },
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Unregister a type previously registered to this "
                    "DomainParticipant.")
            .def(
                    "is_type_registered",
                    [](const PyDomainParticipant& dp, const std::string& name) {
                        return dp->is_type_registered(name);
                    },
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Check if a type has been registered to this "
                    "DomainParticipant.")
            .def(
                    "add_peer",
                    [](PyDomainParticipant& dp,
                       const std::string& peer_desc_str) {
                        dp->add_peer(peer_desc_str);
                    },
                    py::arg("peer"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Attempt to contact an additional peer participant.")
            .def(
                    "add_peers",
                    [](PyDomainParticipant& dp,
                       const std::vector<std::string>& peer_list) {
                        for (auto& p : peer_list) {
                            dp->add_peer(p);
                        }
                    },
                    py::arg("peers"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Add a sequence of peers to be contacted.")
            .def(
                    "remove_peer",
                    [](PyDomainParticipant& dp,
                       const std::string& peer_desc_str) {
                        dp->remove_peer(peer_desc_str);
                    },
                    py::arg("peer"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Remove a peer participant from this list that this "
                    "DomainParticipant will attempt to communicate with.")
            .def(
                    "remove_peers",
                    [](PyDomainParticipant& dp,
                       const std::vector<std::string>& peer_list) {
                        for (auto& p : peer_list) {
                            dp->remove_peer(p);
                        }
                    },
                    py::arg("peers"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Remove a sequence of peers from the contact list.")
            .def(
                    "resume_endpoint_discovery",
                    [](PyDomainParticipant& dp,
                       const dds::core::InstanceHandle& rph) {
                        dp->resume_endpoint_discovery(rph);
                    },
                    py::arg("handle"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Initiates endpoint discovery with the remote "
                    "DomainParticipant identified by its InstanceHandle.")
            .def(
                    "delete_durable_subscription",
                    [](PyDomainParticipant& dp,
                       const rti::core::EndpointGroup& group) {
                        dp->delete_durable_subscription(group);
                    },
                    py::arg("group"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Deletes an existing Durable Subscription on all "
                    "Persistence Services.")
            .def(
                    "register_durable_subscription",
                    [](PyDomainParticipant& dp,
                       const rti::core::EndpointGroup& group,
                       const std::string& topic_name) {
                        dp->register_durable_subscription(group, topic_name);
                    },
                    py::arg("group"),
                    py::arg("topic_name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Registers a Durable Subscription on the specified Topic "
                    "on all Persistence Services")
            .def_property_readonly(
                    "participant_protocol_status",
                    [](PyDomainParticipant& dp) {
                        py::gil_scoped_release guard;
                        return dp->participant_protocol_status();
                    },
                    "Get the protocol status for this participant")
            .def_property_static(
                    "participant_factory_qos",
                    [](py::object&) {
                        py::gil_scoped_release guard;
                        return PyDomainParticipant::participant_factory_qos();
                    },
                    [](py::object&,
                       const dds::domain::qos::DomainParticipantFactoryQos&
                               qos) {
                        py::gil_scoped_release guard;
                        PyDomainParticipant::participant_factory_qos(qos);
                    },
                    "Get a copy of or set the DomainParticipantFactoryQos.")
            .def_static(
                    "finalize_participant_factory",
                    &PyDomainParticipant::finalize_participant_factory,
                    py::call_guard<py::gil_scoped_release>(),
                    "Finalize the DomainParticipantFactory")
            .def_property_static(
                    "default_participant_qos",
                    [](py::object&) {
                        py::gil_scoped_release guard;
                        return PyDomainParticipant::default_participant_qos();
                    },
                    [](py::object&,
                       const dds::domain::qos::DomainParticipantQos& qos) {
                        py::gil_scoped_release guard;
                        PyDomainParticipant::default_participant_qos(qos);
                    },
                    "Get a copy of or set the default DomainParticipantQos.")
            .def(
                    "__enter__",
                    [](PyDomainParticipant& dp) -> PyDomainParticipant& {
                        return dp;
                    },
                    "Enter a context for this Domain Participant, to be "
                    "cleaned up on exiting context")
            .def(
                    "__exit__",
                    [](PyDomainParticipant& dp,
                       py::object,
                       py::object,
                       py::object) {
                        if (!dp->closed())
                            dp.py_close();
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Exit the context for this Domain Participant, cleaning up "
                    "resources.")
            .def(
                    "close_contained_entities",
                    [](PyDomainParticipant& dp) {
                        dp->close_contained_entities();
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Destroy all contained entities.")
            .def_static(
                    "find",
                    [](const std::string& n) {
                        dds::core::optional<PyDomainParticipant> retval;
                        auto participant =
                                rti::domain::find_participant_by_name(n);
                        if (participant != dds::core::null) {
                            retval = PyDomainParticipant(participant);
                        }
                        return retval;
                    },
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Find a local DomainParticipant by its name.")
            .def_static(
                    "find",
                    []() {
                        std::vector<PyDomainParticipant> v;
                        pyrti::find_participants(std::back_inserter(v));
                        return v;
                    },
                    py::call_guard<py::gil_scoped_release>(),
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
                    py::call_guard<py::gil_scoped_release>(),
                    "Find a local DomainParticipant with the given domain ID. "
                    "If more "
                    "than one DomainParticipant on the same domain exists in "
                    "the "
                    "application, it is not specified which will be returned.")
            .def(
                    "find_publisher",
                    [](PyDomainParticipant& dp, const std::string& name) {
                        return PyPublisher(rti::pub::find_publisher(dp, name));
                    },
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Lookup a Publisher within the DomainParticipant by its "
                    "entity name.")
            .def(
                    "find_publishers",
                    [](const PyDomainParticipant& dp) {
                        std::vector<PyPublisher> v;
                        rti::pub::find_publishers(dp, std::back_inserter(v));
                        return v;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Find all Publishers within the DomainParticipant.")
            .def_property_readonly(
                    "implicit_publisher",
                    [](PyDomainParticipant& dp) -> PyPublisher& {
                        py::gil_scoped_release guard;
                        auto pub = PyPublisher(rti::pub::implicit_publisher(dp));
                        return dp.py_entity_property<PyPublisher>(
                            PyDomainParticipant::Property::IMPLICIT_PUBLISHER,
                            rti::pub::implicit_publisher);
                    },
                    "Get the implicit Publisher for the DomainParticipant.")
            .def_property_readonly(
                    "builtin_subscriber",
                    [](PyDomainParticipant& dp) -> PySubscriber& {
                        py::gil_scoped_release guard;
                        return dp.py_entity_property<PySubscriber>(
                            PyDomainParticipant::Property::BUILTIN_SUBSCRIBER,
                            dds::sub::builtin_subscriber);
                    },
                    "Get the built-in subscriber for the DomainParticipant.")
            .def(
                    "find_subscriber",
                    [](PyDomainParticipant& dp, const std::string& name) {
                        return PySubscriber(
                                rti::sub::find_subscriber(dp, name));
                    },
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Find a Subscriber in the DomainParticipant by its entity "
                    "name.")
            .def(
                    "find_subscribers",
                    [](PyDomainParticipant& dp) {
                        std::vector<PySubscriber> v;
                        rti::sub::find_subscribers(dp, std::back_inserter(v));
                        return v;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Find all subscribers within the DomainParticipant.")
            .def_property_readonly(
                    "implicit_subscriber",
                    [](PyDomainParticipant& dp) -> PySubscriber& {
                        py::gil_scoped_release guard;
                        return dp.py_entity_property<PySubscriber>(
                                PyDomainParticipant::Property::
                                        IMPLICIT_SUBSCRIBER,
                                rti::sub::implicit_subscriber);
                    },
                    "Get the implicit Subscriber for the DomainParticipant.")
            .def(
                    "ignore_participant",
                    [](const PyDomainParticipant& dp,
                       const dds::core::InstanceHandle& h) {
                        dds::domain::ignore(dp, h);
                    },
                    py::arg("handle"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Ignore a DomainParticipant given it's handle.")
            .def(
                    "ignore_participants",
                    [](PyDomainParticipant& dp,
                       std::vector<dds::core::InstanceHandle>& v) {
                        dds::domain::ignore(dp, v.begin(), v.end());
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Ignore DomainParticipants given a list of handles.")
            .def(
                    "ignore_topic",
                    [](PyDomainParticipant& dp,
                       const dds::core::InstanceHandle& handle) {
                        dds::topic::ignore(dp, handle);
                    },
                    py::arg("handle"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Ignore a Topic matching the provided handle.")
            .def(
                    "ignore_topics",
                    [](PyDomainParticipant& dp,
                       std::vector<dds::core::InstanceHandle>& v) {
                        dds::topic::ignore(dp, v.begin(), v.end());
                    },
                    py::arg("handles"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Ignore a list of Topics specified by their handles.")
            .def(
                    "ignore_datawriter",
                    [](PyDomainParticipant& dp,
                       const dds::core::InstanceHandle& handle) {
                        dds::pub::ignore(dp, handle);
                    },
                    py::arg("handle"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Ignore a DataWriter matching the provided handle.")
            .def(
                    "ignore_datawriters",
                    [](PyDomainParticipant& dp,
                       std::vector<dds::core::InstanceHandle>& v) {
                        dds::pub::ignore(dp, v.begin(), v.end());
                    },
                    py::arg("handles"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Ignore a list of DataWriters specified by their handles.")
            .def(
                    "ignore_datareader",
                    [](PyDomainParticipant& dp,
                       const dds::core::InstanceHandle& handle) {
                        dds::sub::ignore(dp, handle);
                    },
                    py::arg("handle"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Ignore a DataReader matching the provided handle.")
            .def(
                    "ignore_datareaders",
                    [](PyDomainParticipant& dp,
                       std::vector<dds::core::InstanceHandle>& v) {
                        dds::sub::ignore(dp, v.begin(), v.end());
                    },
                    py::arg("handles"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Ignore a list of DataReaders specified by their handles.")
#ifndef _MSC_VER
            .def(
                    "find_topics",
                    [](PyDomainParticipant& dp) {
                        std::vector<PyAnyTopic> v;
                        rti::topic::find_topics(dp, std::back_inserter(v));
                        return v;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Find all Topics in the DomainParticipant.")
#endif
            .def(
                    "discovered_topics",
                    [](const PyDomainParticipant& dp) {
                        std::vector<dds::core::InstanceHandle> v;
                        dds::topic::discover_any_topic(
                                dp,
                                std::back_inserter(v));
                        return v;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Get all Topic handles discovered by this "
                    "DomainParticipant.")
            .def(
                    "discovered_topic_data",
                    [](const PyDomainParticipant& dp,
                       const dds::core::InstanceHandle& handle) {
                        return dds::topic::discover_topic_data(dp, handle);
                    },
                    py::arg("handle"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Get information about a discovered topic using its "
                    "handle.")
            .def(
                    "discovered_topic_data",
                    [](const PyDomainParticipant& dp, const PyIEntity& topic) {
                        return dds::topic::discover_topic_data(
                                dp,
                                topic.py_instance_handle());
                    },
                    py::arg("topic"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Get information about a discovered topic.")
            .def(
                    "discovered_topic_data",
                    [](const PyDomainParticipant& dp,
                       const std::vector<dds::core::InstanceHandle>& handles) {
                        std::vector<dds::topic::TopicBuiltinTopicData> v;
                        for (auto& h : handles) {
                            v.push_back(dds::topic::discover_topic_data(dp, h));
                        }
                        return v;
                    },
                    py::arg("handles"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Get information about a discovered topics "
                    "with their handles.")
            .def(
                    "discovered_topic_data",
                    [](const PyDomainParticipant& dp) {
                        std::vector<dds::topic::TopicBuiltinTopicData> v;
                        dds::topic::discover_topic_data(
                                dp,
                                std::back_inserter(v));
                        return v;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Get information about all discovered topics.")
            .def(
                    "find_datawriter",
                    [](PyDomainParticipant& dp, const std::string name) {
                        auto dw = rti::pub::find_datawriter_by_name<
                                dds::pub::AnyDataWriter>(dp, name);
                        return PyAnyDataWriter(dw);
                    },
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Find a DataWriter by its name.")
            .def(
                    "find_datareader",
                    [](PyDomainParticipant& dp, const std::string name) {
                        auto dr = rti::sub::find_datareader_by_name<
                                dds::sub::AnyDataReader>(dp, name);
                        return PyAnyDataReader(dr);
                    },
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Find a DataReader by its name.")
            .def(
                    "find_registered_content_filters",
                    [](PyDomainParticipant& dp) {
                        std::vector<std::string> v;
                        rti::topic::find_registered_content_filters(
                                dp,
                                std::back_inserter(v));
                        return v;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Retrieve a list of all registered content filter names.")
            .def_property_readonly(
                    "participant_reader",
                    [](PyDomainParticipant& dp)
                            -> PyDataReader<
                                    dds::topic::ParticipantBuiltinTopicData>& {
                        py::gil_scoped_release guard;
                        return dp.py_builtin_reader<dds::topic::ParticipantBuiltinTopicData>(
                            PyDomainParticipant::Property::PARTICIPANT_READER,
                            dds::topic::participant_topic_name);
                    },
                    "Get the DomainParticipant built-in topic reader.")
            .def_property_readonly(
                    "publication_reader",
                    [](PyDomainParticipant& dp)
                            -> PyDataReader<
                                    dds::topic::PublicationBuiltinTopicData>& {
                        py::gil_scoped_release guard;
                        return dp.py_builtin_reader<dds::topic::PublicationBuiltinTopicData>(
                            PyDomainParticipant::Property::PUBLICATION_READER,
                            dds::topic::publication_topic_name);
                    },
                    "Get the publication built-in topic reader.")
            .def_property_readonly(
                    "subscription_reader",
                    [](PyDomainParticipant& dp)
                            -> PyDataReader<
                                    dds::topic::SubscriptionBuiltinTopicData>& {
                        py::gil_scoped_release guard;
                        return dp.py_builtin_reader<dds::topic::SubscriptionBuiltinTopicData>(
                            PyDomainParticipant::Property::SUBSCRIPTION_READER,
                            dds::topic::subscription_topic_name);
                    },
                    "Get the subscription built-in topic reader.")
            /*
            !!!NOT USED BY RTI CONNEXT DDS!!!
            .def_property_readonly(
                    "topic_reader",
                    [](PyDomainParticipant& dp) ->
            PyDataReader<dds::topic::TopicBuiltinTopicData>& {
                        py::gil_scoped_release guard;
                        return
            dp.py_builtin_reader<dds::topic::TopicBuiltinTopicData>(
                            PyDomainParticipant::Property::TOPIC_READER,
                            dds::topic::topic_topic_name);
                    },
                    "Get the topic built-in topic reader.")
            */
            .def_property_readonly(
                    "service_request_reader",
                    [](PyDomainParticipant& dp)
                            -> PyDataReader<rti::topic::ServiceRequest>& {
                        py::gil_scoped_release guard;
                        return dp.py_builtin_reader<rti::topic::ServiceRequest>(
                            PyDomainParticipant::Property::SERVICE_REQUEST_READER,
                            rti::topic::service_request_topic_name);
                    },
                    "Get the ServiceRequest built-in topic reader.")
            .def(
                    "discovered_participants",
                    [](PyDomainParticipant& dp) {
                        return rti::domain::discovered_participants(dp);
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Retrieves the instance handles of other "
                    "DomainParticipants "
                    "discovered by this one.")
            .def(
                    "discovered_participant_data",
                    [](PyDomainParticipant& dp,
                       const dds::core::InstanceHandle& handle) {
                        return rti::domain::discovered_participant_data(
                                dp,
                                handle);
                    },
                    py::arg("handle"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Retrieve DomainParticipant information by handle.")
            .def(
                    "discovered_participant_data",
                    [](PyDomainParticipant& dp,
                       const std::vector<dds::core::InstanceHandle>& handles) {
                        std::vector<dds::topic::ParticipantBuiltinTopicData> v;
                        for (auto& h : handles) {
                            v.push_back(
                                    rti::domain::discovered_participant_data(
                                            dp,
                                            h));
                        }
                        return v;
                    },
                    py::arg("handles"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Retrieve DomainParticipant information with a sequence of "
                    "handles.")
            .def(
                    "find_flow_controller",
                    [](PyDomainParticipant& dp, const std::string& name) {
                        dds::core::optional<rti::pub::FlowController> retval;
                        auto fc = rti::pub::find_flow_controller(dp, name);
                        if (fc != dds::core::null) {
                            retval = fc;
                        }
                        return retval;
                    },
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Find a FlowController configured in this "
                    "DomainParticipant.")
            .def(py::self == py::self,
                 py::call_guard<py::gil_scoped_release>(),
                 "Test for equality.")
            .def(py::self != py::self,
                 py::call_guard<py::gil_scoped_release>(),
                 "Test for inequality.")
            .doc() =
            "Container for all Entity objects.\n"
            "* It acts as a container for all other Entity objects.\n"
            "* It acts as a factory for the Publisher, Subscriber, Topic and "
            "Entity objects.\n"
            "* It represents the participation of the application on a "
            "communication plane that isolates applications running on the "
            "same set of physical computers from each other. A domain "
            "establishes a virtual network linking all applications that share "
            "the same domainId and isolating them from applications running on "
            "different domains. In this way, several independent distributed "
            "applications can coexist in the same physical network without "
            "interfering, or even being aware of each other.\n"
            "* It provides administration services in the domain, offering "
            "operations that allow the application to ignore locally any "
            "information about a given participant (DomainParticipant.ignore), "
            "publication (ignore_publication), subscription "
            "(ignore_subscription) or topic (ignore_topic).";
}

template<>
void process_inits<DomainParticipant>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class_with_seq<
            PyDomainParticipant,
            PyIEntity,
            std::unique_ptr<PyDomainParticipant, no_gil_delete<PyDomainParticipant>>>(
                m,
                "DomainParticipant");
    });
}

}  // namespace pyrti
