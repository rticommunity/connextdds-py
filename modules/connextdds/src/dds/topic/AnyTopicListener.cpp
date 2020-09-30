#include "PyConnext.hpp"
#include "PyAnyTopicListener.hpp"

namespace pyrti {

template<>
void init_class_defs(
        py::class_<PyAnyTopicListener, PyAnyTopicListenerTrampoline<>>& cls)
{
    cls.def(py::init<>())
            .def("on_inconsistent_topic",
                 (void (PyAnyTopicListener::*)(
                         PyAnyTopic&,
                         const dds::core::status::InconsistentTopicStatus&))
                         & PyAnyTopicListener::on_inconsistent_topic,
                 "Inconsistent topic callback.");
}

template<>
void init_class_defs(py::class_<
                     PyNoOpAnyTopicListener,
                     PyAnyTopicListener,
                     PyNoOpAnyTopicListenerTrampoline<>>& cls)
{
    cls.def(py::init<>())
            .def("on_inconsistent_topic",
                 (void (PyNoOpAnyTopicListener::*)(
                         PyAnyTopic&,
                         const dds::core::status::InconsistentTopicStatus&))
                         & PyNoOpAnyTopicListener::on_inconsistent_topic,
                 "Inconsistent topic callback.");
}

template<>
void process_inits<dds::topic::AnyTopicListener>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<PyAnyTopicListener, PyAnyTopicListenerTrampoline<>>(
                m,
                "AnyTopicListener");
    });

    l.push_back([m]() mutable {
        return init_class<
                PyNoOpAnyTopicListener,
                PyAnyTopicListener,
                PyNoOpAnyTopicListenerTrampoline<>>(m, "NoOpAnyTopicListener");
    });
}

}  // namespace pyrti
