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
void init_class_defs(py::class_<DestinationOrder>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<DestinationOrderKind>(),
                 py::arg("kind"),
                 "Creates a policy with the specified destination order kind.")
            .def_property(
                    "kind",
                    (DestinationOrderKind(DestinationOrder::*)() const)
                            & DestinationOrder::kind,
                    (DestinationOrder
                     & (DestinationOrder::*) (DestinationOrderKind))
                            & DestinationOrder::kind,
                    "The destination order kind.")
            .def_property(
                    "scope",
                    [](const DestinationOrder& d) { return d->scope(); },
                    [](DestinationOrder& d,
                       rti::core::policy::DestinationOrderScopeKind k) {
                        return d->scope(k);
                    },
                    "The destination order scope.")
            .def_property(
                    "source_timestamp_tolerance",
                    [](const DestinationOrder& d) {
                        return d->source_timestamp_tolerance();
                    },
                    [](DestinationOrder& d, const dds::core::Duration& t) {
                        return d->source_timestamp_tolerance(t);
                    },
                    "The allowed tolerance between source timestamps of "
                    "consecutive samples.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

static void init_dds_destination_order_kind(py::module& m)
{
    init_dds_safe_enum<DestinationOrderKind_def>(
            m,
            "DestinationOrderKind",
            [](py::object& o) {
                py::enum_<DestinationOrderKind::type>(o, "DestinationOrderKind")
                        .value("BY_RECEPTION_TIMESTAMP",
                               DestinationOrderKind::type::
                                       BY_RECEPTION_TIMESTAMP,
                               "[default] Indicates that data is ordered based "
                               "on the reception "
                               "time at each Subscriber."
                               "\n\n"
                               "Since each subscriber may receive the data at "
                               "different times "
                               "there is no guaranteed that the changes will "
                               "be seen in the same "
                               "order. Consequently, it is possible for each "
                               "subscriber to end "
                               "up with a different final value for the data.")
                        .value("BY_SOURCE_TIMESTAMP",
                               DestinationOrderKind::type::BY_SOURCE_TIMESTAMP,
                               "Indicates that data is ordered based on a "
                               "time-stamp placed at "
                               "the source (by RTI Connext or by the "
                               "application)."
                               "\n\n"
                               "In any case this guarantees a consistent final "
                               "value for the "
                               "data in all subscribers."
                               "\n\n"
                               "Note: If Batching is needed along with "
                               "DestinationOrderKind.BY_SOURCE_TIMESTAMP and "
                               "DestinationOrderScopeKind.INSTANCE, then the "
                               "Batch.source_timestamp_resolution and "
                               "Batch.thread_safe_write setting of Batch "
                               "should be set to "
                               "Duration.zero() and true respectively.")
                        .export_values();
            });
}

template<>
void process_inits<DestinationOrder>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        init_dds_destination_order_kind(m);
        return init_class<DestinationOrder>(m, "DestinationOrder");
    });
}

}  // namespace pyrti