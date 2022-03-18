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
#include "PyDataWriterListener.hpp"
#include "IdlTypeSupport.hpp"

namespace pyrti {

using PyIdlDataWriterListener =
        PyDataWriterListener<rti::topic::cdr::CSampleWrapper>;

using IdlDataWriterListenerPyClass = py::class_<
        PyDataWriterListener<rti::topic::cdr::CSampleWrapper>,
        PyDataWriterListenerTrampoline<rti::topic::cdr::CSampleWrapper>,
        std::shared_ptr<PyDataWriterListener<rti::topic::cdr::CSampleWrapper>>>;

using PyIdlNoOpDataWriterListener =
        PyNoOpDataWriterListener<rti::topic::cdr::CSampleWrapper>;

using IdlNoOpDataWriterListenerPyClass = py::class_<
        PyNoOpDataWriterListener<rti::topic::cdr::CSampleWrapper>,
        PyDataWriterListener<rti::topic::cdr::CSampleWrapper>,
        PyNoOpDataWriterListenerTrampoline<rti::topic::cdr::CSampleWrapper>,
        std::shared_ptr<
                PyNoOpDataWriterListener<rti::topic::cdr::CSampleWrapper>>>;

}  // namespace pyrti