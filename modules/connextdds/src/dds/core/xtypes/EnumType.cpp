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
#include <dds/core/xtypes/EnumType.hpp>

using namespace dds::core::xtypes;
using namespace rti::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(
        py::class_<EnumType, AbstractConstructedType<EnumMember>>& cls)
{
    cls.def(py::init<const std::string&>(),
            py::arg("name"),
            "Creates an empty EnumType.")
            .def(py::init<const std::string&, const std::vector<EnumMember>&>(),
                 py::arg("name"),
                 py::arg("members"),
                 "Creates a enum with the members in the list.")
            .def(py::init([](DynamicType& dt) {
                     return static_cast<EnumType&>(dt);
                 }),
                 py::arg("type"),
                 "Cast a DynamicType to an EnumType.")
            .def("find_member_by_ordinal",
                 &EnumType::find_member_by_ordinal,
                 py::arg("ordinal"),
                 "Gets the index of the member with this ordinal value.")
            .def("add_member",
                 &EnumType::add_member,
                 py::arg("member"),
                 "Adds a member at the end.")
            .def("add_members",
                 (EnumType & (EnumType::*) (const std::vector<EnumMember>&) )
                         & EnumType::add_members<std::vector<EnumMember>>,
                 py::arg("members"),
                 "Adds a member at the end.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<EnumType>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<EnumType, AbstractConstructedType<EnumMember>>(
                m,
                "EnumType");
    });
}

}  // namespace pyrti
