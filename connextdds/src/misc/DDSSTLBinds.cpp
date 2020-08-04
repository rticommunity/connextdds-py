#include "PyAnyDataReader.hpp"
#include "PyAnyDataWriter.hpp"
#include "PyAnyTopic.hpp"
#include "PyCondition.hpp"
#include "PyConnext.hpp"
#include "PyEntity.hpp"
#include <pybind11/stl_bind.h>

void init_dds_stl_bindings(py::module &m) {
  py::bind_vector<std::vector<bool>>(m, "BoolSeq");
  py::bind_vector<std::vector<uint8_t>>(m, "OctetSeq", py::buffer_protocol());
  py::bind_vector<std::vector<char>>(m, "CharSeq", py::buffer_protocol());
  py::bind_vector<std::vector<uint16_t>>(m, "Uin16Seq", py::buffer_protocol());
  py::bind_vector<std::vector<int16_t>>(m, "Int16Seq", py::buffer_protocol());
  py::bind_vector<std::vector<uint32_t>>(m, "Uint32Seq", py::buffer_protocol());
  py::bind_vector<std::vector<int32_t>>(m, "Int32Seq", py::buffer_protocol());
  py::bind_vector<std::vector<rti::core::uint64>>(m, "Uint64Seq",
                                                  py::buffer_protocol());
  py::bind_vector<std::vector<rti::core::int64>>(m, "Int64Seq",
                                                 py::buffer_protocol());
  py::bind_vector<std::vector<float>>(m, "Float32Seq", py::buffer_protocol());
  py::bind_vector<std::vector<double>>(m, "Float64Seq", py::buffer_protocol());
  py::bind_vector<std::vector<wchar_t>>(m, "WcharSeq");
  py::bind_vector<std::vector<std::string>>(m, "StringSeq");
  py::bind_vector<std::vector<std::wstring>>(m, "WstringSeq");

  py::bind_vector<std::vector<std::pair<std::string, std::string>>>(
      m, "StringPairSeq");
  py::bind_map<std::map<std::string, std::string>>(m, "StringMap");

  py::implicitly_convertible<py::buffer, std::vector<uint8_t>>();
  py::implicitly_convertible<py::buffer, std::vector<char>>();
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
      py::iterable, std::vector<std::pair<std::string, std::string>>>();
}
