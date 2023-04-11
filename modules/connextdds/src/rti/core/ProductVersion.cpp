/*
 * (c) 2020 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */

#include "PyConnext.hpp"

#include <string>

#include <rti/core/ProductVersion.hpp>

using namespace rti::core;

namespace pyrti {

template<>
void init_class_defs(py::class_<ProductVersion>& cls)
{
    cls.def(py::init<>(), "Creates the unknown product version.")
            .def_property_readonly(
                    "major_version",
                    &ProductVersion::major_version,
                    "The major product version.")
            .def_property_readonly(
                    "minor_version",
                    &ProductVersion::minor_version,
                    "The minor product version.")
            .def_property_readonly(
                    "release_version",
                    &ProductVersion::release_version,
                    "The release letter of the product version.")
            .def_property_readonly(
                    "revision_version",
                    &ProductVersion::revision_version,
                    "The revision number of product.")
            .def_property_readonly(
                    "native_build_id",
                    [](const ProductVersion&) {
                        return std::string(NDDS_Config_Version_to_string());
                    },
                    "A string with the build ID of the native Connext libraries.")
            .def("__str__", &ProductVersion::to_string)
            .def_property_readonly_static(
                    "current",
                    [](py::object&) {
                        return ProductVersion::current();
                    },
                    "Create an instance representing the current version of "
                    "RTI "
                    "Connext.")
            .def_property_readonly_static(
                    "unknown",
                    [](py::object&) {
                        return ProductVersion::unknown();
                    },
                    "Create an instance representing an unknown version.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<ProductVersion>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<ProductVersion>(m, "ProductVersion");
    });
}

}  // namespace pyrti
