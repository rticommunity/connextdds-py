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
        const dds::core::status::StatusMask& m)
        : dds::pub::Publisher(p, q, l, m)
{
    if (nullptr != l)
        py::cast(l).inc_ref();
}

PyPublisher::~PyPublisher()
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

void PyPublisher::py_close()
{
    if (nullptr != this->listener()) {
        py::object listener = py::cast(this->listener());
        this->listener(nullptr, dds::core::status::StatusMask::none());
        listener.dec_ref();
    }
    this->close();
}

template<>
void init_class_defs(py::class_<PyPublisher, PyIEntity>& cls)
{
    cls

            .def(py::init<const PyDomainParticipant&>(),
                 py::arg("participant"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a publisher.")
            .def(py::init([](const PyDomainParticipant& dp,
                             const qos::PublisherQos& q,
                             dds::core::optional<PyPublisherListener*> l,
                             const dds::core::status::StatusMask& m) {
                     auto listener = has_value(l) ? get_value(l) : nullptr;
                     return PyPublisher(dp, q, listener, m);
                 }),
                 py::arg("participant"),
                 py::arg("qos"),
                 py::arg("listener") = py::none(),
                 py::arg_v(
                         "mask",
                         dds::core::status::StatusMask::all(),
                         "StatusMask.all()"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a Publisher with the desired QoS policies and "
                 "specified listener")
            .def(py::init([](PyIEntity& e) {
                     auto entity = e.get_entity();
                     return dds::core::polymorphic_cast<PyPublisher>(entity);
                 }),
                 py::arg("entity"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Cast an Entity to a Publisher.")
            .def_property(
                    "qos",
                    [](const PyPublisher& pub) {
                        py::gil_scoped_release guard;
                        return pub.qos();
                    },
                    [](PyPublisher& pub, const qos::PublisherQos& qos) {
                        py::gil_scoped_release guard;
                        pub.qos(qos);
                    },
                    "The PublisherQos for this Publisher."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def(
                    "__lshift__",
                    [](PyPublisher& pub, const qos::PublisherQos& qos) -> PyPublisher& {
                        pub << qos;
                        return pub;
                    },
                    py::is_operator(),
                    py::call_guard<py::gil_scoped_release>(),
                    "Set the PublisherQos.")
            .def(
                    "__rshift__",
                    [](PyPublisher& pub, qos::PublisherQos& qos) -> PyPublisher& {
                        pub >> qos;
                        return pub;
                    },
                    py::is_operator(),
                    py::call_guard<py::gil_scoped_release>(),
                    "Copy the PublisherQos.")
            .def_property(
                    "default_datawriter_qos",
                    [](const PyPublisher& pub) {
                        py::gil_scoped_release guard;
                        return pub.default_datawriter_qos();
                    },
                    [](PyPublisher& pub, const qos::DataWriterQos& qos) {
                        py::gil_scoped_release guard;
                        pub.default_datawriter_qos(qos);
                    },
                    "The default DataWriterQos."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property_readonly(
                    "listener",
                    [](const PyPublisher& pub) {
                        py::gil_scoped_release guard;
                        dds::core::optional<PyPublisherListener*> l;
                        auto ptr = dynamic_cast<PyPublisherListener*>(
                                pub.listener());
                        if (nullptr != ptr)
                            l = ptr;
                        return l;
                    },
                    "Get the listener.")
            .def(
                    "bind_listener",
                    [](PyPublisher& pub,
                       dds::core::optional<PyPublisherListener*> l,
                       const dds::core::status::StatusMask& m) {
                        auto listener = has_value(l) ? get_value(l) : nullptr;
                        if (nullptr != listener) {
                            py::cast(listener).inc_ref();
                        }
                        if (nullptr != pub.listener()) {
                            py::cast(pub.listener()).dec_ref();
                        }
                        pub.listener(listener, m);
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    py::arg("listener"),
                    py::arg("event_mask"),
                    "Bind the listener and event mask to the Publisher.")
            .def("wait_for_acknowledgments",
                 &PyPublisher::wait_for_acknowledgments,
                 py::arg("max_wait"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Blocks until all data written by this Publisher's reliable "
                 "DataWriters is acknowledged or the timeout expires.")
            .def_property_readonly(
                    "participant",
                    [](PyPublisher& pub) {
                        py::gil_scoped_release guard;
                        auto dp = pub.participant();
                        return PyDomainParticipant(dp);
                    },
                    "Get the parent DomainParticipant of this Publisher.")
            .def(
                    "wait_for_asynchronous_publishing",
                    [](PyPublisher& pub, const dds::core::Duration& max_wait) {
                        pub->wait_for_acknowledgments(max_wait);
                    },
                    py::arg("max_wait"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Blocks until asynchronous sending is complete or timeout "
                    "expires.")
            .def(
                    "find_datawriter",
                    [](PyPublisher& pub, const std::string& name) {
                        auto dw = rti::pub::find_datawriter_by_name<
                                dds::pub::AnyDataWriter>(pub, name);
                        return PyAnyDataWriter(dw);
                    },
                    py::arg("name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Find a DataWriter in this Publisher by its name.")
            .def(
                    "find_topic_datawriter",
                    [](PyPublisher& pub, const std::string& topic_name) {
                        auto dw = rti::pub::find_datawriter_by_topic_name<
                                dds::pub::AnyDataWriter>(pub, topic_name);
                        return PyAnyDataWriter(dw);
                    },
                    py::arg("topic_name"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Find a DataWriter in this Publisher by its topic name. If "
                    "more than one exists for this Topic, the first one found "
                    "is returned.")
            .def(
                    "find_datawriters",
                    [](const PyPublisher& pub) {
                        std::vector<PyAnyDataWriter> v;
                        rti::pub::find_datawriters(pub, std::back_inserter(v));
                        return v;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Find all DataWriters in the Publisher.")
            .def(
                py::self == py::self,
                py::call_guard<py::gil_scoped_release>(),
                "Test for equality.")
            .def(
                py::self != py::self,
                py::call_guard<py::gil_scoped_release>(),
                "Test for inequality.");

    py::implicitly_convertible<PyIEntity, PyPublisher>();
}

template<>
void process_inits<Publisher>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class_with_seq<PyPublisher, PyIEntity>(m, "Publisher");
    });
}

}  // namespace pyrti
