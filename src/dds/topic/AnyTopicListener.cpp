#include "PyConnext.hpp"
#include "PyAnyTopicListener.hpp"

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyAnyTopicListener, pyrti::PyAnyTopicListenerTrampoline<>>& cls) {
    cls
        .def(
            py::init<>()
        )
        .def(
            "on_inconsistent_topic",
            (void (pyrti::PyAnyTopicListener::*)(pyrti::PyAnyTopic&, const dds::core::status::InconsistentTopicStatus&)) &pyrti::PyAnyTopicListener::on_inconsistent_topic,
            "Inconsistent topic callback."
        );
}

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyNoOpAnyTopicListener, pyrti::PyAnyTopicListener, pyrti::PyNoOpAnyTopicListenerTrampoline<>>& cls) {
    cls
        .def(
            py::init<>()
        )
        .def(
            "on_inconsistent_topic",
            (void (pyrti::PyNoOpAnyTopicListener::*)(pyrti::PyAnyTopic&, const dds::core::status::InconsistentTopicStatus&)) &pyrti::PyNoOpAnyTopicListener::on_inconsistent_topic,
            "Inconsistent topic callback."
        );
}

template<>
void pyrti::process_inits<dds::topic::AnyTopicListener>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyAnyTopicListener, pyrti::PyAnyTopicListenerTrampoline<>>(m, "AnyTopicListener");
        }
    );

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyNoOpAnyTopicListener, pyrti::PyAnyTopicListener, pyrti::PyNoOpAnyTopicListenerTrampoline<>>(m, "NoOpAnyTopicListener");
        }
    );
}
