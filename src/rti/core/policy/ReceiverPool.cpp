#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

template<>
void pyrti::init_class_defs(py::class_<ReceiverPool>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates the default policy."
        )
        .def(
            py::init<
                const rti::core::ThreadSettings&,
                int32_t,
                int32_t
            >(),
            py::arg("thread"),
            py::arg("buffer_size"),
            py::arg("buffer_alignment"),
            "Creates an instance with the thread settings, buffer size and "
            "buffer alignment configuration."
        )
        .def_property(
            "thread",
            (rti::core::ThreadSettings& (ReceiverPool::*)()) &ReceiverPool::thread,
            (ReceiverPool& (ReceiverPool::*)(const rti::core::ThreadSettings&)) &ReceiverPool::thread,
            "Configures the receiver pool thread(s)."
        )
        .def_property(
            "buffer_size",
            (int32_t (ReceiverPool::*)() const) &ReceiverPool::buffer_size,
            (ReceiverPool& (ReceiverPool::*)(int32_t)) &ReceiverPool::buffer_size,
            "The length of the buffer used to store the incoming raw data."
        )
        .def_property(
            "buffer_alignment",
            (int32_t (ReceiverPool::*)() const) &ReceiverPool::buffer_alignment,
            (ReceiverPool& (ReceiverPool::*)(int32_t)) &ReceiverPool::buffer_alignment,
            "The receive buffer alignment."
        )
        .def_property_readonly_static(
            "LENGTH_AUTO",
            [](){
                return ReceiverPool::LENGTH_AUTO;
            },
            "Indicates that the length will be automatically resolved."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

template<>
void pyrti::process_inits<ReceiverPool>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<ReceiverPool>(m, "ReceiverPool");
        }
    );
}
