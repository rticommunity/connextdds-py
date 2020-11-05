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
#include <dds/sub/Subscriber.hpp>
#include <dds/sub/find.hpp>
#include <rti/rti.hpp>
#include "PyEntity.hpp"
#include "PySubscriberListener.hpp"

using namespace dds::sub;

namespace pyrti {

PySubscriber::PySubscriber(
        const PyDomainParticipant& p,
        const dds::sub::qos::SubscriberQos& q,
        PySubscriberListener* l,
        const dds::core::status::StatusMask& m)
        : dds::sub::Subscriber(p, q, l, m)
{
    if (nullptr != l)
        py::cast(l).inc_ref();
}

PySubscriber::~PySubscriber()
{
    if (*this != dds::core::null) {
        if (this->delegate().use_count() <= 2 && !this->delegate()->closed()
            && nullptr != this->listener()) {
            py::object listener = py::cast(this->listener());
            this->listener(nullptr, dds::core::status::StatusMask::none());
            listener.dec_ref();
        }
    }
}

void PySubscriber::py_close()
{
    if (nullptr != this->listener()) {
        py::object listener = py::cast(this->listener());
        this->listener(nullptr, dds::core::status::StatusMask::none());
        listener.dec_ref();
    }
    this->close();
}

template<>
void init_class_defs(py::class_<PySubscriber, PyIEntity>& cls)
{
    cls.def(py::init<const PyDomainParticipant&>(),
            py::arg("participant"),
            py::call_guard<py::gil_scoped_release>(),
            "Create a subscriber under a DomainParticipant.")
            .def(py::init([](const PyDomainParticipant& dp,
                             const qos::SubscriberQos& q,
                             dds::core::optional<PySubscriberListener*> l,
                             const dds::core::status::StatusMask& m) {
                     auto listener = has_value(l) ? get_value(l) : nullptr;
                     return PySubscriber(dp, q, listener, m);
                 }),
                 py::arg("participant"),
                 py::arg("qos"),
                 py::arg("listener") = py::none(),
                 py::arg_v(
                         "mask",
                         dds::core::status::StatusMask::all(),
                         "StatusMask.all()"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a Subscriber under a DomainParticipant with a "
                 "listener.")
            .def(py::init([](PyIEntity& e) {
                     auto entity = e.get_entity();
                     return dds::core::polymorphic_cast<PySubscriber>(entity);
                 }),
                 py::arg("entity"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Cast an Entity to a Subscriber.")
            .def("notify_datareaders",
                 &PySubscriber::notify_datareaders,
                 py::call_guard<py::gil_scoped_release>(),
                 "This operation invokes the operation on_data_available on "
                 "the DataReaderListener objects attached to contained "
                 "DataReader entities with a DATA_AVAILABLE status that is "
                 "considered changed")
            .def_property_readonly(
                    "listener",
                    [](const PySubscriber& sub) {
                        dds::core::optional<PySubscriberListener*> l;
                        auto ptr = dynamic_cast<PySubscriberListener*>(
                                sub.listener());
                        if (nullptr != ptr)
                            l = ptr;
                        return l;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the listener.")
            .def(
                    "bind_listener",
                    [](PySubscriber& sub,
                       dds::core::optional<PySubscriberListener*> l,
                       const dds::core::status::StatusMask& m) {
                        auto listener = has_value(l) ? get_value(l) : nullptr;
                        if (nullptr != listener) {
                            py::cast(listener).inc_ref();
                        }
                        if (nullptr != sub.listener()) {
                            py::cast(sub.listener()).dec_ref();
                        }
                        sub.listener(listener, m);
                    },
                    py::arg("listener"),
                    py::arg("event_mask"),
                    "Bind the listener and event mask to the Subscriber.")
            .def_property(
                    "qos",
                    (qos::SubscriberQos(PySubscriber::*)() const)
                            & PySubscriber::qos,
                    (void (PySubscriber::*)(const qos::SubscriberQos&))
                            & PySubscriber::qos,
                    py::call_guard<py::gil_scoped_release>(),
                    "The SubscriberQos for this Subscriber."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property(
                    "default_datareader_qos",
                    (qos::DataReaderQos(PySubscriber::*)() const)
                            & PySubscriber::default_datareader_qos,
                    [](PySubscriber& sub, const qos::DataReaderQos& qos) {
                        return PySubscriber(sub.default_datareader_qos(qos));
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "The default DataReaderQos."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property_readonly(
                    "participant",
                    [](PySubscriber& sub) {
                        auto dp = sub.participant();
                        return PyDomainParticipant(dp);
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Get the parent DomainParticipant for this Subscriber.")
            .def(
                    "find_datareaders",
                    [](PySubscriber& sub) {
                        std::vector<PyAnyDataReader> v;
                        rti::sub::find_datareaders(sub, std::back_inserter(v));
                        return v;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Find all DataReaders in the Subscriber.")
            .def(
                py::self == py::self,
                py::call_guard<py::gil_scoped_release>(),
                "Test for equality.")
            .def(
                py::self != py::self,
                py::call_guard<py::gil_scoped_release>(),
                "Test for inequality.");

    py::implicitly_convertible<PyIEntity, PySubscriber>();
}

template<>
void process_inits<Subscriber>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class_with_seq<PySubscriber, PyIEntity>(m, "Subscriber");
    });
}

}  // namespace pyrti
