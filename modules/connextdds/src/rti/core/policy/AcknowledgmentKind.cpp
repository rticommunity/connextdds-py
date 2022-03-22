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
void process_inits<AcknowledgmentKind>(py::module& m, ClassInitList&)
{
    init_dds_safe_enum<
            AcknowledgmentKind_def>(m, "AcknowledgmentKind", [](py::object& o) {
        py::enum_<AcknowledgmentKind::type>(o, "AcknowledgmentKind")
                .value("PROTOCOL",
                       AcknowledgmentKind::type::PROTOCOL,
                       "Samples are acknowledged by RTPS protocol."
                       "\n\n"
                       "Samples are acknowledged according to the Real-Time "
                       "Publish-Subscribe (RTPS) interoperability protocol.")
                .value("APPLICATION_AUTO",
                       AcknowledgmentKind::type::APPLICATION_AUTO,
                       "Samples are acknowledged automatically after a "
                       "subscribing "
                       "application has accessed them."
                       "\n\n"
                       "The DataReader automatically acknowledges a sample "
                       "after it has "
                       "been taken and the loan returned.")
                .value("APPLICATION_ORDERED",
                       AcknowledgmentKind::type::APPLICATION_ORDERED,
                       "Samples up to a specified sequence number are "
                       "acknowledged.")
                .value("APPLICATION_EXPLICIT",
                       AcknowledgmentKind::type::APPLICATION_EXPLICIT,
                       "Samples are acknowledged after the subscribing "
                       "application "
                       "explicitly calls acknowledge on the samples."
                       "\n\n"
                       "Samples received by a DataReader are explicitly "
                       "acknowledged "
                       "by the subscribing application, after it calls either "
                       "DataReader.acknowledge_all or "
                       "DataReader.acknowledge_sample.")
                .export_values();
    });
}

}  // namespace pyrti
