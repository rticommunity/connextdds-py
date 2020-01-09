#include "PyConnext.hpp"
#include "PyPublisherListener.hpp"

template<>
void pyrti::init_class_defs(
    py::class_<
        pyrti::PyPublisherListener,
        pyrti::PyAnyDataWriterListener,
        pyrti::PyAnyDataWriterListenerTrampoline<pyrti::PyPublisherListener>>& cls) {
        
    cls
        .def(
            py::init<>()
        );
}

template<>
void pyrti::init_class_defs(
    py::class_<
        pyrti::PyNoOpPublisherListener,
        pyrti::PyPublisherListener,
        pyrti::PyNoOpAnyDataWriterListenerTrampoline<pyrti::PyNoOpPublisherListener>>& cls) {
    cls
        .def(
            py::init<>()
        );
}

template<>
void pyrti::process_inits<dds::pub::PublisherListener>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<
                pyrti::PyPublisherListener,
                pyrti::PyAnyDataWriterListener,
                pyrti::PyAnyDataWriterListenerTrampoline<pyrti::PyPublisherListener>>(m, "PublisherListener");
        }
    );

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<
                pyrti::PyNoOpPublisherListener,
                pyrti::PyPublisherListener,
                pyrti::PyNoOpAnyDataWriterListenerTrampoline<pyrti::PyNoOpPublisherListener>>(m, "NoOpPublisherListener");
        }
    );
}