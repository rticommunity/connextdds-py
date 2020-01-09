#pragma once

#include "PyConnext.hpp"
#include <dds/topic/AnyTopic.hpp>
#include "PyEntity.hpp"

namespace pyrti {
    class PyIAnyTopic {
    public:
        virtual 
        dds::topic::AnyTopic get_any_topic() const = 0;

        virtual
        pyrti::PyDomainParticipant py_domain_participant() const = 0;

        virtual
        dds::core::status::InconsistentTopicStatus py_inconsistent_topic_status() = 0;

        virtual
        dds::topic::qos::TopicQos py_qos() const = 0;

        virtual
        void py_qos(const dds::topic::qos::TopicQos&) = 0;

        virtual
        const std::string py_name() const = 0;

        virtual
        const std::string py_type_name() const = 0;

        virtual
        void py_close() = 0;

        virtual
        ~PyIAnyTopic() {}
    };

    class PyAnyTopic : public dds::topic::AnyTopic, public PyIAnyTopic {
    public:
        using dds::topic::AnyTopic::AnyTopic;
        
        PyAnyTopic(dds::topic::AnyTopic& at) : dds::topic::AnyTopic(at) {}

        PyAnyTopic(const dds::topic::AnyTopic& at) : dds::topic::AnyTopic(at) {}

        PyAnyTopic(const pyrti::PyIAnyTopic& at) : dds::topic::AnyTopic(at.get_any_topic()) {}

        dds::topic::AnyTopic get_any_topic() const override {
            return (*this);
        }

        pyrti::PyDomainParticipant py_domain_participant() const override {
            auto dp = this->domain_participant();
            return pyrti::PyDomainParticipant(dp);
        }

        dds::core::status::InconsistentTopicStatus py_inconsistent_topic_status() override { return this->inconsistent_topic_status(); }

        dds::topic::qos::TopicQos py_qos() const override { return this->qos(); }

        void py_qos(const dds::topic::qos::TopicQos& q) override { this->qos(q); }

        const std::string py_name() const override { return this->name(); }

        const std::string py_type_name() const override { return this->type_name(); }

        void py_close() override { this->close(); }

        virtual
        ~PyAnyTopic() {}
    };
}
