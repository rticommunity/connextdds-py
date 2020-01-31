#include "PyConnext.hpp"
#include <dds/sub/Subscriber.hpp>
#include <dds/sub/find.hpp>
#include <rti/rti.hpp>
#include "PyEntity.hpp"
#include "PySubscriberListener.hpp"

using namespace dds::sub;

namespace pyrti {

template<>
void init_class_defs(py::class_<PySubscriber>& cls) {
    cls
        .def(
            py::init<
                const PyDomainParticipant&
            >(),
            py::arg("participant"),
            "Create a subscriber under a DomainParticipant."
        )
        .def(
            py::init<
                const PyDomainParticipant&,
                const qos::SubscriberQos&,
                PySubscriberListener*,
                const dds::core::status::StatusMask&
            >(),
            py::arg("participant"),
            py::arg("qos"),
            py::arg("listener") = (PySubscriberListener*) nullptr,
            py::arg_v("mask", dds::core::status::StatusMask::all(), "StatusMask.all"),
            py::keep_alive<1,4>(),
            "Create a Subscriber under a DomainParticipant with a listener."
        )
        .def(
            py::init(
                [](PyIEntity& e) {
                    auto entity = e.get_entity();
                    return dds::core::polymorphic_cast<PySubscriber>(entity);
                }
            )
        )
        .def(
            "notify_datareaders",
            &PySubscriber::notify_datareaders,
            "This operation invokes the operation on_data_available on the DataReaderListener objects attached to contained DataReader entities with a DATA_AVAILABLE status that is considered changed"
        )
        .def_property_readonly(
            "listener",
            [](const PySubscriber& sub) {
                return dynamic_cast<PySubscriberListener*>(sub.listener());
            },
            "Get the listener."
        )
        .def(
            "bind_listener",
            [](PySubscriber& sub, PySubscriberListener* l, const dds::core::status::StatusMask& m) {
                sub.listener(l, m);
            },
            py::arg("listener"),
            py::arg("event_mask"),
            py::keep_alive<1, 2>(),
            "Bind the listener and event mask to the Subscriber."
        )
        .def_property(
            "qos",
            (qos::SubscriberQos (PySubscriber::*)() const) &PySubscriber::qos,
            (void (PySubscriber::*)(const qos::SubscriberQos&)) &PySubscriber::qos,
            "Get a copy of or set the SubscriberQos."
        )
        .def_property(
            "default_datareader_qos",
            (qos::DataReaderQos (PySubscriber::*)() const) &PySubscriber::default_datareader_qos,
            [](PySubscriber& sub, const qos::DataReaderQos& qos) {
                return PySubscriber(sub.default_datareader_qos(qos));
            },
            "Get a copy of or set the default DataReaderQos."
        )
        .def_property_readonly(
            "participant",
            &PySubscriber::participant,
            "Get the parent DomainParticipant for this Subscriber."
        )
        .def(
            "find_datareaders",
            [](const PySubscriber& sub) {
                std::vector<PyAnyDataReader> v;
                rti::sub::find_datareaders(sub, std::back_inserter(v));
                return v;
            },
            "Find all DataReaders in the Subscriber."
        )
        .def(
            "enable",
            &PySubscriber::enable,
            "Enables this entity (if it was created disabled)."
        )
        .def_property_readonly(
            "status_changes",
            &PySubscriber::status_changes,
            "The list of communication statuses that are triggered."
        )
        .def_property_readonly(
            "instance_handle",
            &PySubscriber::instance_handle,
            "The instance handle that represents this entity."
        )
        .def(
            "close",
            &PySubscriber::close,
            "Forces the destruction of this entity."
        )
        .def(
            "retain",
            &PySubscriber::retain,
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

    py::implicitly_convertible<PyIEntity, PySubscriber>();
}

template<>
void process_inits<Subscriber>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<PySubscriber>(m, "Subscriber");
        }
    );
}

}
