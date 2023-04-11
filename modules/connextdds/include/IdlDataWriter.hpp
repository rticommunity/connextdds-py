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
#include "IdlTopic.hpp"
#include "PyDataWriter.hpp"

namespace pyrti {

using IdlDataWriter = PyDataWriter<rti::topic::cdr::CSampleWrapper>;
using IdlDataWriterPyClass = PyDataWriterClass<rti::topic::cdr::CSampleWrapper>;

template<>
void init_dds_typed_datawriter_template(IdlDataWriterPyClass& cls);

IdlDataWriter create_idl_py_writer(
        const PyPublisher& publisher,
        const PyTopic<rti::topic::cdr::CSampleWrapper>& topic,
        const dds::pub::qos::DataWriterQos* qos = nullptr,
        PyDataWriterListenerPtr<rti::topic::cdr::CSampleWrapper>* listener = nullptr,
        dds::core::status::StatusMask mask =
                dds::core::status::StatusMask::none());

}  // namespace pyrti