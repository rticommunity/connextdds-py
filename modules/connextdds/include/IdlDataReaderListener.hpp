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
#include "PyDataReaderListener.hpp"
#include "IdlTypeSupport.hpp"

namespace pyrti {

using PyIdlDataReaderListener = PyDataReaderListener<rti::topic::cdr::CSampleWrapper>;

using IdlDataReaderListenerPyClass = py::class_<
        PyDataReaderListener<rti::topic::cdr::CSampleWrapper>,
        PyDataReaderListenerTrampoline<rti::topic::cdr::CSampleWrapper>,
        std::shared_ptr<PyDataReaderListener<rti::topic::cdr::CSampleWrapper>>>;

using PyIdlNoOpDataReaderListener =
        PyNoOpDataReaderListener<rti::topic::cdr::CSampleWrapper>;

using IdlNoOpDataReaderListenerPyClass = py::class_<
        PyNoOpDataReaderListener<rti::topic::cdr::CSampleWrapper>,
        PyDataReaderListener<rti::topic::cdr::CSampleWrapper>,
        PyNoOpDataReaderListenerTrampoline<rti::topic::cdr::CSampleWrapper>,
        std::shared_ptr<PyNoOpDataReaderListener<rti::topic::cdr::CSampleWrapper>>>;

}  // namespace pyrti