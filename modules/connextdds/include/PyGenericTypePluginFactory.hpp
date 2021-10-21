/*
 * (c) 2021 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software solely for use with RTI products.  The Software is
 * provided "as is", with no warranty of any type, including any warranty for
 * fitness for any purpose. RTI is under no obligation to maintain or support
 * the Software.  RTI shall not be liable for any incidental or consequential
 * damages arising out of the use or inability to use the software.
 */

#pragma once

#include "PyConnext.hpp"
#include <rti/core/xtypes/DynamicTypeImpl.hpp>
#include <rti/topic/cdr/GenericTypePluginFactory.hpp>

namespace pyrti {

// Holds a pointer to a TypePlugin and its associated DynamicType created from
// a GenericTypePluginFactory. This struct is used to pass around these pointers
// between Python and C++. The pointers are owned by the factory and are valid
// until the factory is destroyed.
struct PluginDynamicTypeHolder {
    rti::core::xtypes::DynamicTypeImpl *type;
    rti::topic::cdr::CTypePlugin *type_plugin;
};

} // namespace pyrti