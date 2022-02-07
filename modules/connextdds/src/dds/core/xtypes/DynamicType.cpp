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
#include <dds/core/xtypes/DynamicType.hpp>
#include <dds/core/xtypes/AliasType.hpp>
#include <dds/core/xtypes/CollectionTypes.hpp>
#include <dds/core/xtypes/EnumType.hpp>
#include <dds/core/xtypes/StructType.hpp>

using namespace dds::core::xtypes;

namespace pyrti {

py::object py_cast_type(dds::core::xtypes::DynamicType& dt)
{
    switch (dt.kind().underlying()) {
    case dds::core::xtypes::TypeKind::inner_enum::ALIAS_TYPE: {
        auto resolved = rti::core::xtypes::resolve_alias(dt);
        return py_cast_type(resolved);
    }
    case dds::core::xtypes::TypeKind::ARRAY_TYPE:
        return py::cast(static_cast<dds::core::xtypes::ArrayType&>(dt));
    case dds::core::xtypes::TypeKind::ENUMERATION_TYPE:
        return py::cast(static_cast<dds::core::xtypes::EnumType&>(dt));
    case dds::core::xtypes::TypeKind::SEQUENCE_TYPE:
        return py::cast(static_cast<dds::core::xtypes::SequenceType&>(dt));
    case dds::core::xtypes::TypeKind::STRUCTURE_TYPE:
        return py::cast(static_cast<dds::core::xtypes::StructType&>(dt));
    case dds::core::xtypes::TypeKind::STRING_TYPE:
        return py::cast(static_cast<dds::core::xtypes::StringType&>(dt));
    case dds::core::xtypes::TypeKind::WSTRING_TYPE:
        return py::cast(static_cast<dds::core::xtypes::StringType&>(dt));
    case dds::core::xtypes::TypeKind::BOOLEAN_TYPE:
        return py::cast(
                static_cast<dds::core::xtypes::PrimitiveType<bool>&>(dt));
    case dds::core::xtypes::TypeKind::UINT_8_TYPE:
        return py::cast(
                static_cast<dds::core::xtypes::PrimitiveType<uint8_t>&>(dt));
    case dds::core::xtypes::TypeKind::INT_16_TYPE:
        return py::cast(
                static_cast<dds::core::xtypes::PrimitiveType<int16_t>&>(dt));
    case dds::core::xtypes::TypeKind::UINT_16_TYPE:
        return py::cast(
                static_cast<dds::core::xtypes::PrimitiveType<uint16_t>&>(dt));
    case dds::core::xtypes::TypeKind::INT_32_TYPE:
        return py::cast(
                static_cast<dds::core::xtypes::PrimitiveType<int32_t>&>(dt));
    case dds::core::xtypes::TypeKind::UINT_32_TYPE:
        return py::cast(
                static_cast<dds::core::xtypes::PrimitiveType<uint32_t>&>(dt));
    case dds::core::xtypes::TypeKind::INT_64_TYPE:
        return py::cast(static_cast<
                        dds::core::xtypes::PrimitiveType<rti::core::int64>&>(
                dt));
    case dds::core::xtypes::TypeKind::UINT_64_TYPE:
        return py::cast(static_cast<
                        dds::core::xtypes::PrimitiveType<rti::core::uint64>&>(
                dt));
    case dds::core::xtypes::TypeKind::FLOAT_32_TYPE:
        return py::cast(
                static_cast<dds::core::xtypes::PrimitiveType<float>&>(dt));
    case dds::core::xtypes::TypeKind::FLOAT_64_TYPE:
        return py::cast(
                static_cast<dds::core::xtypes::PrimitiveType<double>&>(dt));
    case dds::core::xtypes::TypeKind::FLOAT_128_TYPE:
        return py::cast(static_cast<dds::core::xtypes::PrimitiveType<
                                rti::core::LongDouble>&>(dt));
    case dds::core::xtypes::TypeKind::CHAR_8_TYPE:
        return py::cast(
                static_cast<dds::core::xtypes::PrimitiveType<char>&>(dt));
#if rti_connext_version_gte(6, 0, 0, 0)
    case dds::core::xtypes::TypeKind::CHAR_16_TYPE:
        return py::cast(
                static_cast<dds::core::xtypes::PrimitiveType<DDS_Wchar>&>(dt));
#endif
    default:
        return py::cast(dt);
    }
}

template<>
void init_class_defs(py::class_<DynamicType>& cls)
{
    cls.def_property_readonly("kind", &DynamicType::kind, "Get the type kind.")
            .def_property_readonly("name", &DynamicType::name, "Gets the name.")
            .def(
                    "is_primitive_type",
                    [](const DynamicType& dt) {
                        return dds::core::xtypes::is_primitive_type(dt);
                    },
                    "Determines if this DynamicType is a PrimitiveType")
            .def(
                    "is_constructed_type",
                    [](const DynamicType& dt) {
                        return dds::core::xtypes::is_constructed_type(dt);
                    },
                    "Determines if this DynamicType is a constructed type.")
            .def(
                    "is_collection_type",
                    [](const DynamicType& dt) {
                        return dds::core::xtypes::is_collection_type(dt);
                    },
                    "Determins if this DynamicType is a CollectionType.")
            .def(
                    "is_aggregation_type",
                    [](const DynamicType& dt) {
                        return dds::core::xtypes::is_aggregation_type(dt);
                    },
                    "Determines if this DynamicType is an aggregation type.")
            .def("print_idl",
                 &rti::core::xtypes::print_idl,
                 py::arg("index") = 0,
                 "Prints the IDL representation of this type to the standard "
                 "output.")
            .def(
                "__str__",
                [](const DynamicType& t) {
                    return rti::core::xtypes::to_string(t);
                },
                "DynamicData value to string.")
#if rti_connext_version_gte(6, 0, 1, 0)
            .def(
                "to_string",
                [](const DynamicType& t, const rti::core::xtypes::DynamicTypePrintFormatProperty& prop) {
                    return rti::core::xtypes::to_string(t, prop);
                },
                py::arg_v(
                    "format",
                    rti::core::xtypes::DynamicTypePrintFormatProperty(),
                    "DynamicTypePrintFormatProperty()"),
                "Convert DynamicType to string with print format.")
#endif
            .def(py::self == py::self,
                 "Compare DynamicType objects for equality.")
            .def(py::self != py::self,
                 "Compare DynamicType objects for inequality.")
            .def_static(
                "from_xml",
                [](
                const std::string& file_name,
                const std::string& type_name,
                dds::core::optional<std::vector<std::string>>& include_paths,
                uint32_t unbounded_str_max,
                uint32_t unbounded_seq_max) {
                    DDS_TypeCodeFactory* factory = DDS_TypeCodeFactory_get_instance();
                    DDS_ExceptionCode_t ex;
                    DDS_StringSeq native_paths = DDS_SEQUENCE_INITIALIZER;
                    rti::core::detail::NativeSequenceAdapter<char*>
                        paths_adapter(native_paths);
                    if (has_value(include_paths)) {
                        rti::core::native_conversions::to_native(native_paths, get_value(include_paths));
                    }
                    DDS_TypeCode* tc = DDS_TypeCodeFactory_create_tc_from_xml_file(
                            factory,
                            file_name.c_str(),
                            type_name.c_str(),
                            &native_paths,
                            unbounded_str_max,
                            unbounded_seq_max,
                            &ex
                        );
                    rti::core::check_tc_ex_code(ex, "DynamicType.from_xml error");
                    return py_cast_type(
                        rti::core::native_conversions::cast_from_native<DynamicType>(*tc));
                },
                py::arg("filename"),
                py::arg("typename"),
                py::arg("include_paths") = py::none(),
                py::arg("unbounded_string_max_len") = 255,
                py::arg("unbounded_seq_max_len") = 100,
                py::return_value_policy::reference,
                "Load a DynamicType from an XML file."
            );
}

template<>
void process_inits<DynamicType>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DynamicType>(m, "DynamicType");
    });
}

}  // namespace pyrti
