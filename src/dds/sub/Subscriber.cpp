#include "PyConnext.hpp"
#include <dds/sub/Subscriber.hpp>
#include <dds/sub/find.hpp>
#include <rti/rti.hpp>
#include "PyEntity.hpp"
#include "PySubscriberListener.hpp"

using namespace dds::sub;

template<>
void pyrti::init_class_defs(py::class_<pyrti::PySubscriber>& cls) {
    cls
        .def(
            py::init(
                [](pyrti::PyIEntity& e) {
                    auto entity = e.get_entity();
                    return dds::core::polymorphic_cast<pyrti::PySubscriber>(entity);
                }
            )
        )
        .def(
            py::init<
                dds::domain::DomainParticipant&
            >(),
            py::arg("participant"),
            "Create a subscriber under a DomainParticipant."
        )
        .def(
            py::init(
                [](dds::domain::DomainParticipant& dp,
                    const qos::SubscriberQos& qos,
                    pyrti::PySubscriberListener* l,
                    dds::core::status::StatusMask& m) {
                    return pyrti::PySubscriber(dp, qos, l, m);
                }
            ),
            py::arg("participant"),
            py::arg("qos"),
            py::arg("listener") = (pyrti::PySubscriberListener*) nullptr,
            py::arg_v("mask", dds::core::status::StatusMask::all(), "StatusMask.all"),
            py::keep_alive<1,4>(),
            "Create a Subscriber under a DomainParticipant with a listener."
        )
        .def(
            "notify_datareaders",
            &pyrti::PySubscriber::notify_datareaders,
            "This operation invokes the operation on_data_available on the DataReaderListener objects attached to contained DataReader entities with a DATA_AVAILABLE status that is considered changed"
        )
        .def_property_readonly(
            "listener",
            [](const pyrti::PySubscriber& sub) {
                return dynamic_cast<pyrti::PySubscriberListener*>(sub.listener());
            },
            "Get the listener."
        )
        .def(
            "bind_listener",
            [](pyrti::PySubscriber& sub, pyrti::PySubscriberListener* l, const dds::core::status::StatusMask& m) {
                sub.listener(l, m);
            },
            py::arg("listener"),
            py::arg("event_mask"),
            py::keep_alive<1, 2>(),
            "Bind the listener and event mask to the Subscriber."
        )
        .def_property(
            "qos",
            (qos::SubscriberQos (pyrti::PySubscriber::*)() const) &pyrti::PySubscriber::qos,
            (void (pyrti::PySubscriber::*)(const qos::SubscriberQos&)) &pyrti::PySubscriber::qos,
            "Get a copy of or set the SubscriberQos."
        )
        .def_property(
            "default_datareader_qos",
            (qos::DataReaderQos (pyrti::PySubscriber::*)() const) &pyrti::PySubscriber::default_datareader_qos,
            [](pyrti::PySubscriber& sub, const qos::DataReaderQos& qos) {
                return pyrti::PySubscriber(sub.default_datareader_qos(qos));
            },
            "Get a copy of or set the default DataReaderQos."
        )
        .def_property_readonly(
            "participant",
            &pyrti::PySubscriber::participant,
            "Get the parent DomainParticipant for this Subscriber."
        )
        .def(
            "find_datareader",
            [](pyrti::PySubscriber& sub, const std::string& name) {
                return pyrti::PyAnyDataReader(rti::sub::find_datareader_by_name<AnyDataReader>(sub, name));
            },
            py::arg("name"),
            "Find a DataReader in this Subscriber by its name."
        )
        .def(
            "find_topic_datareader",
            [](pyrti::PySubscriber& sub, const std::string& topic_name) {
                return pyrti::PyAnyDataReader(rti::sub::find_datareader_by_topic_name<AnyDataReader>(sub, topic_name));
            },
            py::arg("topic_name"),
            "Find a DataReader in this Subscriber by its topic name. If more than one exists for this Topic, the first one found is returned."
        )
        .def(
            "find_datareader",
            [](const pyrti::PySubscriber& sub) {
                std::vector<pyrti::PyAnyDataReader> v;
                std::back_insert_iterator<std::vector<pyrti::PyAnyDataReader>> it(v);
                rti::sub::find_datareaders(sub, it);
                return v;
            },
            "Find all DataReaders in the Subscriber."
        )
        .def(
            "enable",
            &pyrti::PySubscriber::enable,
            "Enables this entity (if it was created disabled)."
        )
        .def_property_readonly(
            "status_changes",
            &pyrti::PySubscriber::status_changes,
            "The list of communication statuses that are triggered."
        )
        .def_property_readonly(
            "instance_handle",
            &pyrti::PySubscriber::instance_handle,
            "The instance handle that represents this entity."
        )
        .def(
            "close",
            &pyrti::PySubscriber::close,
            "Forces the destruction of this entity."
        )
        .def(
            "retain",
            &pyrti::PySubscriber::retain,
            "Disables the automatic destruction of this entity."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );

    py::implicitly_convertible<pyrti::PyIEntity, pyrti::PySubscriber>();
}

template<>
void pyrti::process_inits<Subscriber>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PySubscriber>(m, "Subscriber");
        }
    );
}
