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
#include <dds/core/policy/CorePolicy.hpp>
#include "PySafeEnum.hpp"

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<Presentation>& cls)
{
    cls.def(py::init<>(), "Create an instance with the default value.")
            .def(py::init<PresentationAccessScopeKind, bool, bool>(),
                 py::arg("access_scope"),
                 py::arg("coherent_access"),
                 py::arg("ordered_access"),
                 "Creates an instance with the specified access scope and "
                 "coherent "
                 "and ordered access.")
            .def_property(
                    "access_scope",
                    (PresentationAccessScopeKind(Presentation::*)() const)
                            & Presentation::access_scope,
                    (Presentation
                     & (Presentation::*) (PresentationAccessScopeKind))
                            & Presentation::access_scope,
                    "Determines the largest scope spanning the entities for "
                    "which the "
                    "order and coherency of changes can be preserved.")
            .def_property(
                    "coherent_access",
                    (bool (Presentation::*)() const)
                            & Presentation::coherent_access,
                    (Presentation & (Presentation::*) (bool) )
                            & Presentation::coherent_access,
                    "Controls whether coherent access is supported within the "
                    "access_scope.")
            .def_property(
                    "ordered_access",
                    (bool (Presentation::*)() const)
                            & Presentation::ordered_access,
                    (Presentation & (Presentation::*) (bool) )
                            & Presentation::ordered_access,
                    "Controls whether ordered access is supported within the "
                    "access_scope.")
            .def_static(
                    "group_access_scope",
                    &Presentation::GroupAccessScope,
                    py::arg("coherent"),
                    py::arg("ordered"),
                    "Creates a Presentation instance with group access scope.")
            .def_static(
                    "instance_access_scope",
                    &Presentation::InstanceAccessScope,
                    py::arg("coherent"),
                    py::arg("ordered"),
                    "Creates a Presentation instance with instance access "
                    "scope.")
            .def_static(
                    "topic_access_scope",
                    &Presentation::TopicAccessScope,
                    py::arg("coherent"),
                    py::arg("ordered"),
                    "Creates a Presentation instance with topic access scope.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

static void init_dds_presentation_access_scope_kind(py::module& m)
{
    init_dds_safe_enum<PresentationAccessScopeKind_def>(
            m,
            "PresentationAccessScopeKind",
            [](py::object& o) {
                py::enum_<PresentationAccessScopeKind::type>(o, "Enum")
                        .value("INSTANCE",
                               PresentationAccessScopeKind::type::INSTANCE,
                               "[default] Scope spans only a single instance."
                               "\n\n"
                               "Indicates that changes to one instance need "
                               "not be coherent nor "
                               "ordered with respect to changes to any other "
                               "instance. In other "
                               "words, order and coherent changes apply to "
                               "each instance "
                               "separately.")
                        .value("TOPIC",
                               PresentationAccessScopeKind::type::TOPIC,
                               "Scope spans to all instances within the same "
                               "DataWriter (or "
                               "DataReader), but not across instances in "
                               "different DataWriter "
                               "(or DataReader).")
                        .value("GROUP",
                               PresentationAccessScopeKind::type::GROUP,
                               "Scope spans to all instances belonging to "
                               "DataWriter (or "
                               "DataReader) entities within the same Publisher "
                               "(or Subscriber).")
                        .value("HIGHEST_OFFERED",
                               PresentationAccessScopeKind::type::
                                       HIGHEST_OFFERED,
                               "This value only applies to a Subscriber. The "
                               "Subscriber will use "
                               "the access scope specified by each remote "
                               "Publisher.")
                        .export_values();
            });
}

template<>
void process_inits<Presentation>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        init_dds_presentation_access_scope_kind(m);
        return init_class<Presentation>(m, "Presentation");
    });
}

}  // namespace pyrti
