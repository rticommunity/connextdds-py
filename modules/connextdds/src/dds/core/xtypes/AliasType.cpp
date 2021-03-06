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
#include <dds/core/xtypes/AliasType.hpp>

using namespace dds::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<AliasType, DynamicType>& cls)
{
    cls.def(py::init<const std::string&, const DynamicType&, bool>(),
            py::arg("name"),
            py::arg("type_name"),
            py::arg("is_pointer") = false,
            "Creates an alias with a name and a related type.")
            .def("related_type",
                 &AliasType::related_type,
                 "Gets the related type.")
            .def_property_readonly(
                    "is_pointer",
                    &AliasType::is_pointer,
                    "Gets whether this alias makes related_type a pointer")
            .def("resolve",
                 &rti::core::xtypes::resolve_alias,
                 "Resolves an AliasType recursively to get the final "
                 "underlying "
                 "type.")
            .def_static(
                    "resolve_type",
                    &rti::core::xtypes::resolve_alias,
                    py::arg("alias_type"),
                    "Resolves an AliasType recursively to get the final "
                    "underlying "
                    "type.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.")
            .doc() = "Represents a typedef.";
}

template<>
void process_inits<AliasType>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<AliasType, DynamicType>(m, "AliasType");
    });
}

}  // namespace pyrti
