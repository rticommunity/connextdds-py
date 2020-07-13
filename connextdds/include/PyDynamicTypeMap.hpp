#pragma once

#include "PyOpaqueTypes.hpp"
#include <dds/core/InstanceHandle.hpp>
#include <dds/core/xtypes/DynamicType.hpp>
#include <map>
#include <pybind11/pybind11.h>
#include <string>

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
};

}  // namespace pyrti
