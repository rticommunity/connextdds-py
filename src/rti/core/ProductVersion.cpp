#include "PyConnext.hpp"
#include <rti/core/ProductVersion.hpp>

using namespace rti::core;

template<>
void pyrti::init_class_defs(py::class_<ProductVersion>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates the unknown product version."
        )
        .def_property_readonly(
            "major_version",
            &ProductVersion::major_version,
            "The major product version."
        )
        .def_property_readonly(
            "minor_version",
            &ProductVersion::minor_version,
            "The minor product version."
        )
        .def_property_readonly(
            "release_version",
            &ProductVersion::release_version,
            "The release letter of the product version."
        )
        .def_property_readonly(
            "revision_version",
            &ProductVersion::revision_version,
            "The revision number of product."
        )
        .def(
            "__str__",
            &ProductVersion::to_string
        )
        .def_static(
            "current",
            &ProductVersion::current,
            "Create an instance representing the current version of RTI "
            "Connext."
        )
        .def_static(
            "unknown",
            &ProductVersion::unknown,
            "Create an instance representing an unknown version."
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
void pyrti::process_inits<ProductVersion>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<ProductVersion>(m, "ProductVersion");
        }
    );
}
