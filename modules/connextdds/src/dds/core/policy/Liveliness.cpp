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
void init_class_defs(py::class_<Liveliness>& cls)
{
    cls.def(py::init<>(),
            "Creates an automatic liveliness policy with infinite lease "
            "duration.")
            .def(py::init<LivelinessKind, const dds::core::Duration&>(),
                 py::arg("kind"),
                 py::arg("duration"),
                 "Creates an instance with the specified liveliness kind and "
                 "lease "
                 "duration.")
            .def_property(
                    "kind",
                    (LivelinessKind(Liveliness::*)() const) & Liveliness::kind,
                    (Liveliness & (Liveliness::*) (LivelinessKind))
                            & Liveliness::kind,
                    "The liveliness kind.")
            .def_property(
                    "lease_duration",
                    (dds::core::Duration(Liveliness::*)() const)
                            & Liveliness::lease_duration,
                    (Liveliness & (Liveliness::*) (const dds::core::Duration&) )
                            & Liveliness::lease_duration,
                    "The duration within which a Entity must be asserted or "
                    "else it "
                    "is considered not alive."
                    "\n\n"
                    "This property's getter returns a deep copy.")
#if rti_connext_version_gte(6, 0, 0, 0)
            .def_property(
                    "assertions_per_lease_duration",
                    [](const Liveliness& l) {
                        return l->assertions_per_lease_duration();
                    },
                    [](Liveliness& l, int32_t a) {
                        return l->assertions_per_lease_duration(a);
                    },
                    "The number of assertions to send during the lease "
                    "duration.")
#else
            .def_property(
                    "assertions_per_lease_duration",
                    [](const Liveliness& l) {
                        return l.delegate()
                                .native()
                                .assertions_per_lease_duration;
                    },
                    [](Liveliness& l, int32_t a) {
                        return l.delegate()
                                       .native()
                                       .assertions_per_lease_duration = a;
                    },
                    "The number of assertions to send during the lease "
                    "duration.")
#endif
            .def_property_readonly_static(
                    "automatic",
                    [](py::object&) {
                        return Liveliness::Automatic();
                    },
                    "Creates a Liveliness instance with "
                    "LivelinessKind.AUTOMATIC.")
            .def_static(
                    "manual_by_participant",
                    &Liveliness::ManualByParticipant,
                    py::arg_v(
                            "lease",
                            dds::core::Duration::infinite(),
                            "Duration.infinite"),
                    "Creates a Liveliness instance with "
                    "LivelinessKind.MANUAL_BY_PARTICIPANT.")
            .def_static(
                    "manual_by_topic",
                    &Liveliness::ManualByTopic,
                    py::arg_v(
                            "lease",
                            dds::core::Duration::infinite(),
                            "Duration.infinite"),
                    "Creates a Liveliness instance with "
                    "LivelinessKind.MANUAL_BY_TOPIC.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

static void init_dds_liveliness_kind(py::module& m)
{
    init_dds_safe_enum<
            LivelinessKind_def>(m, "LivelinessKind", [](py::object& o) {
        py::enum_<LivelinessKind::type>(o, "LivelinessKind")
                .value("AUTOMATIC",
                       LivelinessKind::type::AUTOMATIC,
                       "[default] The infrastructure will automatically signal "
                       "liveliness for the DataWriter (s) at least as often as "
                       "required "
                       "by the DataWriter (S) lease_duration."
                       "\n\n"
                       "A DataWriter with this setting does not need to take "
                       "any "
                       "specific action in order to be considered 'alive.' The "
                       "DataWriter is only 'not alive' when the participant to "
                       "which it "
                       "belongs terminates (gracefully or not), or when there "
                       "is a "
                       "network problem that prevents the current participant "
                       "from "
                       "contacting that remote participant.")
                .value("MANUAL_BY_PARTICIPANT",
                       LivelinessKind::type::MANUAL_BY_PARTICIPANT,
                       "RTI Connext will assume that as long as at least one "
                       "DataWriter "
                       "belonging to the DomainParticipant (or the "
                       "DomainParticipant "
                       "itself) has asserted its liveliness, then the other "
                       "DataWriters "
                       "belonging to that same DomainParticipant are also "
                       "alive."
                       "\n\n"
                       "The user application takes responsibility to signal "
                       "liveliness "
                       "to RTI Connext either by calling "
                       "DomainParticipant.assert_liveliness, or by calling "
                       "DataWriter.assert_liveliness, or DataWriter.write() on "
                       "any "
                       "DataWriter belonging to the DomainParticipant.")
                .value("MANUAL_BY_TOPIC",
                       LivelinessKind::type::MANUAL_BY_TOPIC,
                       "RTI Connext will only assume liveliness of the "
                       "DataWriter if the "
                       "application has asserted liveliness of that DataWriter "
                       "itself."
                       "\n\n"
                       "The user application takes responsibility to signal "
                       "liveliness "
                       "to RTI Connext using the DataWriter.assert_liveliness "
                       "method, or "
                       "by writing some data.")
                .export_values();
    });
}

template<>
void process_inits<Liveliness>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        init_dds_liveliness_kind(m);
        return init_class<Liveliness>(m, "Liveliness");
    });
}

}  // namespace pyrti
