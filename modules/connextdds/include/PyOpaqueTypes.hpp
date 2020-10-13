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

#include <pybind11/pybind11.h>
#include "PyEntity.hpp"
#include "PyCondition.hpp"
#include <dds/core/xtypes/MemberType.hpp>
#include <dds/core/xtypes/DynamicData.hpp>
#include <dds/core/InstanceHandle.hpp>
#include "PyAnyDataReader.hpp"
#include "PyAnyDataWriter.hpp"
#include "PyAnyTopic.hpp"
#include <rti/core/EndpointGroup.hpp>
#include <rti/core/Locator.hpp>
#include <rti/core/policy/CorePolicy.hpp>


PYBIND11_MAKE_OPAQUE(std::vector<bool>);
PYBIND11_MAKE_OPAQUE(std::vector<uint8_t>);
PYBIND11_MAKE_OPAQUE(std::vector<char>);
PYBIND11_MAKE_OPAQUE(std::vector<uint16_t>);
PYBIND11_MAKE_OPAQUE(std::vector<int16_t>);
PYBIND11_MAKE_OPAQUE(std::vector<uint32_t>);
PYBIND11_MAKE_OPAQUE(std::vector<int32_t>);
PYBIND11_MAKE_OPAQUE(std::vector<uint64_t>);
PYBIND11_MAKE_OPAQUE(std::vector<int64_t>);
PYBIND11_MAKE_OPAQUE(std::vector<float>);
PYBIND11_MAKE_OPAQUE(std::vector<double>);
PYBIND11_MAKE_OPAQUE(std::vector<rti::core::LongDouble>);
PYBIND11_MAKE_OPAQUE(std::vector<wchar_t>);
PYBIND11_MAKE_OPAQUE(std::vector<std::string>);
PYBIND11_MAKE_OPAQUE(std::vector<std::wstring>);
PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyICondition*>);
PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyCondition>);
PYBIND11_MAKE_OPAQUE(std::vector<dds::core::xtypes::Member>);
PYBIND11_MAKE_OPAQUE(std::vector<dds::core::xtypes::EnumMember>);
PYBIND11_MAKE_OPAQUE(std::vector<dds::core::xtypes::UnionMember>);
PYBIND11_MAKE_OPAQUE(std::vector<dds::core::InstanceHandle>);
PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyIEntity*>);
PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyEntity>);
PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyDomainParticipant>);
PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyIAnyTopic*>);
PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyAnyTopic>);
PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PySubscriber>);
PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyIAnyDataReader*>);
PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyAnyDataReader>);
PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyPublisher>);
PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyIAnyDataWriter*>);
PYBIND11_MAKE_OPAQUE(std::vector<pyrti::PyAnyDataWriter>);
PYBIND11_MAKE_OPAQUE(std::vector<rti::core::EndpointGroup>);
PYBIND11_MAKE_OPAQUE(std::vector<rti::core::Locator>);
PYBIND11_MAKE_OPAQUE(std::vector<rti::core::LocatorFilterElement>);
PYBIND11_MAKE_OPAQUE(std::vector<rti::core::ChannelSettings>);
PYBIND11_MAKE_OPAQUE(std::vector<rti::core::TransportUnicastSettings>);
PYBIND11_MAKE_OPAQUE(std::vector<std::pair<std::string, std::string>>);
PYBIND11_MAKE_OPAQUE(std::map<std::string, std::string>);
