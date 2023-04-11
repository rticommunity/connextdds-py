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
#include "PyDataReader.hpp"


namespace pyrti {

using PyIdlDataReader = PyDataReader<rti::topic::cdr::CSampleWrapper>;

using IdlDataReaderPyClass = py::class_<
        PyIdlDataReader,
        PyIDataReader,
        std::unique_ptr<PyIdlDataReader, no_gil_delete<PyIdlDataReader>>>;

template<>
void init_dds_typed_datareader_template(IdlDataReaderPyClass& cls);

PyIdlDataReader create_idl_py_reader(
        const PySubscriber& subscriber,
        const PyTopic<rti::topic::cdr::CSampleWrapper>& topic,
        const PyContentFilteredTopic<rti::topic::cdr::CSampleWrapper>& cft =
                dds::core::null,
        const dds::sub::qos::DataReaderQos* qos = nullptr,
        PyDataReaderListenerPtr<rti::topic::cdr::CSampleWrapper>* listener =
                nullptr,
        const dds::core::status::StatusMask& mask =
                dds::core::status::StatusMask::none());

}  // namespace pyrti