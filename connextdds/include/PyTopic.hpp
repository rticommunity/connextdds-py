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
    class PyITopicDescription : public pyrti::PyIEntity {
    public:
        virtual
        dds::topic::TopicDescription<T> get_topic_description() = 0;

        virtual
        const std::string py_name() const = 0;

        virtual
        const std::string py_type_name() const = 0;

        virtual
        const pyrti::PyDomainParticipant py_participant() const = 0;
    };

    template<typename T>
    class PyTopicDescription :  public dds::topic::TopicDescription<T>,
                                public pyrti::PyITopicDescription<T> {
    public:
        using dds::topic::TopicDescription<T>::TopicDescription;

        dds::topic::TopicDescription<T> get_topic_description() override {
            return *this;
        }

        const std::string py_name() const override {
            return this->name();
        }

        const std::string py_type_name() const override {
            return this->type_name();
        }

        const pyrti::PyDomainParticipant py_participant() const override {
            dds::domain::DomainParticipant dp(this->participant());
            return pyrti::PyDomainParticipant(dp);
        }

        dds::core::Entity get_entity() override { return *this; }

        void py_enable() override { this->enable(); }

        const dds::core::status::StatusMask py_status_changes() override { return this->status_changes(); }

        const dds::core::InstanceHandle py_instance_handle() const override { return this->instance_handle(); }

        void py_close() override { this->close(); }

        void py_retain() override { this->retain(); }
    };

    template<typename T>
    class PyTopic : public dds::topic::Topic<T>,
                    public pyrti::PyITopicDescription<T>,
                    public pyrti::PyIAnyTopic {
    public:
        using dds::topic::Topic<T>::Topic;

        /*PyTopic(const pyrti::PyDomainParticipant& dp, const std::string& n) : dds::topic::Topic<T>(dp, n) {}

        PyTopic(
            const pyrti::PyDomainParticipant& dp, 
            const std::string& n,
            const std::string& tn
        ) : dds::topic::Topic<T>(dp, n, tn) {}

        PyTopic(
            const pyrti::PyDomainParticipant& dp,
            const std::string& n,
            const dds::topic::qos::TopicQos& q,
            pyrti::PyTopicListener<T>* l,
            const dds::core::status::StatusMask& m
        ) : dds::topic::Topic<T>(dp, n, q, l, m) {}

        PyTopic(
            const pyrti::PyDomainParticipant& dp,
            const std::string& n,
            const std::string& tn,
            const dds::topic::qos::TopicQos& q,
            pyrti::PyTopicListener<T>* l,
            const dds::core::status::StatusMask& m
        ) : dds::topic::Topic<T>(dp, n, tn, q, l, m) {} */

        dds::topic::TopicDescription<T> get_topic_description() override {
            return dds::topic::TopicDescription<T>(*this);
        }

        const std::string py_name() const override {
            return this->name();
        }

        const std::string py_type_name() const override {
            return this->type_name();
        }

        const pyrti::PyDomainParticipant py_participant() const override {
            dds::domain::DomainParticipant dp(this->py_domain_participant());
            return pyrti::PyDomainParticipant(dp);
        }

        dds::core::Entity get_entity() override { return *this; }

        void py_enable() override { this->enable(); }

        const dds::core::status::StatusMask py_status_changes() override { return this->status_changes(); }

        const dds::core::InstanceHandle py_instance_handle() const override { return this->instance_handle(); }

        void py_close() override { this->close(); }

        void py_retain() override { this->retain(); }

        dds::topic::AnyTopic get_any_topic() const override {
            return dds::topic::AnyTopic(*this);
        }

        pyrti::PyDomainParticipant py_domain_participant() const override {
            return this->py_participant();
        }

        dds::core::status::InconsistentTopicStatus py_inconsistent_topic_status() override {
            return this->inconsistent_topic_status();
        }

        dds::topic::qos::TopicQos py_qos() const override {
            return this->qos();
        }

        void py_qos(const dds::topic::qos::TopicQos& q) override {
            this->qos(q);
        }
    };

    template<typename T>
    void init_itopic_description_defs(py::class_<pyrti::PyITopicDescription<T>, pyrti::PyIEntity>& cls) {
        cls
            .def(
                "name",
                &pyrti::PyITopicDescription<T>::py_name,
                "The name of the entity conforming to the ITopicDescription "
                "interface."
            )
            .def(
                "type_name",
                &pyrti::PyITopicDescription<T>::py_type_name,
                "The name of the associated type."
            )
            .def(
                "participant",
                &pyrti::PyITopicDescription<T>::py_participant,
                "The parent DomainParticipant."
            );
    }

    template<typename T>
    void init_topic_description_defs(py::class_<pyrti::PyTopicDescription<T>, pyrti::PyITopicDescription<T>>& cls) {
        cls
            .def(
                py::init(
                    [](pyrti::PyITopicDescription<T>& t) {
                        return pyrti::PyTopicDescription<T>(t.get_topic_description());
                    }
                ),
                py::arg("topic"),
                "Cast a Topic to a TopicDescription."
            )
            .def(
                py::init(
                    [](pyrti::PyIEntity& t) {
                        auto entity = t.get_entity();
                        return dds::core::polymorphic_cast<pyrti::PyTopicDescription<T>>(entity);
                    }
                ),
                py::arg("entity"),
                "Cast a Topic to a TopicDescription."
            )
            .def(
                py::self == py::self,
                "Test for equality."
            )
            .def(
                py::self != py::self,
                "Test for inequality."
            );

        py::implicitly_convertible<pyrti::PyITopicDescription<T>, pyrti::PyTopicDescription<T>>();
        py::implicitly_convertible<pyrti::PyIEntity, pyrti::PyTopicDescription<T>>();
    }

    template<typename T>
    void init_dds_typed_topic_base_template(py::class_<pyrti::PyTopic<T>, pyrti::PyITopicDescription<T>, pyrti::PyIAnyTopic>& cls) {
        cls
            .def(
                py::init(
                    [](pyrti::PyIEntity& e) {
                        auto entity = e.get_entity();
                        return dds::core::polymorphic_cast<pyrti::PyTopic<T>>(entity);
                    }
                ),
                "Cast an Entity to a Topic."
            )
            .def(
                py::init(
                    [](pyrti::PyITopicDescription<T>& t) {
                        auto td = t.get_topic_description();
                        return dds::core::polymorphic_cast<pyrti::PyTopic<T>>(td);
                    }
                ),
                "Cast an ITopicDescription to a Topic."
            )
            .def(
                py::init([](pyrti::PyIAnyTopic& at) {
                    auto topic = at.get_any_topic().get<T>();
                    return pyrti::PyTopic<T>(topic);
                }),
                py::arg("topic"),
                "Create a typed Topic from an AnyTopic."
            )
            .def_property_readonly(
                "listener",
                [](pyrti::PyTopic<T>& t) {
                    return dynamic_cast<pyrti::PyTopicListener<T>*>(t.listener());
                },
                py::keep_alive<1,2>(),
                "The listener."
            )
            .def(
                "bind_listener",
                [](pyrti::PyTopic<T>& t, pyrti::PyTopicListener<T>* l, const dds::core::status::StatusMask& m) {
                    t.listener(l, m);
                },
                py::arg("listener"),
                py::arg("event_mask"),
                py::keep_alive<1, 2>(),
                "Set the listener and event mask."
            )
            .def_property(
                "qos",
                (dds::topic::qos::TopicQos (pyrti::PyTopic<T>::*)() const) &pyrti::PyTopic<T>::qos,
                (void (pyrti::PyTopic<T>::*)(const dds::topic::qos::TopicQos&)) &pyrti::PyTopic<T>::qos,
                "Get a copy of or set the TopicQos."
            )
            .def_property_readonly(
                "inconsistent_topic_status",
                &pyrti::PyTopic<T>::inconsistent_topic_status,
                "Get a copy of the current InconsistentTopicStatus for this Topic."
            )
            .def_static(
                "find",
                [](pyrti::PyDomainParticipant& dp, const std::string& name) -> py::object {
                    auto t = dds::topic::find<dds::topic::Topic<T>>(dp, name);
                    return (t == dds::core::null) ? py::cast(nullptr) : py::cast(pyrti::PyTopic<T>(t));
                },
                py::arg("participant"),
                py::arg("name"),
                "Look up a Topic by its name in the DomainParticipant."
            )
            .def(
                py::self == py::self,
                "Test for equality."
            )
            .def(
                py::self != py::self,
                "Test for inequality."
            );

        py::implicitly_convertible<pyrti::PyIAnyTopic, pyrti::PyTopic<T>>();
        py::implicitly_convertible<pyrti::PyIEntity, pyrti::PyTopic<T>>();
        py::implicitly_convertible<pyrti::PyITopicDescription<T>, pyrti::PyTopic<T>>();

        py::bind_vector<std::vector<pyrti::PyTopic<T>>>(cls, "Seq");
        py::implicitly_convertible<py::iterable, std::vector<pyrti::PyTopic<T>>>();
    }

    template<typename T>
    void init_dds_typed_topic_template(py::class_<pyrti::PyTopic<T>, pyrti::PyITopicDescription<T>, pyrti::PyIAnyTopic>& cls) {
        init_dds_typed_topic_base_template<T>(cls);

        cls
            .def(
                py::init<
                    const pyrti::PyDomainParticipant&,
                    const std::string&
                >(),
                py::arg("participant"),
                py::arg("topic_name"),
                "Creates a new Topic."
            )
            .def(
                py::init<
                    const pyrti::PyDomainParticipant&,
                    const std::string&,
                    const std::string&
                >(),
                py::arg("participant"),
                py::arg("topic_name"),
                py::arg("type_name"),
                "Creates a new Topic."
            )
            .def(
                py::init<
                    const pyrti::PyDomainParticipant&,
                    const std::string&,
                    const dds::topic::qos::TopicQos&,
                    pyrti::PyTopicListener<T>*,
                    const dds::core::status::StatusMask&
                >(),
                py::arg("participant"),
                py::arg("topic_name"),
                py::arg("qos"),
                py::arg("listener") = (pyrti::PyTopicListener<T>*) nullptr,
                py::arg("mask") = dds::core::status::StatusMask::all(),
                "Creates a new Topic."
            )
            .def(
                py::init<
                    const pyrti::PyDomainParticipant&,
                    const std::string&,
                    const std::string&,
                    const dds::topic::qos::TopicQos&,
                    pyrti::PyTopicListener<T>*,
                    const dds::core::status::StatusMask&
                >(),
                py::arg("participant"),
                py::arg("topic_name"),
                py::arg("type_name"),
                py::arg("qos"),
                py::arg("listener") = (pyrti::PyTopicListener<T>*) nullptr,
                py::arg("mask") = dds::core::status::StatusMask::all(),
                "Creates a new Topic."
            );
    }

    template<typename T>
    void init_topic(py::object& o) {
        py::class_<pyrti::PyITopicDescription<T>, pyrti::PyIEntity> itd(o, "ITopicDescription");
        py::class_<pyrti::PyTopicDescription<T>, pyrti::PyITopicDescription<T>> td(o, "TopicDescription");
        py::class_<pyrti::PyTopic<T>, pyrti::PyITopicDescription<T>, pyrti::PyIAnyTopic> t(o, "Topic");

        pyrti::init_itopic_description_defs(itd);
        pyrti::init_topic_description_defs(td);
        pyrti::init_dds_typed_topic_template(t);
    }
}