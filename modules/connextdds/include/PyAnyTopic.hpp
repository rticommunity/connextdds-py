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
#include <dds/topic/AnyTopic.hpp>
#include "PyEntity.hpp"

namespace pyrti {

class PyIAnyTopic {
public:
    virtual dds::topic::AnyTopic get_any_topic() const = 0;

    virtual PyDomainParticipant py_domain_participant() const = 0;

    virtual dds::core::status::InconsistentTopicStatus
    py_inconsistent_topic_status() = 0;

    virtual dds::topic::qos::TopicQos py_qos() const = 0;

    virtual void py_qos(const dds::topic::qos::TopicQos&) = 0;

    virtual const std::string py_name() const = 0;

    virtual const std::string py_type_name() const = 0;

    virtual void py_close() = 0;

    virtual ~PyIAnyTopic()
    {
    }
};

class PyAnyTopic : public dds::topic::AnyTopic, public PyIAnyTopic {
public:
    using dds::topic::AnyTopic::AnyTopic;

    PyAnyTopic(dds::topic::AnyTopic& at) : dds::topic::AnyTopic(at)
    {
    }

    PyAnyTopic(const dds::topic::AnyTopic& at) : dds::topic::AnyTopic(at)
    {
    }

    PyAnyTopic(const PyIAnyTopic& at) : dds::topic::AnyTopic(at.get_any_topic())
    {
    }

    dds::topic::AnyTopic get_any_topic() const override
    {
        return (*this);
    }

    PyDomainParticipant py_domain_participant() const override
    {
        auto dp = this->domain_participant();
        return PyDomainParticipant(dp);
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

    const std::string py_name() const override
    {
        return this->name();
    }

    const std::string py_type_name() const override
    {
        return this->type_name();
    }

    void py_close() override
    {
        this->close();
    }

    virtual ~PyAnyTopic()
    {
    }
};

}  // namespace pyrti
