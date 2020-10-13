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
void process_inits<RefilterKind>(py::module& m, ClassInitList& l)
{
    init_dds_safe_enum<rti::core::policy::RefilterKind_def>(
            m,
            "RefilterKind",
            [](py::object& o) {
                py::enum_<RefilterKind::type>(o, "Enum")
                        .value("NOTHING",
                               RefilterKind::type::NOTHING,
                               "[default] Do not filter existing samples for a "
                               "new reader."
                               "\n\n"
                               "On the publishing side, when a new reader "
                               "matches a writer, the "
                               "writer can be configured to filter previously "
                               "written samples "
                               "stored in the writer queue for the new reader. "
                               "This option "
                               "configures the writer to not filter any "
                               "existing samples for the "
                               "reader and the reader will do the filtering.")
                        .value("EVERYTHING",
                               RefilterKind::type::EVERYTHING,
                               "Filter all existing samples for a new reader."
                               "\n\n"
                               "On the publishing side, when a new reader "
                               "matches a writer, "
                               "the writer can be configured to filter "
                               "previously written "
                               "samples stored in the writer queue. This "
                               "option configures the "
                               "writer to filter all existing samples for the "
                               "reader when a new "
                               "reader is matched to the writer.")
                        .value("ON_DEMAND",
                               RefilterKind::type::ON_DEMAND,
                               "Filter existing samples only when they are "
                               "requested by the reader."
                               "\n\n"
                               "On the publishing side, when a new reader "
                               "matches a writer, the "
                               "writer can be configured to filter previously "
                               "written samples "
                               "stored in the writer queue. This option "
                               "configures the writer to "
                               "filter only existing samples that are "
                               "requested by the reader.")
                        .export_values();
            });
}

}  // namespace pyrti
