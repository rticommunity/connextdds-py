#pragma once

#include "PyConnext.hpp"
#include <dds/core/Entity.hpp>
#include <dds/domain/DomainParticipant.hpp>
#include <dds/pub/Publisher.hpp>
#include <dds/sub/Subscriber.hpp>

namespace pyrti {
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

        pyrti::PyDomainParticipant& operator=(const dds::domain::DomainParticipant& dp) {
            dds::domain::DomainParticipant::operator=(dp);
            return *this;
        }

        dds::core::Entity get_entity() override {
            return dds::core::Entity(*this);
        }

        void py_enable() override { this->enable(); };
 
        const dds::core::status::StatusMask py_status_changes() override { return this->status_changes(); }

        const dds::core::InstanceHandle py_instance_handle() const override { return this->instance_handle(); }

        void py_close() override { this->close(); }

        void py_retain() override { this->retain(); }


        virtual
        ~PyDomainParticipant() {}
    };

    class PyPublisher : public dds::pub::Publisher, public PyIEntity {
    public:
        using dds::pub::Publisher::Publisher;

        PyPublisher(const dds::pub::Publisher& pub) : dds::pub::Publisher(pub) {}

        dds::core::Entity get_entity() override {
            return dds::core::Entity(*this);
        }

        void py_enable() override { this->enable(); };
 
        const dds::core::status::StatusMask py_status_changes() override { return this->status_changes(); }

        const dds::core::InstanceHandle py_instance_handle() const override { return this->instance_handle(); }

        void py_close() override { this->close(); }

        void py_retain() override { this->retain(); }


        virtual
        ~PyPublisher() {}
    };

    class PySubscriber : public dds::sub::Subscriber, public PyIEntity {
    public:
        using dds::sub::Subscriber::Subscriber;

        PySubscriber(const dds::sub::Subscriber& sub) : dds::sub::Subscriber(sub) {}

        dds::core::Entity get_entity() override {
            return dds::core::Entity(*this);
        }

        void py_enable() override { this->enable(); };
 
        const dds::core::status::StatusMask py_status_changes() override { return this->status_changes(); }

        const dds::core::InstanceHandle py_instance_handle() const override { return this->instance_handle(); }

        void py_close() override { this->close(); }

        void py_retain() override { this->retain(); }

        virtual
        ~PySubscriber() {}
    };
}