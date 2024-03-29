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

inline SubscriberListenerPtr get_subscriber_listener(const dds::sub::Subscriber& s) {
    return get_listener<dds::sub::Subscriber, SubscriberListenerPtr>(s);
}

inline void set_subscriber_listener(
        dds::sub::Subscriber& s,
        PySubscriberListenerPtr l) {
    set_listener<dds::sub::Subscriber, PySubscriberListenerPtr>(s, l);
}

inline void set_subscriber_listener(
        dds::sub::Subscriber& s,
        PySubscriberListenerPtr l,
        const dds::core::status::StatusMask& m) {
    set_listener<dds::sub::Subscriber, PySubscriberListenerPtr>(s, l, m);
}

inline PySubscriberListenerPtr downcast_subscriber_listener_ptr(SubscriberListenerPtr l) {
    return downcast_listener_ptr<PySubscriberListenerPtr, SubscriberListenerPtr>(l);
}

PySubscriber::PySubscriber(
        const PyDomainParticipant& p,
        const dds::sub::qos::SubscriberQos& q,
        PySubscriberListenerPtr l,
        const dds::core::status::StatusMask& m)
        : dds::sub::Subscriber(p, q, l, m)
{
    if (nullptr != l) {
        py::gil_scoped_acquire acquire;
        py::cast(l).inc_ref();
    }
}

PySubscriber::~PySubscriber()
{
    if (*this != dds::core::null) {
        if (this->delegate().use_count() <= LISTENER_USE_COUNT_MIN && !this->delegate()->closed()) {
            auto listener_ptr = get_subscriber_listener(*this);
            if (nullptr != listener_ptr) {
                PySubscriberListenerPtr null_listener = nullptr;
                set_subscriber_listener(*this, null_listener, dds::core::status::StatusMask::none());
                {
                    py::gil_scoped_acquire acquire;
                    py::cast(listener_ptr).dec_ref();
                }
            }
        }
    }
}

void PySubscriber::py_close()
{
    auto listener_ptr = get_subscriber_listener(*this);
    if (nullptr != listener_ptr) {
        PySubscriberListenerPtr null_listener = nullptr;
        set_subscriber_listener(*this, null_listener, dds::core::status::StatusMask::none());
        {
            py::gil_scoped_acquire acquire;
            py::cast(listener_ptr).dec_ref();
        }
    }
    this->close();
}


template<>
void init_class_defs(
        py::class_<
            PySubscriber,
            PyIEntity,
            std::unique_ptr<PySubscriber, no_gil_delete<PySubscriber>>>& cls)
{
    cls.def(py::init<const PyDomainParticipant&>(),
               py::arg("participant"),
               py::call_guard<py::gil_scoped_release>(),
               "Create a subscriber under a DomainParticipant.")
            .def(py::init([](const PyDomainParticipant& dp,
                                  const qos::SubscriberQos& q,
                                  PySubscriberListenerPtr listener,
                                  const dds::core::status::StatusMask& m) {
                     return PySubscriber(dp, q, listener, m);
                 }),
                    py::arg("participant"),
                    py::arg("qos"),
                    py::arg("listener") = py::none(),
                    py::arg_v(
                            "mask",
                            dds::core::status::StatusMask::all(),
                            "StatusMask.ALL"),
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
            .def_property(
                    "listener",
                    [](const PySubscriber& sub) {
                        py::gil_scoped_release guard;
                        return downcast_subscriber_listener_ptr(
                                get_subscriber_listener(sub));
                    },
                    [](PySubscriber& sub, PySubscriberListenerPtr listener) {
                        py::gil_scoped_release guard;
                        if (nullptr != listener) {
                            py::gil_scoped_acquire acquire;
                            py::cast(listener).inc_ref();
                        }
                        auto old_listener = get_subscriber_listener(sub);
                        set_subscriber_listener(sub, listener);
                        if (nullptr != old_listener) {
                            py::gil_scoped_acquire acquire;
                            py::cast(old_listener).dec_ref();
                        }
                    },
                    "Get the listener.")
            .def(
                    "set_listener",
                    [](PySubscriber& sub,
                            PySubscriberListenerPtr listener,
                            const dds::core::status::StatusMask& m) {
                        if (nullptr != listener) {
                            py::gil_scoped_acquire acquire;
                            py::cast(listener).inc_ref();
                        }
                        auto old_listener = get_subscriber_listener(sub);
                        set_subscriber_listener(sub, listener, m);
                        if (nullptr != old_listener) {
                            py::gil_scoped_acquire acquire;
                            py::cast(old_listener).dec_ref();
                        }
                    },
                    py::arg("listener"),
                    py::arg("event_mask"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Bind the listener and event mask to the Subscriber.")
            .def_property(
                    "qos",
                    [](const PySubscriber& sub) {
                        py::gil_scoped_release guard;
                        return sub.qos();
                    },
                    [](PySubscriber& sub, const qos::SubscriberQos& qos) {
                        py::gil_scoped_release guard;
                        sub.qos(qos);
                    },
                    "The SubscriberQos for this Subscriber."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property(
                    "default_datareader_qos",
                    [](const PySubscriber& sub) {
                        py::gil_scoped_release guard;
                        return sub.default_datareader_qos();
                    },
                    [](PySubscriber& sub, const qos::DataReaderQos& qos) {
                        py::gil_scoped_release guard;
                        sub.default_datareader_qos(qos);
                    },
                    "The default DataReaderQos."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property_readonly(
                    "participant",
                    [](PySubscriber& sub) {
                        py::gil_scoped_release guard;
                        auto dp = sub.participant();
                        return PyDomainParticipant(dp);
                    },
                    "Get the parent DomainParticipant for this Subscriber.")
            .def(
                    "find_datareader",
                    [](PySubscriber& sub, const std::string& name)
                            -> dds::core::optional<PyAnyDataReader> {
                        auto dr = rti::sub::find_datareader_by_name<
                                dds::sub::AnyDataReader>(sub, name);
                        if (dr == dds::core::null) {
                            return {};
                        }
                        return PyAnyDataReader(dr);
                    },
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Find a DataReader in this Subscriber by its name.")
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
                    "find_datareaders",
                    [](const PySubscriber& sub,
                            const dds::sub::status::DataState& ds) {
                        std::vector<PyAnyDataReader> v;
                        dds::sub::find(sub, ds, std::back_inserter(v));
                        return v;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Find all DataReaders that contain samples of the given "
                    "DataState in the Subscriber.")
            .def(
                    "find_datareaders",
                    [](const PySubscriber& sub, const std::string& topic_name) {
                        std::vector<PyAnyDataReader> v;
                        dds::sub::find<dds::sub::AnyDataReader>(
                                sub,
                                topic_name,
                                std::back_inserter(v));
                        return v;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Find all DataReaders for a given topic name")
            .def(
                    "find_datareader_by_topic_name",
                    [](PySubscriber& sub, const std::string& topic_name)
                            -> dds::core::optional<PyAnyDataReader> {
                        auto dr = rti::sub::find_datareader_by_topic_name<
                                dds::sub::AnyDataReader>(sub, topic_name);
                        if (dr == dds::core::null) {
                            return {};
                        }
                        return PyAnyDataReader(dr);
                    },
                    py::arg("topic_name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Find a DataReader in this Subscriber by its topic name. "
                    "If "
                    "more than one exists for this Topic, the first one found "
                    "is returned.")
            .def(py::self == py::self,
                    py::call_guard<py::gil_scoped_release>(),
                    "Test for equality.")
            .def(py::self != py::self,
                    py::call_guard<py::gil_scoped_release>(),
                    "Test for inequality.");

    py::implicitly_convertible<PyIEntity, PySubscriber>();
}

template<>
void process_inits<Subscriber>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class_with_seq<
            PySubscriber,
            PyIEntity,
            std::unique_ptr<PySubscriber, no_gil_delete<PySubscriber>>>(
                m,
                "Subscriber");
    });
}

}  // namespace pyrti
