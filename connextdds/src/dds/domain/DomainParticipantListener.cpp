#include "PyConnext.hpp"
#include "PyDomainParticipantListener.hpp"

template<>
void pyrti::init_class_defs(
    py::class_<
        pyrti::PyDomainParticipantListener,
        pyrti::PyPublisherListener,
        pyrti::PySubscriberListener,
        pyrti::PyAnyTopicListener,
        pyrti::PyDomainParticipantListenerTrampoline<>>& cls) {
    cls
        .def(
            py::init<>()
        );
}

template<>
void pyrti::init_class_defs(
    py::class_<
        pyrti::PyNoOpDomainParticipantListener,
        pyrti::PyDomainParticipantListener,
        pyrti::PyNoOpDomainParticipantListenerTrampoline<>>& cls) {
    cls
        .def(
            py::init<>()
        );
}

template<>
void pyrti::process_inits<dds::domain::DomainParticipantListener>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<
                pyrti::PyDomainParticipantListener,
                pyrti::PyPublisherListener,
                pyrti::PySubscriberListener,
                pyrti::PyAnyTopicListener,
                pyrti::PyDomainParticipantListenerTrampoline<>>(m, "DomainParticipantListener");
        }
    );

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<
                pyrti::PyNoOpDomainParticipantListener,
                pyrti::PyDomainParticipantListener,
                pyrti::PyNoOpDomainParticipantListenerTrampoline<>>(m, "NoOpDomainParticipantListener");
        }
    );
}