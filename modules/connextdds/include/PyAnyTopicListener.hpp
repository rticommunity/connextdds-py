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
#include "PyOpaqueTypes.hpp"
#include <dds/topic/AnyTopicListener.hpp>
#include "PyAnyTopic.hpp"

namespace pyrti {

class PyAnyTopicListener : public dds::topic::AnyTopicListener {
public:
    using dds::topic::AnyTopicListener::AnyTopicListener;

    using dds::topic::AnyTopicListener::on_inconsistent_topic;

    virtual void on_inconsistent_topic(
            PyAnyTopic& topic,
            const dds::core::status::InconsistentTopicStatus& status) = 0;

    virtual ~PyAnyTopicListener()
    {
    }
};


class PyNoOpAnyTopicListener : public PyAnyTopicListener {
public:
    using PyAnyTopicListener::PyAnyTopicListener;

    using PyAnyTopicListener::on_inconsistent_topic;

    void on_inconsistent_topic(
            PyAnyTopic&,
            const dds::core::status::InconsistentTopicStatus&) override
    {
    }

    virtual ~PyNoOpAnyTopicListener()
    {
    }
};


template<class TLBase = PyAnyTopicListener>
class PyAnyTopicListenerTrampoline : public TLBase {
public:
    using TLBase::TLBase;

    void on_inconsistent_topic(
            dds::topic::AnyTopic& topic,
            const dds::core::status::InconsistentTopicStatus& status) override
    {
        auto at = PyAnyTopic(topic);
        this->on_inconsistent_topic(at, status);
    }

    void on_inconsistent_topic(
            PyAnyTopic& topic,
            const dds::core::status::InconsistentTopicStatus& status) override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                TLBase,
                on_inconsistent_topic,
                topic,
                status);
    }

    virtual ~PyAnyTopicListenerTrampoline()
    {
    }
};


template<class TLBase = PyNoOpAnyTopicListener>
class PyNoOpAnyTopicListenerTrampoline
        : public PyAnyTopicListenerTrampoline<TLBase> {
public:
    using PyAnyTopicListenerTrampoline<TLBase>::PyAnyTopicListenerTrampoline;

    using PyAnyTopicListenerTrampoline<TLBase>::on_inconsistent_topic;

    void on_inconsistent_topic(
            PyAnyTopic& topic,
            const dds::core::status::InconsistentTopicStatus& status) override
    {
        PYBIND11_OVERLOAD(void, TLBase, on_inconsistent_topic, topic, status);
    }

    virtual ~PyNoOpAnyTopicListenerTrampoline()
    {
    }
};

}  // namespace pyrti
