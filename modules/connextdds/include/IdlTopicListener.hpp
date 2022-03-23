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
#include "PyTopicListener.hpp"
#include "IdlTypeSupport.hpp"

namespace pyrti {

using PyIdlTopicListener = PyTopicListener<rti::topic::cdr::CSampleWrapper>;

using IdlTopicListenerPyClass = py::class_<
        PyTopicListener<rti::topic::cdr::CSampleWrapper>,
        PyTopicListenerTrampoline<rti::topic::cdr::CSampleWrapper>,
        std::shared_ptr<PyTopicListener<rti::topic::cdr::CSampleWrapper>>>;

using PyIdlNoOpTopicListener = PyNoOpTopicListener<rti::topic::cdr::CSampleWrapper>;

using IdlNoOpTopicListenerPyClass = py::class_<
        PyNoOpTopicListener<rti::topic::cdr::CSampleWrapper>,
        PyTopicListener<rti::topic::cdr::CSampleWrapper>,
        PyNoOpTopicListenerTrampoline<rti::topic::cdr::CSampleWrapper>,
        std::shared_ptr<PyNoOpTopicListener<rti::topic::cdr::CSampleWrapper>>>;

}  // namespace pyrti