#include "PyConnext.hpp"
#include "PyPublisherListener.hpp"

namespace pyrti {

template<>
void init_class_defs(
    py::class_<
        PyPublisherListener,
        PyAnyDataWriterListener,
        PyAnyDataWriterListenerTrampoline<PyPublisherListener>>& cls) {
        
    cls
        .def(
            py::init<>()
        );
}

template<>
void init_class_defs(
    py::class_<
        PyNoOpPublisherListener,
        PyPublisherListener,
        PyNoOpAnyDataWriterListenerTrampoline<PyNoOpPublisherListener>>& cls) {
    cls
        .def(
            py::init<>()
        );
}

template<>
void process_inits<dds::pub::PublisherListener>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<
                PyPublisherListener,
                PyAnyDataWriterListener,
                PyAnyDataWriterListenerTrampoline<PyPublisherListener>>(m, "PublisherListener");
        }
    );

    l.push_back(
        [m]() mutable {
            return init_class<
                PyNoOpPublisherListener,
                PyPublisherListener,
                PyNoOpAnyDataWriterListenerTrampoline<PyNoOpPublisherListener>>(m, "NoOpPublisherListener");
        }
    );
}

}
