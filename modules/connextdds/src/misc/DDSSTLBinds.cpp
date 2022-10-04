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

#include "PyConnext.hpp"
#include <pybind11/stl_bind.h>
#include "PyEntity.hpp"
#include "PyAnyTopic.hpp"
#include "PyAnyDataReader.hpp"
#include "PyAnyDataWriter.hpp"
#include "PyCondition.hpp"
#include "PyBindVector.hpp"

void init_dds_stl_bindings(py::module& m)
{
    pyrti::bind_vector<bool>(m, "BoolSeq");
    pyrti::bind_buffer_vector<uint8_t>(m, "Uint8Seq", "OctetSeq");
    pyrti::bind_buffer_vector<char>(m, "CharSeq");
    pyrti::bind_buffer_vector<int8_t>(m, "Int8Seq");
    pyrti::bind_buffer_vector<uint16_t>(m, "Uint16Seq", "UshortSeq");
    pyrti::bind_buffer_vector<int16_t>(m, "Int16Seq", "ShortSeq");
    pyrti::bind_buffer_vector<uint32_t>(m, "Uint32Seq", "ULongSeq");
    pyrti::bind_buffer_vector<int32_t>(m, "Int32Seq", "LongSeq");
    pyrti::bind_buffer_vector<rti::core::uint64>(m, "Uint64Seq", "ULongLongSeq");
    pyrti::bind_buffer_vector<rti::core::int64>(m, "Int64Seq", "LongLongSeq");
    pyrti::bind_buffer_vector<float>(m, "Float32Seq", "FloatSeq");
    pyrti::bind_buffer_vector<double>(m, "Float64Seq", "DoubleSeq");
    pyrti::bind_vector<wchar_t>(m, "WcharSeq");
    pyrti::bind_vector<std::string>(m, "StringSeq");
    pyrti::bind_vector<std::wstring>(m, "WstringSeq");

    pyrti::bind_vector<std::pair<std::string, std::string>>(m, "StringPairSeq");
    py::bind_map<std::map<std::string, std::string>>(m, "StringMap");

    py::implicitly_convertible<py::buffer, std::vector<uint8_t>>();
    py::implicitly_convertible<py::buffer, std::vector<char>>();
    py::implicitly_convertible<py::buffer, std::vector<int8_t>>();
    py::implicitly_convertible<py::buffer, std::vector<uint16_t>>();
    py::implicitly_convertible<py::buffer, std::vector<int16_t>>();
    py::implicitly_convertible<py::buffer, std::vector<uint32_t>>();
    py::implicitly_convertible<py::buffer, std::vector<int32_t>>();
    py::implicitly_convertible<py::buffer, std::vector<rti::core::uint64>>();
    py::implicitly_convertible<py::buffer, std::vector<rti::core::int64>>();
    py::implicitly_convertible<py::buffer, std::vector<float>>();
    py::implicitly_convertible<py::buffer, std::vector<double>>();
    py::implicitly_convertible<py::iterable, std::vector<bool>>();
    py::implicitly_convertible<py::iterable, std::vector<uint8_t>>();
    py::implicitly_convertible<py::iterable, std::vector<char>>();
    py::implicitly_convertible<py::iterable, std::vector<int8_t>>();
    py::implicitly_convertible<py::iterable, std::vector<uint16_t>>();
    py::implicitly_convertible<py::iterable, std::vector<int16_t>>();
    py::implicitly_convertible<py::iterable, std::vector<uint32_t>>();
    py::implicitly_convertible<py::iterable, std::vector<int32_t>>();
    py::implicitly_convertible<py::iterable, std::vector<rti::core::uint64>>();
    py::implicitly_convertible<py::iterable, std::vector<rti::core::int64>>();
    py::implicitly_convertible<py::iterable, std::vector<float>>();
    py::implicitly_convertible<py::iterable, std::vector<double>>();
    py::implicitly_convertible<py::iterable, std::vector<wchar_t>>();
    py::implicitly_convertible<py::iterable, std::vector<std::string>>();
    py::implicitly_convertible<py::iterable, std::vector<std::wstring>>();

    py::implicitly_convertible<
            py::iterable,
            std::vector<std::pair<std::string, std::string>>>();
}
