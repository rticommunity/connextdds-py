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

#if rti_connext_version_gte(6, 1, 0)

#include <rti/core/policy/CorePolicy.hpp>
#include "PySafeEnum.hpp"

using namespace rti::core::policy;

namespace pyrti {

template<>
void process_inits<DataReaderInstanceRemovalKind>(py::module& m, ClassInitList& l)
{
    init_dds_safe_enum<
            DataReaderInstanceRemovalKind_def>(m, "DataReaderInstanceRemovalKind ", [](py::object& o) {
        py::enum_<DataReaderInstanceRemovalKind::type>(o, "DataReaderInstanceRemovalKind ")
                .value("NO_INSTANCE",
                       DataReaderInstanceRemovalKind::type::NO_INSTANCE,
                       "No instance can be removed.")
                .value("EMPTY_INSTANCES",
                       DataReaderInstanceRemovalKind::type::EMPTY_INSTANCES,
                       "Only empty instances can be removed")
                .value("FULLY_PROCESSED_INSTANCES",
                       DataReaderInstanceRemovalKind::type::FULLY_PROCESSED_INSTANCES,
                       "Only fully-processed instances can be removed.")
                .value("ANY_INSTANCE",
                       DataReaderInstanceRemovalKind::type::ANY_INSTANCE,
                       "Any instance can be removed.")
                .export_values();
    });
}

}  // namespace pyrti

#endif
