#include "PyConnext.hpp"
#include <rti/core/VendorId.hpp>

using namespace rti::core;

template<>
void pyrti::init_class_defs(py::class_<VendorId>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates unknown vendor ID."
        )
        .def_property_readonly(
            "value",
            &VendorId::value,
            "Provides access to the bytes that represent the vendor id."
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        )
        .def_static(
            "unknown",
            &VendorId::unknown,
            "Create an unknown vendor ID."
        );
}

template<>
void pyrti::process_inits<VendorId>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<VendorId>(m, "VendorId");
        }
    );
}
