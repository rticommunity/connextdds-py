#pragma once

#include "PyConnext.hpp"
#include <pybind11/stl_bind.h>
#include <dds/topic/Topic.hpp>
#include <dds/topic/find.hpp>
#include "PyEntity.hpp"
#include "PyDynamicTypeMap.hpp"
#include "PyTopicListener.hpp"
#include "PyAnyTopic.hpp"

namespace pyrti {

template<typename T>
class PyTopicListener;

template<typename T>
class PyITopicDescription : public PyIEntity {
public:
    virtual dds::topic::TopicDescription<T> get_topic_description() = 0;

    virtual const std::string py_name() const = 0;

    virtual const std::string py_type_name() const = 0;

    virtual const PyDomainParticipant py_participant() const = 0;
};

template<typename T>
class PyTopicDescription : public dds::topic::TopicDescription<T>,
                           public PyITopicDescription<T> {
public:
    using dds::topic::TopicDescription<T>::TopicDescription;

    dds::topic::TopicDescription<T> get_topic_description() override
    {
        return *this;
    }

    const std::string py_name() const override
    {
        return this->name();
    }

    const std::string py_type_name() const override
    {
        return this->type_name();
    }

    const PyDomainParticipant py_participant() const override
    {
        dds::domain::DomainParticipant dp(this->participant());
        return PyDomainParticipant(dp);
    }

    dds::core::Entity get_entity() override
    {
        return *this;
    }

    void py_enable() override
    {
        this->enable();
    }

    const dds::core::status::StatusMask py_status_changes() override
    {
        return this->status_changes();
    }

    const dds::core::InstanceHandle py_instance_handle() const override
    {
        return this->instance_handle();
    }

    void py_close() override
    {
        this->close();
    }

    void py_retain() override
    {
        this->retain();
    }

    bool py_closed() override
    {
        return this->delegate()->closed();
    }

    bool py_enabled() override
    {
        return this->delegate()->enabled();
    }

    int py_use_count() override
    {
        return this->delegate().use_count();
    }

    void py_unretain() override
    {
        this->delegate()->unretain();
    }
};

template<typename T>
class PyTopic : public dds::topic::Topic<T>,
                public PyITopicDescription<T>,
                public PyIAnyTopic {
public:
    using dds::topic::Topic<T>::Topic;

    PyTopic(const PyDomainParticipant& dp,
            const std::string& n,
            const dds::topic::qos::TopicQos& q,
            PyTopicListener<T>* l,
            const dds::core::status::StatusMask& m)
            : dds::topic::Topic<T>(dp, n, q, l, m)
    {
        if (nullptr != l)
            py::cast(l).inc_ref();
    }

    PyTopic(const PyDomainParticipant& dp,
            const std::string& n,
            const std::string& tn,
            const dds::topic::qos::TopicQos& q,
            PyTopicListener<T>* l,
            const dds::core::status::StatusMask& m)
            : dds::topic::Topic<T>(dp, n, tn, q, l, m)
    {
        if (nullptr != l)
            py::cast(l).inc_ref();
    }

    virtual ~PyTopic()
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

    dds::topic::TopicDescription<T> get_topic_description() override
    {
        return dds::topic::TopicDescription<T>(*this);
    }

    const std::string py_name() const override
    {
        return this->name();
    }

    const std::string py_type_name() const override
    {
        return this->type_name();
    }

    const PyDomainParticipant py_participant() const override
    {
        dds::domain::DomainParticipant dp(this->py_domain_participant());
        return PyDomainParticipant(dp);
    }

    dds::core::Entity get_entity() override
    {
        return *this;
    }

    void py_enable() override
    {
        this->enable();
    }

    const dds::core::status::StatusMask py_status_changes() override
    {
        return this->status_changes();
    }

    const dds::core::InstanceHandle py_instance_handle() const override
    {
        return this->instance_handle();
    }

    void py_close() override
    {
        if (nullptr != this->listener()) {
            py::object listener = py::cast(this->listener());
            this->listener(nullptr, dds::core::status::StatusMask::none());
            listener.dec_ref();
        }
        this->close();
    }

    void py_retain() override
    {
        this->retain();
    }

    bool py_closed() override
    {
        return this->delegate()->closed();
    }

    bool py_enabled() override
    {
        return this->delegate()->enabled();
    }

    int py_use_count() override
    {
        return this->delegate().use_count();
    }

    void py_unretain() override
    {
        this->delegate()->unretain();
    }

    dds::topic::AnyTopic get_any_topic() const override
    {
        return dds::topic::AnyTopic(*this);
    }

    PyDomainParticipant py_domain_participant() const override
    {
        return this->py_participant();
    }

    dds::core::status::InconsistentTopicStatus py_inconsistent_topic_status()
            override
    {
        return this->inconsistent_topic_status();
    }

    dds::topic::qos::TopicQos py_qos() const override
    {
        return this->qos();
    }

    void py_qos(const dds::topic::qos::TopicQos& q) override
    {
        this->qos(q);
    }
};

template<typename T>
void init_itopic_description_defs(
        py::class_<PyITopicDescription<T>, PyIEntity>& cls)
{
    cls.def_property_readonly(
               "name",
               &PyITopicDescription<T>::py_name,
               "The name of the entity conforming to the ITopicDescription "
               "interface.")
            .def_property_readonly(
                    "type_name",
                    &PyITopicDescription<T>::py_type_name,
                    "The name of the associated type.")
            .def_property_readonly(
                    "participant",
                    &PyITopicDescription<T>::py_participant,
                    "The parent DomainParticipant.");
}

template<typename T>
void init_topic_description_defs(
        py::class_<PyTopicDescription<T>, PyITopicDescription<T>>& cls)
{
    cls.def(py::init([](PyITopicDescription<T>& t) {
                return PyTopicDescription<T>(t.get_topic_description());
            }),
            py::arg("topic"),
            "Cast a Topic to a TopicDescription.")
            .def(py::init([](PyIEntity& t) {
                     auto entity = t.get_entity();
                     return dds::core::polymorphic_cast<PyTopicDescription<T>>(
                             entity);
                 }),
                 py::arg("entity"),
                 "Cast a Topic to a TopicDescription.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");

    py::implicitly_convertible<PyITopicDescription<T>, PyTopicDescription<T>>();
    py::implicitly_convertible<PyIEntity, PyTopicDescription<T>>();
}

template<typename T>
void init_dds_typed_topic_base_template(
        py::class_<PyTopic<T>, PyITopicDescription<T>, PyIAnyTopic>& cls)
{
    cls.def(py::init([](PyIEntity& e) {
                auto entity = e.get_entity();
                return dds::core::polymorphic_cast<PyTopic<T>>(entity);
            }),
            py::arg("entity"),
            py::call_guard<py::gil_scoped_release>(),
            "Cast an Entity to a Topic.")
            .def(py::init([](PyITopicDescription<T>& t) {
                     auto td = t.get_topic_description();
                     return dds::core::polymorphic_cast<PyTopic<T>>(td);
                 }),
                 py::arg("topic_description"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Cast an ITopicDescription to a Topic.")
            .def(py::init([](PyIAnyTopic& at) {
                     auto topic = at.get_any_topic().get<T>();
                     return PyTopic<T>(topic);
                 }),
                 py::arg("topic"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a typed Topic from an AnyTopic.")
            .def_property_readonly(
                    "listener",
                    [](PyTopic<T>& t) {
                        dds::core::optional<PyTopicListener<T>*> l;
                        auto ptr =
                                dynamic_cast<PyTopicListener<T>*>(t.listener());
                        if (nullptr != ptr)
                            l = ptr;
                        return l;
                    },
                    "The listener.")
            .def(
                    "bind_listener",
                    [](PyTopic<T>& t,
                       dds::core::optional<PyTopicListener<T>*> l,
                       const dds::core::status::StatusMask& m) {
                        auto listener = has_value(l) ? get_value(l) : nullptr;
                        if (nullptr != listener) {
                            py::cast(listener).inc_ref();
                        }
                        if (nullptr != t.listener()) {
                            py::cast(t.listener()).dec_ref();
                        }
                        t.listener(listener, m);
                    },
                    py::arg("listener"),
                    py::arg("event_mask"),
                    "Set the listener and event mask.")
            .def_property(
                    "qos",
                    (dds::topic::qos::TopicQos(PyTopic<T>::*)() const)
                            & PyTopic<T>::qos,
                    (void (PyTopic<T>::*)(const dds::topic::qos::TopicQos&))
                            & PyTopic<T>::qos,
                    "Get the TopicQos for this Topic."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def_property_readonly(
                    "inconsistent_topic_status",
                    &PyTopic<T>::inconsistent_topic_status,
                    "Get a copy of the current InconsistentTopicStatus for "
                    "this Topic.")
            .def_static(
                    "find",
                    [](PyDomainParticipant& dp, const std::string& name) {
                        dds::core::optional<PyTopic<T>> retval;
                        auto t = dds::topic::find<dds::topic::Topic<T>>(
                                dp,
                                name);
                        if (t != dds::core::null)
                            retval = PyTopic<T>(t);
                        return retval;
                    },
                    py::arg("participant"),
                    py::arg("name"),
                    "Look up a Topic by its name in the DomainParticipant.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");

    py::implicitly_convertible<PyIAnyTopic, PyTopic<T>>();
    py::implicitly_convertible<PyIEntity, PyTopic<T>>();
    py::implicitly_convertible<PyITopicDescription<T>, PyTopic<T>>();
}

template<typename T>
void init_dds_typed_topic_template(
        py::class_<PyTopic<T>, PyITopicDescription<T>, PyIAnyTopic>& cls)
{
    init_dds_typed_topic_base_template<T>(cls);

    cls.def(py::init<const PyDomainParticipant&, const std::string&>(),
            py::arg("participant"),
            py::arg("topic_name"),
            py::call_guard<py::gil_scoped_release>(),
            "Creates a new Topic.")
            .def(py::init<
                         const PyDomainParticipant&,
                         const std::string&,
                         const std::string&>(),
                 py::arg("participant"),
                 py::arg("topic_name"),
                 py::arg("type_name"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Creates a new Topic.")
            .def(py::init([](const PyDomainParticipant& dp,
                             const std::string& n,
                             const dds::topic::qos::TopicQos& q,
                             dds::core::optional<PyTopicListener<T>*> l,
                             const dds::core::status::StatusMask& m) {
                     auto listener = has_value(l) ? get_value(l) : nullptr;
                     return PyTopic<T>(dp, n, q, listener, m);
                 }),
                 py::arg("participant"),
                 py::arg("topic_name"),
                 py::arg("qos"),
                 py::arg("listener") = py::none(),
                 py::arg_v(
                         "mask",
                         dds::core::status::StatusMask::all(),
                         "StatusMask.all()"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Creates a new Topic.")
            .def(py::init([](const PyDomainParticipant& dp,
                             const std::string& n,
                             const std::string& t,
                             const dds::topic::qos::TopicQos& q,
                             dds::core::optional<PyTopicListener<T>*> l,
                             const dds::core::status::StatusMask& m) {
                     auto listener = has_value(l) ? get_value(l) : nullptr;
                     return PyTopic<T>(dp, n, t, q, listener, m);
                 }),
                 py::arg("participant"),
                 py::arg("topic_name"),
                 py::arg("type_name"),
                 py::arg("qos"),
                 py::arg("listener") = py::none(),
                 py::arg_v(
                         "mask",
                         dds::core::status::StatusMask::all(),
                         "StatusMask.all()"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Creates a new Topic.");
}

template<typename T>
void init_topic(
        py::class_<PyITopicDescription<T>, PyIEntity>& itd,
        py::class_<PyTopicDescription<T>, PyITopicDescription<T>>& td,
        py::class_<PyTopic<T>, PyITopicDescription<T>, PyIAnyTopic>& t)
{
    init_itopic_description_defs(itd);
    init_topic_description_defs(td);
    init_dds_typed_topic_template(t);
}

}  // namespace pyrti
