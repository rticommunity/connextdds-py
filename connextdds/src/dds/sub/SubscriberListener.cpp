#include "PyConnext.hpp"
#include "PySubscriberListener.hpp"

template<>
void pyrti::init_class_defs(
    py::class_<
        pyrti::PySubscriberListener, 
        pyrti::PyAnyDataReaderListener, 
        pyrti::PySubscriberListenerTrampoline<>>& cls) {
    cls
        .def(
            py::init<>()
        )
        .def(
            "on_data_on_readers",
            (void (pyrti::PySubscriberListener::*)(pyrti::PySubscriber&)) &pyrti::PySubscriberListener::on_data_on_readers,
            "Data on datareaders callback."
        );
}

template<>
void pyrti::init_class_defs(
    py::class_<
        pyrti::PyNoOpSubscriberListener, 
        pyrti::PySubscriberListener, 
        pyrti::PyNoOpSubscriberListenerTrampoline<>>& cls) {
    cls
        .def(
            py::init<>()
        )
        .def(
            "on_data_on_readers",
            (void (pyrti::PyNoOpSubscriberListener::*)(pyrti::PySubscriber&)) &pyrti::PyNoOpSubscriberListener::on_data_on_readers,
            "Data on datareaders callback."
        );
}

template<>
void pyrti::process_inits<dds::sub::SubscriberListener>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<
                pyrti::PySubscriberListener, 
                pyrti::PyAnyDataReaderListener, 
                pyrti::PySubscriberListenerTrampoline<>>(m, "SubscriberListener");
        }
    );

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<
                pyrti::PyNoOpSubscriberListener,
                pyrti::PySubscriberListener,
                pyrti::PyNoOpSubscriberListenerTrampoline<>>(m, "NoOpSubscriberListener");
        }
    );
}
