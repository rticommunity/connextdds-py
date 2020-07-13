#pragma once

#include "PyConnext.hpp"
#include "PyTopic.hpp"
#include <dds/topic/TopicListener.hpp>

namespace py = pybind11;

namespace pyrti {

template<typename T>
class PyTopic;

template<typename T>
class PyTopicListener : public dds::topic::TopicListener<T> {
public:
    using dds::topic::TopicListener<T>::TopicListener;

    using dds::topic::TopicListener<T>::on_inconsistent_topic;

    virtual void on_inconsistent_topic(
            PyTopic<T>& topic,
            const dds::core::status::InconsistentTopicStatus& status) = 0;
};

template<typename T>
class PyNoOpTopicListener : public PyTopicListener<T> {
public:
    using PyTopicListener<T>::PyTopicListener;

    using PyTopicListener<T>::on_inconsistent_topic;

    void on_inconsistent_topic(
            PyTopic<T>& topic,
            const dds::core::status::InconsistentTopicStatus& status) override
    {
    }
};

template<typename T, class TLBase = PyTopicListener<T>>
class PyTopicListenerTrampoline : public TLBase {
public:
    using TLBase::TLBase;

    void on_inconsistent_topic(
            dds::topic::Topic<T>& topic,
            const dds::core::status::InconsistentTopicStatus& status) override
    {
        PyTopic<T> t(topic);
        this->on_inconsistent_topic(t, status);
    }

    void on_inconsistent_topic(
            PyTopic<T>& topic,
            const dds::core::status::InconsistentTopicStatus& status) override
    {
        PYBIND11_OVERLOAD_PURE(
                void,
                TLBase,
                on_inconsistent_topic,
                topic,
                status);
    }
};

template<typename T, class TLBase = PyNoOpTopicListener<T>>
class PyNoOpTopicListenerTrampoline
        : public PyTopicListenerTrampoline<T, TLBase> {
public:
    using PyTopicListenerTrampoline<T, TLBase>::PyTopicListenerTrampoline;

    using PyTopicListenerTrampoline<T, TLBase>::on_inconsistent_topic;

    void on_inconsistent_topic(
            PyTopic<T>& topic,
            const dds::core::status::InconsistentTopicStatus& status) override
    {
        PYBIND11_OVERLOAD(void, TLBase, on_inconsistent_topic, topic, status);
    }
};

template<typename T>
void init_topic_listener_defs(
        py::class_<PyTopicListener<T>, PyTopicListenerTrampoline<T>>& cls)
{
    cls.def(py::init<>())
            .def("on_inconsistent_topic",
                 (void (PyTopicListener<T>::*)(
                         PyTopic<T>&,
                         const dds::core::status::InconsistentTopicStatus&))
                         & PyTopicListener<T>::on_inconsistent_topic,
                 "Inconsistent topic callback.");
}

template<typename T>
void init_noop_topic_listener_defs(py::class_<
                                   PyNoOpTopicListener<T>,
                                   PyTopicListener<T>,
                                   PyNoOpTopicListenerTrampoline<T>>& cls)
{
    cls.def(py::init<>())
            .def("on_inconsistent_topic",
                 (void (PyNoOpTopicListener<T>::*)(
                         PyTopic<T>&,
                         const dds::core::status::InconsistentTopicStatus&))
                         & PyNoOpTopicListener<T>::on_inconsistent_topic,
                 "Inconsistent topic callback.");
}

template<typename T>
void init_topic_listener(
        py::class_<PyTopicListener<T>, PyTopicListenerTrampoline<T>>& tl,
        py::class_<
                PyNoOpTopicListener<T>,
                PyTopicListener<T>,
                PyNoOpTopicListenerTrampoline<T>>& notl)
{
    init_topic_listener_defs(tl);
    init_noop_topic_listener_defs(notl);
}

}  // namespace pyrti
