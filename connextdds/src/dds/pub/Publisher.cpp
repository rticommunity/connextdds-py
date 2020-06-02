#include "PyConnext.hpp"
#include "PySeq.hpp"
#include <dds/pub/Publisher.hpp>
#include <dds/core/cond/StatusCondition.hpp>
#include <rti/rti.hpp>
#include "PyEntity.hpp"
#include "PyAnyDataWriter.hpp"
#include "PyPublisherListener.hpp"

using namespace dds::pub;

namespace pyrti {

PyPublisher::PyPublisher(
    const PyDomainParticipant& p,
    const dds::pub::qos::PublisherQos& q,
    PyPublisherListener* l,
    const dds::core::status::StatusMask& m) : 
dds::pub::Publisher(p, q, l, m) {
    if (nullptr != l) py::cast(l).inc_ref();
}

template<>
void init_class_defs(py::class_<PyPublisher, PyIEntity>& cls) {
    cls
        
        .def(
            py::init<
                const PyDomainParticipant&
            >(),
            py::arg("participant"),
            "Create a publisher."
        )
        .def(
            py::init(
                [](
                    const PyDomainParticipant& dp, 
                    const qos::PublisherQos& q,
                    dds::core::optional<PyPublisherListener*> l,
                    const dds::core::status::StatusMask& m
                ) {
                    auto listener = has_value(l) ? get_value(l) : nullptr;
                    return PyPublisher(dp, q, listener, m);
                }
            ),
            py::arg("participant"),
            py::arg("qos"),
            py::arg("listener") = py::none(),
            py::arg_v("mask", dds::core::status::StatusMask::all(), "StatusMask.all()"),
            "Create a Publisher with the desired QoS policies and specified listener"
        )
        .def(
            py::init(
                [](PyIEntity& e) {
                    auto entity = e.get_entity();
                    return dds::core::polymorphic_cast<PyPublisher>(entity);
                }
            )
        )
        .def_property(
            "qos",
            (const dds::pub::qos::PublisherQos (PyPublisher::*)()const) &PyPublisher::qos,
            (void (PyPublisher::*)(const dds::pub::qos::PublisherQos&)) &PyPublisher::qos,
            "The PublisherQos for this Publisher."
            "\n\n"
            "This property's getter returns a deep copy."
        )
        .def(
            "__lshift__",
            [](PyPublisher& pub, const qos::PublisherQos& qos) {
                return PyPublisher(pub << qos);
            },
            py::is_operator(),
            "Set the PublisherQos."
        )
        .def(
            "__rshift__",
            [](PyPublisher& pub, qos::PublisherQos& qos) {
                return PyPublisher(pub >> qos);
            },
            py::is_operator(),
            "Copy the PublisherQos."
        )
        .def_property(
            "default_datawriter_qos",
            (dds::pub::qos::DataWriterQos (PyPublisher::*)() const) &PyPublisher::default_datawriter_qos,
            [](PyPublisher& pub, qos::DataWriterQos& qos) {
                return PyPublisher(pub.default_datawriter_qos(qos));
            },
            "The default DataWriterQos."
            "\n\n"
            "This property's getter returns a deep copy."
        )
        .def_property_readonly(
            "listener",
            [](const PyPublisher& pub) {
                dds::core::optional<PyPublisherListener*> l;
                auto ptr = dynamic_cast<PyPublisherListener*>(pub.listener());
                if (nullptr != ptr) l = ptr;
                return l;
            },
            "Get the listener."
        )
        .def(
            "bind_listener",
            [](PyPublisher& pub, dds::core::optional<PyPublisherListener*> l, const dds::core::status::StatusMask& m) {
                auto listener = has_value(l) ? get_value(l) : nullptr;
                if (nullptr != listener) {
                    py::cast(listener).inc_ref();
                }
                if (nullptr != pub.listener()) {
                    py::cast(pub.listener()).dec_ref();
                }
                pub.listener(listener, m); 
            },
            py::arg("listener"),
            py::arg("event_mask"),
            "Bind the listener and event mask to the Publisher."
        )
        .def(
            "wait_for_acknowledgments",
            &PyPublisher::wait_for_acknowledgments,
            py::arg("max_wait"),
            "Blocks until all data written by this Publisher's reliable DataWriters is acknowledged or the timeout expires."
        )
        .def_property_readonly(
            "participant",
            [](PyPublisher& pub) {
                auto dp = pub.participant();
                return PyDomainParticipant(dp);
            },
            "Get the parent DomainParticipant of this Publisher."
        )
        .def(
            "wait_for_asynchronous_publishing",
            [](PyPublisher& pub, const dds::core::Duration& max_wait) {
                pub->wait_for_acknowledgments(max_wait);
            },
            py::arg("max_wait"),
            "Blocks until asynchronous sending is complete or timeout expires."
        )
        .def(
            "find_datawriter",
            [](PyPublisher& pub, const std::string& name) {
                auto dw = rti::pub::find_datawriter_by_name<dds::pub::AnyDataWriter>(pub, name);
                return PyAnyDataWriter(dw);
            },
            py::arg("name"),
            "Find a DataWriter in this Publisher by its name."
        )
        .def(
            "find_topic_datawriter",
            [](PyPublisher& pub, const std::string& topic_name) {
                auto dw = rti::pub::find_datawriter_by_topic_name<dds::pub::AnyDataWriter>(pub, topic_name);
                return PyAnyDataWriter(dw);
            },
            py::arg("topic_name"),
            "Find a DataWriter in this Publisher by its topic name. If more than one exists for this Topic, the first one found is returned."
        )
        .def(
            "find_datawriters",
            [](const PyPublisher& pub) {
                std::vector<PyAnyDataWriter> v;
                rti::pub::find_datawriters(pub, std::back_inserter(v));
                return v;
            },
            "Find all DataWriters in the Publisher."
        )
        .def(
            "enable",
            &PyPublisher::enable,
            "Enables this entity (if it was created disabled)."
        )
        .def_property_readonly(
            "status_changes",
            &PyPublisher::status_changes,
            "The list of communication statuses that are triggered."
        )
        .def_property_readonly(
            "instance_handle",
            &PyPublisher::instance_handle,
            "The instance handle that represents this entity."
        )
        .def(
            "close",
            &PyPublisher::close,
            "Forces the destruction of this entity."
        )
        .def(
            "retain",
            &PyPublisher::retain,
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

    py::implicitly_convertible<PyIEntity, PyPublisher>();
}

template<>
void process_inits<Publisher>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class_with_seq<PyPublisher, PyIEntity>(m, "Publisher");
        }
    );   
}

}
