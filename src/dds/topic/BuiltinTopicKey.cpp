#include "PyConnext.hpp"
#include <dds/topic/BuiltinTopicKey.hpp>

using namespace dds::topic;

template<>
void pyrti::init_class_defs(py::class_<BuiltinTopicKey>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates a key whose value is all zeros."
        )
        .def_property_readonly(
            "value",
            &BuiltinTopicKey::value,
            "Returns a copy of the four integers that represent the key."
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
void pyrti::process_inits<BuiltinTopicKey>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<BuiltinTopicKey>(m, "BuiltinTopicKey");
        }
    );
}
