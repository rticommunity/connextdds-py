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
#include <dds/core/xtypes/MemberType.hpp>

using namespace dds::core::xtypes;
using namespace rti::core::xtypes;

namespace pyrti {

template<>
void init_class_defs(
        py::class_<AbstractConstructedType<Member>, DynamicType>& cls)
{
    cls.def("extensibility_kind",
            &AbstractConstructedType<Member>::extensibility_kind,
            "Gets the extensibility kind.")
            .def_property_readonly(
                    "member_count",
                    &AbstractConstructedType<Member>::member_count,
                    "Gets the number of members.")
            .def("member",
                 (const Member& (AbstractConstructedType<
                                 Member>::*) (AbstractConstructedType<Member>::
                                                      MemberIndex) const)
                         & AbstractConstructedType<Member>::member,
                 py::return_value_policy::reference_internal,
                 "Gets a member by its index.")
            .def("member",
                 (const Member& (AbstractConstructedType<
                                 Member>::*) (const std::string&) const)
                         & AbstractConstructedType<Member>::member,
                 py::return_value_policy::reference_internal,
                 "Gets a member by its name.")
            .def("__getitem__",
                 [](const AbstractConstructedType<Member>& act,
                    const std::string& name) -> const Member& { 
                            return act.member(name); 
                 },
                 py::return_value_policy::reference_internal)
            .def("__getitem__",
                 [](const AbstractConstructedType<Member>& act,
                    uint32_t index) -> const Member& { 
                            return act.member(index); 
                 },
                 py::return_value_policy::reference_internal)
            .def("find_member_by_name",
                 &AbstractConstructedType<Member>::find_member_by_name,
                 "Obtains the member index from its name.")
            .def("members",
                 &AbstractConstructedType<Member>::members,
                 "Gets a copy of all the members")
#if rti_connext_version_gte(6, 0, 0, 0)
            .def("cdr_serialized_sample_max_size",
                 &AbstractConstructedType<
                         Member>::cdr_serialized_sample_max_size,
                 "Gets the maximum serialized size of samples of this type.")
#endif
            .def_readonly_static(
                    "INVALID_INDEX",
                    &AbstractConstructedType<Member>::INVALID_INDEX,
                    "Indicates a member doesn't exist")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<AbstractConstructedType<Member>>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<AbstractConstructedType<Member>, DynamicType>(
                m,
                "ACTMember");
    });
}

}  // namespace pyrti
