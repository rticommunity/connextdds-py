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

namespace pyrti {

template<typename T>
struct NoOpSampleConverter {
    static const T& convert(const dds::pub::DataWriter<T>&, const T& sample)
    {
        return sample;
    }
};

// Used by the PyDataWriterTraits to indicate that the GIL doesn't need to be
// acquired nor released.
struct gil_noop {
};

// DefaultPyDataWriterTraits are used by types that do not require any
// conversion between a python and a C++ data sample (or the conversion is
// automatically performed by pybind11). This currently includes DynamicData and
// the built-in types. IDL-based types need a conversion and provide a different
// traits struct.
template <typename T>
struct DefaultPyDataWriterTraits {
    using py_type = T;
    using before_write_gil_policy = py::gil_scoped_release;
    using native_write_gil_policy = gil_noop;
    using sample_converter = NoOpSampleConverter<T>;
};

}  // namespace pyrti
