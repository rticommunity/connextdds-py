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

#pragma once

#include "PyConnext.hpp"
#include <dds/core/Entity.hpp>
#include <dds/domain/DomainParticipant.hpp>
#include <dds/pub/Publisher.hpp>
#include <dds/sub/Subscriber.hpp>
#include <mutex>

namespace pyrti {

class PyDomainParticipantListener;
class PyPublisherListener;
class PySubscriberListener;

#if rti_connext_version_lt(6, 1, 0, 0)
using DomainParticipantListenerPtr = dds::domain::DomainParticipantListener*;

using PyDomainParticipantListenerPtr = PyDomainParticipantListener*;
#else
using DomainParticipantListenerPtr = std::shared_ptr<dds::domain::DomainParticipantListener>;

using PyDomainParticipantListenerPtr = std::shared_ptr<PyDomainParticipantListener>;
#endif

#if rti_connext_version_lt(6, 1, 0, 0)
using PublisherListenerPtr = dds::pub::PublisherListener*;

using PyPublisherListenerPtr = PyPublisherListener*;
#else
using PublisherListenerPtr = std::shared_ptr<dds::pub::PublisherListener>;

using PyPublisherListenerPtr = std::shared_ptr<PyPublisherListener>;
#endif

#if rti_connext_version_lt(6, 1, 0, 0)
using SubscriberListenerPtr = dds::sub::SubscriberListener*;

using PySubscriberListenerPtr = PySubscriberListener*;
#else
using SubscriberListenerPtr = std::shared_ptr<dds::sub::SubscriberListener>;

using PySubscriberListenerPtr = std::shared_ptr<PySubscriberListener>;
#endif

class PyIEntity {
public:
    virtual dds::core::Entity get_entity() = 0;

    virtual void py_enable() = 0;

    virtual const dds::core::status::StatusMask py_status_changes() = 0;

    virtual const dds::core::InstanceHandle py_instance_handle() const = 0;

    virtual void py_close() = 0;

    virtual void py_retain() = 0;

    virtual bool py_closed() = 0;

    virtual bool py_enabled() = 0;

    virtual int py_use_count() = 0;

    virtual void py_unretain() = 0;

    virtual ~PyIEntity()
    {
    }
};

class PyEntity : public dds::core::Entity, public PyIEntity {
public:
    using dds::core::Entity::Entity;

    PyEntity(const dds::core::Entity& e) : dds::core::Entity(e)
    {
    }

    dds::core::Entity get_entity() override
    {
        return dds::core::Entity(*this);
    }

    void py_enable() override
    {
        this->enable();
    };

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

    virtual ~PyEntity()
    {
    }
};

template<typename T>
class PyDataReader;

class PYRTI_SYMBOL_PUBLIC PyDomainParticipant
        : public dds::domain::DomainParticipant,
          public PyIEntity {
public:
    using dds::domain::DomainParticipant::DomainParticipant;

    enum class Property {
        BUILTIN_SUBSCRIBER,
        PARTICIPANT_READER,
        TOPIC_READER,
        PUBLICATION_READER,
        SUBSCRIPTION_READER,
        SERVICE_REQUEST_READER,
        IMPLICIT_PUBLISHER,
        IMPLICIT_SUBSCRIBER,
    };

    struct PropertyHash {
        std::size_t operator()(Property p) const
        {
            return static_cast<std::size_t>(p);
        }
    };

    PyDomainParticipant(const PyDomainParticipant& dp) : dds::domain::DomainParticipant(dp)
    {}

    PyDomainParticipant& operator=(const dds::domain::DomainParticipant& dp)
    {
        dds::domain::DomainParticipant::operator=(dp);
        return *this;
    }

    PyDomainParticipant(
            int32_t d,
            const dds::domain::qos::DomainParticipantQos& q,
            PyDomainParticipantListenerPtr l,
            const dds::core::status::StatusMask& m);

    dds::core::Entity get_entity() override
    {
        return dds::core::Entity(*this);
    }

    void py_enable() override
    {
        this->enable();
    };

    const dds::core::status::StatusMask py_status_changes() override
    {
        return this->status_changes();
    }

    const dds::core::InstanceHandle py_instance_handle() const override
    {
        return this->instance_handle();
    }

    void py_close() override;

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

    template<typename T, typename F>
    PyDataReader<T>&
    py_builtin_reader(PyDomainParticipant::Property, F);

    template<typename T, typename F>
    T& py_entity_property(PyDomainParticipant::Property, F);

    virtual ~PyDomainParticipant();

private:
    std::unordered_map<Property, pybind11::object, PropertyHash> _properties;
    static std::recursive_mutex _property_lock;
};

class PyPublisher : public dds::pub::Publisher, public PyIEntity {
public:
    using dds::pub::Publisher::Publisher;

    PyPublisher(const dds::pub::Publisher& pub) : dds::pub::Publisher(pub)
    {
    }

    PyPublisher(
            const PyDomainParticipant& p,
            const dds::pub::qos::PublisherQos& q,
            PyPublisherListenerPtr l,
            const dds::core::status::StatusMask& m);

    dds::core::Entity get_entity() override
    {
        return dds::core::Entity(*this);
    }

    void py_enable() override
    {
        this->enable();
    };

    const dds::core::status::StatusMask py_status_changes() override
    {
        return this->status_changes();
    }

    const dds::core::InstanceHandle py_instance_handle() const override
    {
        return this->instance_handle();
    }

    void py_close() override;

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

    virtual ~PyPublisher();
};

class PySubscriber : public dds::sub::Subscriber, public PyIEntity {
public:
    using dds::sub::Subscriber::Subscriber;

    PySubscriber(const dds::sub::Subscriber& sub) : dds::sub::Subscriber(sub)
    {
    }

    PySubscriber(
            const PyDomainParticipant& p,
            const dds::sub::qos::SubscriberQos& q,
            PySubscriberListenerPtr l,
            const dds::core::status::StatusMask& m);

    dds::core::Entity get_entity() override
    {
        return dds::core::Entity(*this);
    }

    void py_enable() override
    {
        this->enable();
    };

    const dds::core::status::StatusMask py_status_changes() override
    {
        return this->status_changes();
    }

    const dds::core::InstanceHandle py_instance_handle() const override
    {
        return this->instance_handle();
    }

    void py_close() override;

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

    virtual ~PySubscriber();
};

}  // namespace pyrti