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
#include <dds/core/xtypes/UnionType.hpp>

using namespace dds::core::xtypes;
using namespace rti::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(
        py::class_<UnionType, AbstractConstructedType<UnionMember>>& cls)
{
    cls.def(py::init<const std::string&, const DynamicType&>(),
            py::arg("name"),
            py::arg("discriminator_type"),
            "Creates an empty Union.")
            .def(py::init<
                         const std::string&,
                         const DynamicType&,
                         const std::vector<UnionMember>&>(),
                 py::arg("name"),
                 py::arg("discriminator_type"),
                 py::arg("members"),
                 "Creates a Union with the specified UnionMembers.")
            .def(py::init([](DynamicType& dt) {
                     return static_cast<UnionType&>(dt);
                 }),
                 py::arg("type"),
                 "Cast a DynamicType to a UnionType.")
            .def_property_readonly(
                    "descriminator",
                    &UnionType::discriminator,
                    "The union descriminator type.")
            .def(
                    "find_member_by_label",
                    [](const UnionType& u, UnionType::DiscriminatorType d) {
                        return u.find_member_by_label(d);
                    },
                    py::arg("label"),
                    "Gets the index of the member selected by a label.")
            .def("find_member_by_id",
                 &UnionType::find_member_by_id,
                 py::arg("id"),
                 "Gets the index of the member selected by an ID.")
            .def("add_members",
                 (UnionType & (UnionType::*) (const std::vector<UnionMember>&) )
                         & UnionType::add_members<std::vector<UnionMember>>,
                 py::arg("members"),
                 "Adds all the members of a container at the end.")
            .def_property(
                    "extensibility_kind",
                    (ExtensibilityKind(UnionType::*)() const)
                            & UnionType::extensibility_kind,
                    (UnionType & (UnionType::*) (ExtensibilityKind))
                            & UnionType::extensibility_kind,
                    "Union's extensibility kind.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<UnionType>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<UnionType, AbstractConstructedType<UnionMember>>(
                m,
                "UnionType");
    });
}

}  // namespace pyrti
