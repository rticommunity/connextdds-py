#pragma once

#include "PyConnext.hpp"
#include <dds/core/Entity.hpp>
#include <dds/domain/DomainParticipant.hpp>
#include <dds/pub/Publisher.hpp>
#include <dds/sub/Subscriber.hpp>

namespace pyrti {

class PyDomainParticipantListener;
class PyPublisherListener;
class PySubscriberListener;

class PyIEntity {
public:
    virtual
    dds::core::Entity get_entity() = 0;

    virtual
    void py_enable() = 0;

    virtual 
    const dds::core::status::StatusMask py_status_changes() = 0;

    virtual
    const dds::core::InstanceHandle py_instance_handle() const = 0;

    virtual
    void py_close() = 0;

    virtual
    void py_retain() = 0;

    virtual
    ~PyIEntity() {}
};

class PyEntity : public dds::core::Entity, public PyIEntity {
public:
    using dds::core::Entity::Entity;

    PyEntity(const dds::core::Entity& e) : dds::core::Entity(e) {}

    dds::core::Entity get_entity() override {
        return dds::core::Entity(*this);
    }

    void py_enable() override { this->enable(); };

    const dds::core::status::StatusMask py_status_changes() override { return this->status_changes(); }

    const dds::core::InstanceHandle py_instance_handle() const override { return this->instance_handle(); }

    void py_close() override { this->close(); }

    void py_retain() override { this->retain(); }

    virtual
    ~PyEntity() {}
};

class PyDomainParticipant : public dds::domain::DomainParticipant, public PyIEntity {
public:
    using dds::domain::DomainParticipant::DomainParticipant;

    PyDomainParticipant& operator=(const dds::domain::DomainParticipant& dp) {
        dds::domain::DomainParticipant::operator=(dp);
        return *this;
    }

    PyDomainParticipant(
        int32_t d,
        const dds::domain::qos::DomainParticipantQos& q,
        PyDomainParticipantListener* l,
        const dds::core::status::StatusMask& m);

    dds::core::Entity get_entity() override {
        return dds::core::Entity(*this);
    }

    void py_enable() override { this->enable(); };

    const dds::core::status::StatusMask py_status_changes() override { return this->status_changes(); }

    const dds::core::InstanceHandle py_instance_handle() const override { return this->instance_handle(); }

    void py_close() override;

    void py_retain() override { this->retain(); }

    void py_add_prop(pybind11::object o) {
        this->_properties.insert(o);
    }

    virtual
    ~PyDomainParticipant();

private:
    struct PropertyHasher {
        size_t operator()(const pybind11::object& o) const {
            return reinterpret_cast<std::uintptr_t>(o.ptr());
        }
    };

    std::unordered_set<pybind11::object, PropertyHasher> _properties;
};

class PyPublisher : public dds::pub::Publisher, public PyIEntity {
public:
    using dds::pub::Publisher::Publisher;

    PyPublisher(const dds::pub::Publisher& pub) : dds::pub::Publisher(pub) {}

    PyPublisher(
        const PyDomainParticipant& p,
        const dds::pub::qos::PublisherQos& q,
        PyPublisherListener* l,
        const dds::core::status::StatusMask& m);

    dds::core::Entity get_entity() override {
        return dds::core::Entity(*this);
    }

    void py_enable() override { this->enable(); };

    const dds::core::status::StatusMask py_status_changes() override { return this->status_changes(); }

    const dds::core::InstanceHandle py_instance_handle() const override { return this->instance_handle(); }

    void py_close() override;

    void py_retain() override { this->retain(); }

    virtual
    ~PyPublisher();
};

class PySubscriber : public dds::sub::Subscriber, public PyIEntity {
public:
    using dds::sub::Subscriber::Subscriber;

    PySubscriber(const dds::sub::Subscriber& sub) : dds::sub::Subscriber(sub) {}

    PySubscriber(
        const PyDomainParticipant& p,
        const dds::sub::qos::SubscriberQos& q,
        PySubscriberListener* l,
        const dds::core::status::StatusMask& m);

    dds::core::Entity get_entity() override {
        return dds::core::Entity(*this);
    }

    void py_enable() override { this->enable(); };

    const dds::core::status::StatusMask py_status_changes() override { return this->status_changes(); }

    const dds::core::InstanceHandle py_instance_handle() const override { return this->instance_handle(); }

    void py_close() override;

    void py_retain() override { this->retain(); }

    virtual
    ~PySubscriber();
};

}