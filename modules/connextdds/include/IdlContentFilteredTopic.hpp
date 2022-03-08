/*
 * (c) 2022 Copyright, Real-Time Innovations, Inc.  All rights reserved.
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
#include "PyContentFilteredTopic.hpp"
#include "IdlTypeSupport.hpp"

namespace pyrti {

using PyIdlContentFilteredTopic =
        PyContentFilteredTopic<rti::topic::cdr::CSampleWrapper>;

using IdlContentFilteredTopicPyClass = py::class_<
        PyIdlContentFilteredTopic,
        PyITopicDescription<rti::topic::cdr::CSampleWrapper>,
        PyIAnyTopic,
        std::unique_ptr<
                PyIdlContentFilteredTopic,
                no_gil_delete<PyIdlContentFilteredTopic>>>;

}  // namespace pyrti