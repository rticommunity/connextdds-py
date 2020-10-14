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
#include <dds/core/xtypes/StructType.hpp>
#include "PySafeEnum.hpp"

using namespace dds::core::xtypes;

namespace pyrti {

template<>
void process_inits<ExtensibilityKind>(py::module& m, ClassInitList&)
{
    init_dds_safe_enum<ExtensibilityKind_def>(
            m,
            "ExtensibilityKind",
            [](py::object& o) {
                py::enum_<ExtensibilityKind::type>(o, "ExtensibilityKind")
                        .value("FINAL",
                               ExtensibilityKind::type::FINAL,
                               "Final extensibility.")
                        .value("EXTENSIBLE",
                               ExtensibilityKind::type::EXTENSIBLE,
                               "Extensible extensibility.")
                        .value("MUTABLE",
                               ExtensibilityKind::type::MUTABLE,
                               "Mutable extensibility.")
                        .export_values();
            });
}

}  // namespace pyrti
