#include "PyConnext.hpp"
#include <dds/pub/Publisher.hpp>
#include <dds/core/cond/StatusCondition.hpp>
#include <rti/rti.hpp>
#include "PyEntity.hpp"
#include "PyAnyDataWriter.hpp"
#include "PyPublisherListener.hpp"

using namespace dds::pub;

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyPublisher, pyrti::PyIEntity>& cls) {
    cls
        
        .def(
            py::init<
                const pyrti::PyDomainParticipant&
            >(),
            py::arg("participant"),
            "Create a publisher."
        )
        .def(
            py::init<
                const pyrti::PyDomainParticipant&, 
                const qos::PublisherQos&,
                pyrti::PyPublisherListener*,
                const dds::core::status::StatusMask&
            >(),
            py::arg("participant"),
            py::arg("qos"),
            py::arg("listener") = (pyrti::PyPublisherListener*) nullptr,
            py::arg_v("mask", dds::core::status::StatusMask::all(), "StatusMask.all()"),
            py::keep_alive<1,4>(),
            "Create a Publisher with the desired QoS policies and specified listener"
        )
        .def(
            py::init(
                [](pyrti::PyIEntity& e) {
                    auto entity = e.get_entity();
                    return dds::core::polymorphic_cast<pyrti::PyPublisher>(entity);
                }
            )
        )
        .def_property(
            "qos",
            (const dds::pub::qos::PublisherQos (pyrti::PyPublisher::*)()const) &pyrti::PyPublisher::qos,
            (void (pyrti::PyPublisher::*)(const dds::pub::qos::PublisherQos&)) &pyrti::PyPublisher::qos,
            "Get a copy of or set the PublisherQos."
        )
        .def(
            "__lshift__",
            [](pyrti::PyPublisher& pub, const qos::PublisherQos& qos) {
                return pyrti::PyPublisher(pub << qos);
            },
            py::is_operator(),
            "Set the PublisherQos."
        )
        .def(
            "__rshift__",
            [](pyrti::PyPublisher& pub, qos::PublisherQos& qos) {
                return pyrti::PyPublisher(pub >> qos);
            },
            py::is_operator(),
            "Copy the PublisherQos."
        )
        .def_property(
            "default_datawriter_qos",
            (dds::pub::qos::DataWriterQos (pyrti::PyPublisher::*)() const) &pyrti::PyPublisher::default_datawriter_qos,
            [](pyrti::PyPublisher& pub, qos::DataWriterQos& qos) {
                return pyrti::PyPublisher(pub.default_datawriter_qos(qos));
            },
            "Get a copy of or set the default DataWriterQos."
        )
        .def_property_readonly(
            "listener",
            [](const PyPublisher& pub) {
                return dynamic_cast<pyrti::PyPublisherListener*>(pub.listener());
            },
            "Get the listener."
        )
        .def(
            "bind_listener",
            [](pyrti::PyPublisher& pub, pyrti::PyPublisherListener* l, const dds::core::status::StatusMask& m) {
                pub.listener(l, m); 
            },
            py::arg("listener"),
            py::arg("event_mask"),
            py::keep_alive<1, 2>(),
            "Bind the listener and event mask to the Publisher."
        )
        .def(
            "wait_for_acknowledgments",
            &pyrti::PyPublisher::wait_for_acknowledgments,
            py::arg("max_wait"),
            "Blocks until all data written by this Publisher's reliable DataWriters is acknowledged or the timeout expires."
        )
        .def_property_readonly(
            "participant",
            [](pyrti::PyPublisher& pub) {
                auto dp = pub.participant();
                return pyrti::PyDomainParticipant(dp);
            },
            "Get the parent DomainParticipant of this Publisher."
        )
        .def(
            "wait_for_asynchronous_publishing",
            [](pyrti::PyPublisher& pub, const dds::core::Duration& max_wait) {
                pub->wait_for_acknowledgments(max_wait);
            },
            py::arg("max_wait"),
            "Blocks until asynchronous sending is complete or timeout expires."
        )
        .def(
            "find_datawriter",
            [](pyrti::PyPublisher& pub, const std::string& name) {
                auto dw = rti::pub::find_datawriter_by_name<dds::pub::AnyDataWriter>(pub, name);
                return pyrti::PyAnyDataWriter(dw);
            },
            py::arg("name"),
            "Find a DataWriter in this Publisher by its name."
        )
        .def(
            "find_topic_datawriter",
            [](Publisher& pub, const std::string& topic_name) {
                auto dw = rti::pub::find_datawriter_by_topic_name<dds::pub::AnyDataWriter>(pub, topic_name);
                return pyrti::PyAnyDataWriter(dw);
            },
            py::arg("topic_name"),
            "Find a DataWriter in this Publisher by its topic name. If more than one exists for this Topic, the first one found is returned."
        )
        .def(
            "find_datawriters",
            [](const pyrti::PyPublisher& pub) {
                std::vector<pyrti::PyAnyDataWriter> v;
                rti::pub::find_datawriters(pub, std::back_inserter(v));
                return v;
            },
            "Find all DataWriters in the Publisher."
        )
        .def(
            "enable",
            &pyrti::PyPublisher::enable,
            "Enables this entity (if it was created disabled)."
        )
        .def_property_readonly(
            "status_changes",
            &pyrti::PyPublisher::status_changes,
            "The list of communication statuses that are triggered."
        )
        .def_property_readonly(
            "instance_handle",
            &pyrti::PyPublisher::instance_handle,
            "The instance handle that represents this entity."
        )
        .def(
            "close",
            &pyrti::PyPublisher::close,
            "Forces the destruction of this entity."
        )
        .def(
            "retain",
            &pyrti::PyPublisher::retain,
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

    py::implicitly_convertible<pyrti::PyIEntity, pyrti::PyPublisher>();
}

template<>
void pyrti::process_inits<Publisher>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyPublisher, pyrti::PyIEntity>(m, "Publisher");
        }
    );   
}
