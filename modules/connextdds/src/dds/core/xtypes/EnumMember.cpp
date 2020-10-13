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
#include "PySeq.hpp"
#include <dds/core/xtypes/MemberType.hpp>

using namespace dds::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(py::class_<EnumMember>& cls)
{
    cls.def(py::init<const std::string&, int32_t>(),
            py::arg("name"),
            py::arg("ordinal"),
            "Create an EnumMember with a given name and ordinal value.")
            .def_property(
                    "name",
                    [](const EnumMember& e) {
                        return e.name().to_std_string();
                    },
                    [](EnumMember& e, std::string& n) { e.name(n); },
                    "The member name.")
            .def_property(
                    "ordinal",
                    (int32_t(EnumMember::*)() const) & EnumMember::ordinal,
                    (EnumMember & (EnumMember::*) (int32_t))
                            & EnumMember::ordinal,
                    "The member's ordinal.")
            .def(py::self == py::self, "Test for equality.")
            .def(
                    "__int__",
                    (int32_t(EnumMember::*)() const) & EnumMember::ordinal)
            .def(
                    "__eq__",
                    [](const EnumMember& member, const EnumMember& other) {
                        return (member == other);
                    },
                    py::is_operator(),
                    "Test for equality.")
            .def(
                    "__ne__",
                    [](const EnumMember& member, const EnumMember& other) {
                        return !(member == other);
                    },
                    py::is_operator(),
                    "Test for inequality.")
            .def(
                    "__eq__",
                    [](const EnumMember& member, int32_t other) {
                        return (member.ordinal() == other);
                    },
                    py::is_operator(),
                    "Test for equality.")
            .def(
                    "__ne__",
                    [](const EnumMember& member, int32_t other) {
                        return !(member.ordinal() == other);
                    },
                    py::is_operator(),
                    "Test for inequality.");
}

template<>
void process_inits<EnumMember>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class_with_seq<EnumMember>(m, "EnumMember");
    });
}

}  // namespace pyrti
