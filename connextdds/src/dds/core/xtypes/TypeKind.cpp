#include "PyConnext.hpp"
#include <PySafeEnum.hpp>
#include <dds/core/xtypes/TypeKind.hpp>

namespace py = pybind11;

using namespace dds::core::xtypes;

namespace pyrti {

template<>
void process_inits<TypeKind>(py::module& m, ClassInitList&) {
    auto tk = init_dds_safe_enum<TypeKind_def>(m, "TypeKind");

    py::enum_<TypeKind::type>(tk, "TypeKind")
        .value(
            "NO_TYPE",
            TypeKind::type::NO_TYPE,
            "Sentinel indicating no value."
        )
        .value(
            "PRIMITIVE_TYPE",
            TypeKind::type::PRIMITIVE_TYPE,
            "Primitive type."
        )
        .value(
            "CONSTRUCTED_TYPE",
            TypeKind::type::CONSTRUCTED_TYPE,
            "Constructed type."
        )
        .value(
            "COLLECTION_TYPE",
            TypeKind::type::COLLECTION_TYPE,
            "Collection type."
        )
        .value(
            "AGGREGATION_TYPE",
            TypeKind::type::AGGREGATION_TYPE,
            "Aggregation type."
        )
        .value(
            "ANNOTATION_TYPE",
            TypeKind::type::ANNOTATION_TYPE,
            "Annotation type."
        )
        .value(
            "BOOLEAN_TYPE",
            TypeKind::type::BOOLEAN_TYPE,
            "Boolean type."
        )
        .value(
            "UINT_8_TYPE",
            TypeKind::type::UINT_8_TYPE,
            "Unsigned 8-bit int type."
        )
        .value(
            "INT_16_TYPE",
            TypeKind::type::INT_16_TYPE,
            "Signed 16-bit int type."
        )
        .value(
            "UINT_16_TYPE",
            TypeKind::type::UINT_16_TYPE,
            "Unsigned 16-bit int type."
        )
        .value(
            "INT_32_TYPE",
            TypeKind::type::INT_32_TYPE,
            "Signed 32-bit int type."
        )
        .value(
            "UINT_32_TYPE",
            TypeKind::type::UINT_32_TYPE,
            "Unsigned 32-bit int type."
        )
        .value(
            "INT_64_TYPE",
            TypeKind::type::INT_64_TYPE,
            "Signed 64-bit int type."
        )
        .value(
            "UINT_64_TYPE",
            TypeKind::type::UINT_64_TYPE,
            "Unsigned 64-bit int type"
        )
        .value(
            "FLOAT_32_TYPE",
            TypeKind::type::FLOAT_32_TYPE,
            "32-bit floating point type."
        )
        .value(
            "FLOAT_64_TYPE",
            TypeKind::type::FLOAT_64_TYPE,
            "64-bit floating point type."
        )
        .value(
            "FLOAT_128_TYPE",
            TypeKind::type::FLOAT_128_TYPE,
            "128-bit floating point type."
        )
        .value(
            "CHAR_8_TYPE",
            TypeKind::type::CHAR_8_TYPE,
            "8-bit character type."
        )
#if rti_connext_version_gte(6, 0, 0)
        .value(
            "CHAR_16_TYPE",
            TypeKind::type::CHAR_16_TYPE,
            "16-bit character type."
        )
#endif
        .value(
            "ENUMERATION_TYPE",
            TypeKind::type::ENUMERATION_TYPE,
            "Enumeration type."
        )
        .value(
            "BITSET_TYPE",
            TypeKind::type::BITSET_TYPE,
            "Bitset type."
        )
        .value(
            "ALIAS_TYPE",
            TypeKind::type::ALIAS_TYPE,
            "Alias type."
        )
        .value(
            "ARRAY_TYPE",
            TypeKind::type::ARRAY_TYPE,
            "Array type."
        )
        .value(
            "SEQUENCE_TYPE",
            TypeKind::type::SEQUENCE_TYPE,
            "Sequence type."
        )
        .value(
            "STRING_TYPE",
            TypeKind::type::STRING_TYPE,
            "String type."
        )
        .value(
            "WSTRING_TYPE",
            TypeKind::type::WSTRING_TYPE,
            "Wide character string type."
        )
        .value(
            "MAP_TYPE",
#if rti_connext_version_lt(6, 0, 1)
            TypeKind::type::MAP_TYPE,
#else
            TypeKind::type::MAP_TYPE_OMG,
#endif
            "Map type"
        )
        .value(
            "UNION_TYPE",
            TypeKind::type::UNION_TYPE,
            "Union type."
        )
        .value(
            "STRUCTURE_TYPE",
            TypeKind::type::STRUCTURE_TYPE,
            "Structure type."
        )
        .value(
            "UNION_FWD_DECL_TYPE",
            TypeKind::type::UNION_FWD_DECL_TYPE,
            "Forward declaration of a union type."
        )
        .value(
            "STRUCTURE_FWD_DECL_TYPE",
            TypeKind::type::STRUCTURE_FWD_DECL_TYPE,
            "Forward declaration of structure type."
        )
        .export_values();
}

}

