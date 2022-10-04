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

#pragma once

#include <map>
#include <string>
#include <dds/core/InstanceHandle.hpp>
#include <dds/core/xtypes/DynamicType.hpp>
#include <pybind11/pybind11.h>
#include "PyOpaqueTypes.hpp"

namespace pyrti {

class PyDynamicTypeMap {
private:
    static std::unordered_map<std::string, dds::core::xtypes::DynamicType>
            type_map;

public:
    static bool add(
            const std::string& name,
            const dds::core::xtypes::DynamicType& type)
    {
        auto p = type_map.insert(
                std::pair<std::string, dds::core::xtypes::DynamicType>(
                        name,
                        type));
        return p.second;
    }

    static dds::core::xtypes::DynamicType get(const std::string& name)
    {
        std::unordered_map<std::string, dds::core::xtypes::DynamicType>::
                iterator it(type_map.find(name));
        if (it == type_map.end())
            throw pybind11::key_error(
                    name + std::string("not found in type map."));
        return it->second;
    }

    static void finalize()
    {
        type_map.clear();
    }
};

}  // namespace pyrti
