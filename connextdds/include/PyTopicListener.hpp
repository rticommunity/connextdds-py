#pragma once

#include "PyConnext.hpp"
#include <dds/topic/TopicListener.hpp>
#include "PyTopic.hpp"

namespace py = pybind11;

namespace pyrti {
    template<typename T>
    class PyTopic;

    template<typename T>
    class PyTopicListener : public dds::topic::TopicListener<T> {
    public:
        using dds::topic::TopicListener<T>::TopicListener;

        using dds::topic::TopicListener<T>::on_inconsistent_topic;

        virtual
        void on_inconsistent_topic(
            pyrti::PyTopic<T>& topic,
            const dds::core::status::InconsistentTopicStatus& status
        ) = 0;
    };

    template<typename T>
    class PyNoOpTopicListener : public pyrti::PyTopicListener<T> {
    public:
        using pyrti::PyTopicListener<T>::PyTopicListener;

        using pyrti::PyTopicListener<T>::on_inconsistent_topic;

        void on_inconsistent_topic(
            pyrti::PyTopic<T>& topic,
            const dds::core::status::InconsistentTopicStatus& status
        ) override {}
    };

    template<typename T, class TLBase = pyrti::PyTopicListener<T>>
    class PyTopicListenerTrampoline : public TLBase {
    public:
        using TLBase::TLBase;

        void on_inconsistent_topic(
            dds::topic::Topic<T>& topic,
            const dds::core::status::InconsistentTopicStatus& status
        ) override {
            pyrti::PyTopic<T> t(topic);
            this->on_inconsistent_topic(t, status);
        }

        void on_inconsistent_topic(
            pyrti::PyTopic<T>& topic,
            const dds::core::status::InconsistentTopicStatus& status
        ) override {
            PYBIND11_OVERLOAD_PURE(
                void,
                TLBase,
                on_inconsistent_topic,
                topic,
                status
            );
        }
    };

    template<typename T, class TLBase = pyrti::PyNoOpTopicListener<T>>
    class PyNoOpTopicListenerTrampoline : public pyrti::PyTopicListenerTrampoline<T, TLBase> {
    public:
        using pyrti::PyTopicListenerTrampoline<T, TLBase>::PyTopicListenerTrampoline;

        using pyrti::PyTopicListenerTrampoline<T, TLBase>::on_inconsistent_topic;

        void on_inconsistent_topic(
            pyrti::PyTopic<T>& topic,
            const dds::core::status::InconsistentTopicStatus& status
        ) override {
            PYBIND11_OVERLOAD(
                void,
                TLBase,
                on_inconsistent_topic,
                topic,
                status
            );
        }
    };

    template<typename T>
    void init_topic_listener_defs(py::class_<pyrti::PyTopicListener<T>, pyrti::PyTopicListenerTrampoline<T>>& cls) {
        cls
            .def(
                py::init<>()
            )
            .def(
                "on_inconsistent_topic",
                (void (pyrti::PyTopicListener<T>::*)(pyrti::PyTopic<T>&, const dds::core::status::InconsistentTopicStatus&)) &pyrti::PyTopicListener<T>::on_inconsistent_topic,
                "Inconsistent topic callback."
            );
    }

    template<typename T>
    void init_noop_topic_listener_defs(py::class_<pyrti::PyNoOpTopicListener<T>, pyrti::PyNoOpTopicListenerTrampoline<T>>& cls) {
        cls
            .def(
                py::init<>()
            )
            .def(
                "on_inconsistent_topic",
                (void (pyrti::PyNoOpTopicListener<T>::*)(pyrti::PyTopic<T>&, const dds::core::status::InconsistentTopicStatus&)) &pyrti::PyNoOpTopicListener<T>::on_inconsistent_topic,
                "Inconsistent topic callback."
            );
    }

    template<typename T>
    void init_topic_listener(py::object& o) {
        py::class_<pyrti::PyTopicListener<T>, pyrti::PyTopicListenerTrampoline<T>> tl(o, "TopicListener");
        py::class_<pyrti::PyNoOpTopicListener<T>, pyrti::PyNoOpTopicListenerTrampoline<T>> notl(o, "NoOpTopicListener");
        
        pyrti::init_topic_listener_defs(tl);
        pyrti::init_noop_topic_listener_defs(notl);
    }
}

