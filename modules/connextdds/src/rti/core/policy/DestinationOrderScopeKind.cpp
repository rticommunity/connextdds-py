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
#include <rti/core/policy/CorePolicy.hpp>
#include "PySafeEnum.hpp"

using namespace rti::core::policy;

namespace pyrti {

template<>
void process_inits<DestinationOrderScopeKind>(py::module& m, ClassInitList&)
{
    init_dds_safe_enum<DestinationOrderScopeKind_def>(
            m,
            "DestinationOrderScopeKind",
            [](py::object& o) {
                py::enum_<DestinationOrderScopeKind::type>(o, "DestinationOrderScopeKind")
                        .value("INSTANCE",
                               DestinationOrderScopeKind::type::INSTANCE,
                               "[default] Indicates that data is ordered on a "
                               "per instance basis "
                               "if used along with "
                               "DestinationOrderKind.BY_SOURCE_TIMESTAMP. "
                               "\n\n"
                               "The source timestamp of the current sample is "
                               "compared to the "
                               "source timestamp of the previously received "
                               "sample for the same "
                               "instance. The tolerance check is also applied "
                               "per instance.")
                        .value("TOPIC",
                               DestinationOrderScopeKind::type::TOPIC,
                               "Indicates that data is ordered on a per topic "
                               "basis if used "
                               "along with "
                               "DestinationOrderKind.BY_SOURCE_TIMESTAMP."
                               "\n\n"
                               "The source timestamp of the current sample is "
                               "compared to the "
                               "source timestamp of the previously received "
                               "sample for the same "
                               "topic. The tolerance check is also applied per "
                               "topic.")
                        .export_values();
            });
}

}  // namespace pyrti
