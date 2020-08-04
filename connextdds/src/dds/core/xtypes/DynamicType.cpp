#include "PyConnext.hpp"
#include <dds/core/xtypes/AliasType.hpp>
#include <dds/core/xtypes/CollectionTypes.hpp>
#include <dds/core/xtypes/DynamicType.hpp>
#include <dds/core/xtypes/EnumType.hpp>
#include <dds/core/xtypes/StructType.hpp>

using namespace dds::core::xtypes;

namespace pyrti {

py::object py_cast_type(dds::core::xtypes::DynamicType &dt) {
  switch (dt.kind().underlying()) {
  case dds::core::xtypes::TypeKind::inner_enum::ALIAS_TYPE: {
    auto resolved = rti::core::xtypes::resolve_alias(dt);
    return py_cast_type(resolved);
  }
  case dds::core::xtypes::TypeKind::ARRAY_TYPE:
    return py::cast(static_cast<dds::core::xtypes::ArrayType &>(dt));
  case dds::core::xtypes::TypeKind::ENUMERATION_TYPE:
    return py::cast(static_cast<dds::core::xtypes::EnumType &>(dt));
  case dds::core::xtypes::TypeKind::SEQUENCE_TYPE:
    return py::cast(static_cast<dds::core::xtypes::SequenceType &>(dt));
  case dds::core::xtypes::TypeKind::STRUCTURE_TYPE:
    return py::cast(static_cast<dds::core::xtypes::StructType &>(dt));
  case dds::core::xtypes::TypeKind::STRING_TYPE:
    return py::cast(static_cast<dds::core::xtypes::StringType &>(dt));
  case dds::core::xtypes::TypeKind::WSTRING_TYPE:
    return py::cast(static_cast<dds::core::xtypes::StringType &>(dt));
  case dds::core::xtypes::TypeKind::BOOLEAN_TYPE:
    return py::cast(static_cast<dds::core::xtypes::PrimitiveType<bool> &>(dt));
  case dds::core::xtypes::TypeKind::UINT_8_TYPE:
    return py::cast(
        static_cast<dds::core::xtypes::PrimitiveType<uint8_t> &>(dt));
  case dds::core::xtypes::TypeKind::INT_16_TYPE:
    return py::cast(
        static_cast<dds::core::xtypes::PrimitiveType<int16_t> &>(dt));
  case dds::core::xtypes::TypeKind::UINT_16_TYPE:
    return py::cast(
        static_cast<dds::core::xtypes::PrimitiveType<uint16_t> &>(dt));
  case dds::core::xtypes::TypeKind::INT_32_TYPE:
    return py::cast(
        static_cast<dds::core::xtypes::PrimitiveType<int32_t> &>(dt));
  case dds::core::xtypes::TypeKind::UINT_32_TYPE:
    return py::cast(
        static_cast<dds::core::xtypes::PrimitiveType<uint32_t> &>(dt));
  case dds::core::xtypes::TypeKind::INT_64_TYPE:
    return py::cast(
        static_cast<dds::core::xtypes::PrimitiveType<rti::core::int64> &>(dt));
  case dds::core::xtypes::TypeKind::UINT_64_TYPE:
    return py::cast(
        static_cast<dds::core::xtypes::PrimitiveType<rti::core::uint64> &>(dt));
  case dds::core::xtypes::TypeKind::FLOAT_32_TYPE:
    return py::cast(static_cast<dds::core::xtypes::PrimitiveType<float> &>(dt));
  case dds::core::xtypes::TypeKind::FLOAT_64_TYPE:
    return py::cast(
        static_cast<dds::core::xtypes::PrimitiveType<double> &>(dt));
  case dds::core::xtypes::TypeKind::FLOAT_128_TYPE:
    return py::cast(
        static_cast<dds::core::xtypes::PrimitiveType<rti::core::LongDouble> &>(
            dt));
  case dds::core::xtypes::TypeKind::CHAR_8_TYPE:
    return py::cast(static_cast<dds::core::xtypes::PrimitiveType<char> &>(dt));
#if rti_connext_version_gte(6, 0, 0)
  case dds::core::xtypes::TypeKind::CHAR_16_TYPE:
    return py::cast(
        static_cast<dds::core::xtypes::PrimitiveType<DDS_Wchar> &>(dt));
#endif
  default:
    return py::cast(dt);
  }
}

template <> void init_class_defs(py::class_<DynamicType> &cls) {
  cls.def_property_readonly("kind", &DynamicType::kind, "Get the type kind.")
      .def_property_readonly("name", &DynamicType::name, "Gets the name.")
      .def(
          "is_primitive_type",
          [](const DynamicType &dt) {
            return dds::core::xtypes::is_primitive_type(dt);
          },
          "Determines if this DynamicType is a PrimitiveType")
      .def(
          "is_constructed_type",
          [](const DynamicType &dt) {
            return dds::core::xtypes::is_constructed_type(dt);
          },
          "Determines if this DynamicType is a constructed type.")
      .def(
          "is_collection_type",
          [](const DynamicType &dt) {
            return dds::core::xtypes::is_collection_type(dt);
          },
          "Determins if this DynamicType is a CollectionType.")
      .def(
          "is_aggregation_type",
          [](const DynamicType &dt) {
            return dds::core::xtypes::is_aggregation_type(dt);
          },
          "Determines if this DynamicType is an aggregation type.")
      .def("print_idl", &rti::core::xtypes::print_idl, py::arg("index") = 0,
           "Prints the IDL representation of this type to the standard output.")
      .def(py::self == py::self, "Compare DynamicType objects for equality.")
      .def(py::self != py::self, "Compare DynamicType objects for inequality.");
}

template <> void process_inits<DynamicType>(py::module &m, ClassInitList &l) {
  l.push_back(
      [m]() mutable { return init_class<DynamicType>(m, "DynamicType"); });
}

} // namespace pyrti
