#include "PyConnext.hpp"
#include "PyDomainParticipantListener.hpp"

namespace pyrti {

template<>
void init_class_defs(
    py::class_<
        PyDomainParticipantListener,
        PyPublisherListener,
        PySubscriberListener,
        PyAnyTopicListener,
        PyDomainParticipantListenerTrampoline<>>& cls) {
    cls
        .def(
            py::init<>()
        );
}

template<>
void init_class_defs(
    py::class_<
        PyNoOpDomainParticipantListener,
        PyDomainParticipantListener,
        PyNoOpDomainParticipantListenerTrampoline<>>& cls) {
    cls
        .def(
            py::init<>()
        );
}

template<>
void process_inits<dds::domain::DomainParticipantListener>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<
                PyDomainParticipantListener,
                PyPublisherListener,
                PySubscriberListener,
                PyAnyTopicListener,
                PyDomainParticipantListenerTrampoline<>>(m, "DomainParticipantListener");
        }
    );

    l.push_back(
        [m]() mutable {
            return init_class<
                PyNoOpDomainParticipantListener,
                PyDomainParticipantListener,
                PyNoOpDomainParticipantListenerTrampoline<>>(m, "NoOpDomainParticipantListener");
        }
    );
}

}
