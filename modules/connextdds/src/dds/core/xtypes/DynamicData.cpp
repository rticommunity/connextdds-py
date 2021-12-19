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
#include "PySeq.hpp"
#include <pybind11/numpy.h>
#include <dds/core/xtypes/DynamicData.hpp>
#include <dds/core/QosProvider.hpp>
#include "PyInitType.hpp"
#include "PyDynamicTypeMap.hpp"
#include "PyInitOpaqueTypeContainers.hpp"

using namespace dds::core::xtypes;
using namespace dds::topic;

INIT_OPAQUE_TYPE_CONTAINERS(dds::core::xtypes::DynamicData);

namespace pyrti {

struct DynamicDataNestedIndex {
    enum IdType { INT, STRING };

    std::list<rti::core::xtypes::LoanedDynamicData> loan_list;
    IdType index_type;
    int int_index;
    std::string string_index;

    ~DynamicDataNestedIndex()
    {
        while (!loan_list.empty())
            loan_list.pop_back();
    }
};


static rti::core::xtypes::DynamicDataMemberInfo get_member_info(
    const DynamicData& dd,
    const std::string& member)
{
    rti::core::xtypes::DynamicDataMemberInfo mi;

    if (!dd.member_exists_in_type(member)) {
        throw dds::core::InvalidArgumentError("member name " + member + " does not exist in type");
    }

    auto rc = DDS_DynamicData_get_member_info(
        &dd.native(),
        &mi.native(),
        member.c_str(),
        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED);

    rti::core::check_return_code(rc, "DynamicData member info error (name)");

    return mi;
}


static rti::core::xtypes::DynamicDataMemberInfo get_member_info(
    const DynamicData& dd,
    uint32_t index)
{
    rti::core::xtypes::DynamicDataMemberInfo mi;

    if (!dd.member_exists_in_type(index)) {
        throw dds::core::InvalidArgumentError("member id " + std::to_string(index) + " does not exist in type");
    }

    auto rc = DDS_DynamicData_get_member_info(
        &dd.native(),
        &mi.native(),
        nullptr,
        index);

    rti::core::check_return_code(rc, "DynamicData member info error (index)");

    return mi;
}


static TypeKind::inner_enum resolve_type_kind(DynamicData& dd)
{
    const DynamicType& actual_type = rti::core::xtypes::resolve_alias(dd.type());
    return actual_type.kind().underlying();
}


template<typename T>
static TypeKind::inner_enum resolve_member_type_kind(
        DynamicData& dd,
        TypeKind::inner_enum kind,
        const T& key)
{
    if (kind == TypeKind::ALIAS_TYPE) {
        auto& alias = static_cast<const AliasType&>(dd.loan_value(key).get().type());
        const DynamicType& actual_type = rti::core::xtypes::resolve_alias(alias);
        kind = actual_type.kind().underlying();
    }
    return kind;
}


static int calculate_multidimensional_offset(
        const DynamicType& type,
        std::vector<int>& index)
{
    auto kind = type.kind().underlying();
    if (kind != TypeKind::ARRAY_TYPE) {
        throw py::type_error("Invalid index access for this type.");
    }
    auto& array_type = static_cast<const ArrayType&>(type);
    if (array_type.dimension_count() != index.size()) {
        throw py::index_error("Invalid dimensions specified for index access.");
    }

    int offset = 0;
    for (int i = 0; i < index.size(); ++i) {
        auto dim_index = index[i];
        if (dim_index >= array_type.dimension(i)) {
            throw py::index_error("Invalid index for dimension.");
        }
        if (dim_index != 0) {
            int product = dim_index;
            for (int j = i + 1; j < index.size(); ++j) {
                product *= array_type.dimension(j);
            }
            offset += product;
        }
    }
    return offset;
}


static int get_collection_dim_count(DynamicData& data) {
    auto kind = resolve_type_kind(data);
    int dim_count = 1;

    if (kind == TypeKind::ARRAY_TYPE) {
        const ArrayType& array_type = static_cast<const ArrayType&>(data.type());
        dim_count = array_type.dimension_count();
    }
    else if (kind != TypeKind::SEQUENCE_TYPE) {
        throw dds::core::InvalidArgumentError("type does not support subscript");
    }
    return dim_count;
}


static DynamicData& resolve_collection_member(
        DynamicData& data,
        const std::string& indices,
        DynamicDataNestedIndex& id,
        bool is_key)
{
    std::stringstream ss(indices);
    char delimiter;
    std::vector<int> index_list;
    int found_index;
    int dim_count = get_collection_dim_count(data);
    DynamicData* current = &data;

    enum ParseState {
        OPEN,
        INDEX,
        CLOSE_OR_SEP
    };

    ParseState state = ParseState::OPEN;

    while (ss) {
        int next = ss.peek();
        switch(next) {
        case '[':
            if (state != ParseState::OPEN) {
                throw dds::core::InvalidArgumentError("index parse error: '[' not expected");
            }
            ss >> delimiter;
            state = ParseState::INDEX;
            break;
        case ']':
            if (state != ParseState::CLOSE_OR_SEP) {
                throw dds::core::InvalidArgumentError("index parse error: ']' not expected");
            }
            ss >> delimiter;
            state = ParseState::OPEN;
            if (dim_count == index_list.size()) {
                int offset = index_list[0];
                if (dim_count > 1) {
                    offset = calculate_multidimensional_offset(current->type(), index_list);
                }
                next = ss.peek();
                if (next == '[' || !is_key) {
                    id.loan_list.push_back(current->loan_value(offset));
                    current = &id.loan_list.back().get();
                    if (next == '[') {
                        dim_count = get_collection_dim_count(*current);
                    }
                }
                else {
                    // set key's integer index
                    id.index_type = DynamicDataNestedIndex::INT;
                    id.int_index = offset + 1;
                }
            }
            else {
                throw dds::core::InvalidArgumentError("index error: incorrect dimensions in subscript");
            }
            break;
        case ',':
            if (state != ParseState::CLOSE_OR_SEP) {
                throw dds::core::InvalidArgumentError("index parse error: ',' not expected");
            }
            ss >> delimiter;
            state = ParseState::INDEX;
            break;
        default:
            if (next != std::stringstream::traits_type::eof()) {
                if (state != ParseState::INDEX) {
                    throw dds::core::InvalidArgumentError("index parse error");
                }
                ss >> found_index;
                index_list.push_back(found_index);
                state = ParseState::CLOSE_OR_SEP;
            }
            break;
        }
        if (state != ParseState::OPEN) ss >> std::ws;
    }
    if (state != ParseState::OPEN || !ss.eof()) {
        throw dds::core::InvalidArgumentError("index error");
    }

    return *current;
}


static DynamicData& resolve_nested_member(
        DynamicData& dd,
        const std::string& key,
        DynamicDataNestedIndex& id)
{
    std::stringstream ss(key);
    std::string key_field;
    DynamicData* current = &dd;
    std::list<std::string> tokens;

    while (std::getline(ss, key_field, '.')) {
        tokens.push_back(key_field);
    }
    key_field = tokens.back();
    tokens.pop_back();

    for (auto& token : tokens) {
        size_t pos = token.find('[');
        if (pos != std::string::npos) {
            int index;
            id.loan_list.push_back(
                current->loan_value(token.substr(0, pos)));
            current = &id.loan_list.back().get();
            current = &resolve_collection_member(*current, token.substr(pos, std::string::npos), id, false);
        } else {
            id.loan_list.push_back(current->loan_value(token));
            current = &id.loan_list.back().get();
        }
    }

    size_t pos = key_field.find('[');
    if (pos != std::string::npos) {
        int index;
        id.loan_list.push_back(
            current->loan_value(key_field.substr(0, pos)));
        current = &id.loan_list.back().get();
        current = &resolve_collection_member(*current, key_field.substr(pos, std::string::npos), id, true);
    } else {
        id.index_type = DynamicDataNestedIndex::STRING;
        id.string_index = key_field;
    }

    return *current;
}


static
EnumMember resolve_enum_member_in_type(const EnumType& enum_type, int32_t ordinal) {
    return enum_type.member(enum_type.find_member_by_ordinal(ordinal));
}


static 
UnionMember get_union_member(const UnionType& ut, const std::string& key) {
    return ut.member(key);
}


static 
UnionMember get_union_member(const UnionType& ut, const int32_t discriminator) {
    return ut.member(ut.find_member_by_label(discriminator));
}


template<typename T>
static
EnumMember get_enum_member_base(const DynamicData& dd, const T& key, int32_t ordinal) {
    switch(dd.type_kind().underlying()) {
    case TypeKind::STRUCTURE_TYPE: {
        auto& struct_type = static_cast<const StructType&>(dd.type());
        return resolve_enum_member_in_type(
            static_cast<const EnumType&>(struct_type.member(key).type()),
            ordinal);
    }
    case TypeKind::ARRAY_TYPE:
    case TypeKind::SEQUENCE_TYPE: {
        auto& collection_type = static_cast<const CollectionType&>(dd.type());
        return resolve_enum_member_in_type(
            static_cast<const EnumType&>(collection_type.content_type()),
            ordinal);
    }
    case TypeKind::UNION_TYPE: {
        auto& union_type = static_cast<const UnionType&>(dd.type());
        return resolve_enum_member_in_type(
            static_cast<const EnumType&>(
                get_union_member(union_type, key).type()),
            ordinal);
    }
    default:
        throw dds::core::InvalidArgumentError("Cannot get enum member from this data type");
    }
}


template<typename T>
static
EnumMember get_enum_member(DynamicData& dd, const T& key, int32_t ordinal) {
    return get_enum_member_base(dd, key, ordinal);
}


template<>
EnumMember get_enum_member(DynamicData& dd, const std::string& key, int32_t ordinal) {
    DynamicDataNestedIndex index;
    DynamicData& parent = resolve_nested_member(dd, key, index);
    if (index.index_type == DynamicDataNestedIndex::IdType::INT) {
        return get_enum_member_base(parent, index.int_index, ordinal);
    }
    else {
        return get_enum_member_base(parent, index.string_index, ordinal);
    }
}


template<typename T>
std::vector<DynamicData> get_complex_values(DynamicData& data, const T& key)
{
    std::vector<DynamicData> v;
    auto info = get_member_info(data, key);
    if ((info.member_kind().underlying() & TypeKind::COLLECTION_TYPE)
        && !(info.element_kind().underlying() & TypeKind::PRIMITIVE_TYPE)) {
        DynamicData member = data.value<DynamicData>(key);
        for (uint32_t i = 1; i <= info.element_count(); ++i) {
            v.push_back(member.value<DynamicData>(i));
        }
        return v;
    } else {
        throw py::key_error(
                "member is not a collection of non-primitive values.");
    }
}


template<typename T>
static void set_member(
        DynamicData& dd,
        TypeKind::inner_enum kind,
        const T& key,
        py::object& value);


template<typename T>
static void clear_complex_collection(DynamicData& data, const T& key) {
    auto loan = data.loan_value(key);
    DynamicData& member = loan.get();
    auto& collection_type = member.type();
    loan.return_loan();
    data.value(key, DynamicData(collection_type));
}


template<typename T>
void set_complex_values(DynamicData& data, const T& key, py::iterable& values)
{
    auto info = get_member_info(data, key);
    if ((info.member_kind().underlying() & TypeKind::COLLECTION_TYPE)
        && !(info.element_kind().underlying() & TypeKind::PRIMITIVE_TYPE)) {
        auto elem_kind = info.element_kind().underlying();
        if (info.element_count() > 0) clear_complex_collection(data, key);
        auto loan = data.loan_value(key);
        DynamicData& member = loan.get();
        int i = 1;
        for (auto handle : values) {
            auto obj = py::cast<py::object>(handle);
            set_member(member, elem_kind, i++, obj);
        }
    } else {
        throw py::key_error(
                "member is not a collection of non-primitive values.");
    }
}


/*
    EXPERIMENTAL!!!

    std::vector<T>::resize forces initialization of added elements. This
    workaround relies on compiler optimizations to avoid this forced
    initialization when the vector template type is a primitive.
 */
template<typename T>
static void resize_no_init(std::vector<T>& v, ssize_t newSize) {
    /*
        A continguous array of this struct must have the same memory layout
        as a type T array or this optimization will not work (and the assert
        following the struct definition will fail)
    */
    struct vector_no_init_elem {
        typename std::vector<T>::value_type data;
        vector_no_init_elem() {}
    };
    static_assert(
        sizeof(vector_no_init_elem[10]) == sizeof(typename std::vector<T>::value_type[10]), 
        "alignment error");

    /*
        To make use of this compiler optimization, we will use the allocator
        from the primitive vector and rebind it for our struct with an empty
        default constructor. The compiler will optimize away the overhead of
        default initialization on calls to vector_no_init::resize.
    */
    using vector_allocator = 
        typename std::vector<T>::allocator_type;

    using vector_allocator_traits = 
        std::allocator_traits<vector_allocator>;

    using no_init_allocator = 
        typename vector_allocator_traits::template rebind_alloc<vector_no_init_elem>;

    using vector_no_init = std::vector<vector_no_init_elem, no_init_allocator>;

    /*
        Because the memory layout of std::vector<T> matches that of our vector_no_init
        we can perform a reinterpret cast on our input argument and use the
        optimized resize. Note that added vector entries will contain unitialized
        values, but since this function will only be used when returning vectors filled
        with values from an existing DynamicData collection, the unitialized values
        should never be accessible to the calling application.
    */
    reinterpret_cast<vector_no_init&>(v).resize(newSize);
}


template<typename V, typename K>
static std::vector<V> get_collection_buffer_member(
        const DynamicData& dd,
        const K& key) {
    std::vector<V> values;
    auto mi = get_member_info(dd, key);
    resize_no_init(values, mi.element_count());
    dd.get_values<V>(key, values);
    return values;
}


template<typename T>
static py::object get_collection_member(
        DynamicData& dd,
        TypeKind::inner_enum kind,
        const T& key)
{
    kind = resolve_member_type_kind(dd, kind, key);
    switch (kind) {
    case TypeKind::BOOLEAN_TYPE: {
        std::vector<bool> retval;
        auto dd_values = dd.get_values<uint8_t>(key);
        for (auto value : dd_values) {
            retval.push_back(value != 0);
        }
        return py::cast(retval);
    }
    case TypeKind::UINT_8_TYPE:
        return py::cast(get_collection_buffer_member<uint8_t, T>(dd, key));
    case TypeKind::INT_16_TYPE:
        return py::cast(get_collection_buffer_member<int16_t, T>(dd, key));
    case TypeKind::UINT_16_TYPE:
        return py::cast(get_collection_buffer_member<uint16_t, T>(dd, key));
    case TypeKind::INT_32_TYPE:
        return py::cast(get_collection_buffer_member<int32_t, T>(dd, key));
    case TypeKind::ENUMERATION_TYPE: {
        std::vector<EnumMember> retval;
        auto loaned_collection = dd.loan_value(key);
        auto& collection = loaned_collection.get();
        auto& collection_type = static_cast<const CollectionType&>(collection.type());
        auto& enum_type = static_cast<const EnumType&>(collection_type.content_type());
        retval.reserve(collection.member_count());
        for (int i = 1; i <= collection.member_count(); ++i) {
            retval.push_back(
                enum_type.member(
                    enum_type.find_member_by_ordinal(
                        collection.template value<int32_t>(i)
                    )
                )
            );
        }
        return py::cast(retval);
    }
    case TypeKind::UINT_32_TYPE:
        return py::cast(get_collection_buffer_member<uint32_t, T>(dd, key));
    case TypeKind::INT_64_TYPE:
        return py::cast(get_collection_buffer_member<rti::core::int64, T>(dd, key));
    case TypeKind::UINT_64_TYPE:
        return py::cast(get_collection_buffer_member<rti::core::uint64, T>(dd, key));
    case TypeKind::FLOAT_32_TYPE:
        return py::cast(get_collection_buffer_member<float, T>(dd, key));
    case TypeKind::FLOAT_64_TYPE:
        return py::cast(get_collection_buffer_member<double, T>(dd, key));
    case TypeKind::FLOAT_128_TYPE: {
        std::vector<rti::core::LongDouble> retval;
        auto loaned_member = dd.loan_value(key);
        DynamicData& member = loaned_member.get();
        for (int i = 1; i <= member.member_count(); ++i) {
            auto ld = member.value<rti::core::LongDouble>(i);
            retval.push_back(ld);
        }
        return py::cast(retval);
    }
    case TypeKind::CHAR_8_TYPE:
        return py::cast(get_collection_buffer_member<char, T>(dd, key));
#if rti_connext_version_gte(6, 0, 0, 0)
    case TypeKind::CHAR_16_TYPE: {
        std::vector<wchar_t> retval;
        auto dd_values = dd.get_values<uint16_t>(key);
        for (auto c : dd_values) {
            retval.push_back(static_cast<wchar_t>(c));
        }
        return py::cast(retval);
    }
#endif
    case TypeKind::STRING_TYPE: {
        std::vector<std::string> retval;
        auto loaned_member = dd.loan_value(key);
        DynamicData& member = loaned_member.get();
        for (int i = 1; i <= member.member_count(); ++i) {
            retval.push_back(member.value<std::string>(i));
        }
        return py::cast(retval);
    }
    case TypeKind::WSTRING_TYPE: {
        std::vector<std::wstring> retval;
        auto loaned_member = dd.loan_value(key);
        DynamicData& member = loaned_member.get();
        for (int i = 1; i <= member.member_count(); ++i) {
            std::wstring val;
            auto dd_wstr = member.get_values<DDS_Wchar>(i);
            for (auto c : dd_wstr) {
                wchar_t wc = static_cast<wchar_t>(c);
                val.push_back(wc);
            }
            retval.push_back(val);
        }
        return py::cast(retval);
    }
    default:
        return py::cast(get_complex_values(dd, key));
    }
}


template<typename T>
static py::object get_member(
        DynamicData& dd,
        TypeKind::inner_enum kind,
        const T& key,
        bool dict_access = false)
{
    kind = resolve_member_type_kind(dd, kind, key);
    switch (kind) {
    case TypeKind::BOOLEAN_TYPE:
        return py::cast(dd.value<bool>(key));
    case TypeKind::UINT_8_TYPE:
        return py::cast(dd.value<uint8_t>(key));
    case TypeKind::INT_16_TYPE:
        return py::cast(dd.value<int16_t>(key));
    case TypeKind::UINT_16_TYPE:
        return py::cast(dd.value<uint16_t>(key));
    case TypeKind::INT_32_TYPE:
        return py::cast(dd.value<int32_t>(key));
    case TypeKind::ENUMERATION_TYPE:
        return py::cast(get_enum_member(dd, key, dd.value<int32_t>(key)));
    case TypeKind::UINT_32_TYPE:
        return py::cast(dd.value<uint32_t>(key));
    case TypeKind::INT_64_TYPE:
        return py::cast(dd.value<rti::core::int64>(key));
    case TypeKind::UINT_64_TYPE:
        return py::cast(dd.value<rti::core::uint64>(key));
    case TypeKind::FLOAT_32_TYPE:
        return py::cast(dd.value<float>(key));
    case TypeKind::FLOAT_64_TYPE:
        return py::cast(dd.value<double>(key));
    case TypeKind::FLOAT_128_TYPE:
        return py::cast(dd.value<rti::core::LongDouble>(key));
    case TypeKind::CHAR_8_TYPE:
        return py::cast(dd.value<char>(key));
#if rti_connext_version_gte(6, 0, 0, 0)
    case TypeKind::CHAR_16_TYPE:
        return py::cast(static_cast<wchar_t>(dd.value<DDS_Wchar>(key)));
#endif
    case TypeKind::STRING_TYPE:
        return py::cast(dd.value<std::string>(key));
    case TypeKind::WSTRING_TYPE: {
        std::vector<DDS_Wchar> v(dd.get_values<DDS_Wchar>(key));
        std::wstring s;
        for (auto c : v) {
            s.push_back(static_cast<wchar_t>(c));
        }
        return py::cast(s);
    }
    case TypeKind::ARRAY_TYPE:
    case TypeKind::SEQUENCE_TYPE: {
        if (dict_access) {
            auto mi = get_member_info(dd, key);
            return get_collection_member(dd, mi.element_kind().underlying(), key);
        }
    }
    default:
        return py::cast(dd.value<DynamicData>(key));
    }
}


template<typename T>
static bool validate_buffer_type(const py::buffer_info& info) {
    /*
        If this buffer is incompatible, throw a type exception.
    */
    if (info.ndim != 1 || info.strides[0] % static_cast<ssize_t>(sizeof(T)))
        throw py::type_error("Only valid 1D buffers are allowed");
    if (!py::detail::compare_buffer_info<T>::compare(info))
        throw py::type_error("Format mismatch (Python: " + info.format + " C++: " + py::format_descriptor<T>::format() + ")");
    /*
        This optimization only works with contiguous buffer objects; for a "step"
        that is greater than 1, fall back to the default pybind11 stl_bind 
        conversion.
    */
    return (info.strides[0] / static_cast<ssize_t>(sizeof(T))) == 1;
}


template<typename T, typename F>
static bool set_buffer_values(
    DynamicData& dd,
    const char* field_name,
    const int field_index,
    const py::buffer_info& info,
    F func)
{
    DDS_DynamicData* native_ptr = &dd.native();
    if (!validate_buffer_type<T>(info)) return false;
    T* ptr = static_cast<T*>(info.ptr);
    DDS_UnsignedLong len = info.shape[0];
    if (func(
            native_ptr,
            field_name,
            field_index,
            len,
            ptr) != DDS_RETCODE_OK) {
        throw dds::core::IllegalOperationError("Failed to set buffer collection member");
    }
    return true;
}


template<typename T, typename F>
static bool set_buffer_values(
    DynamicData& dd,
    const std::string& key,
    const py::buffer_info& info,
    F func)
{
    return set_buffer_values<T>(
        dd,
        key.c_str(),
        DDS_DYNAMIC_DATA_MEMBER_ID_UNSPECIFIED,
        info,
        func);
}


template<typename T, typename F>
static bool set_buffer_values(
    DynamicData& dd,
    const int& key,
    const py::buffer_info& info,
    F func)
{
    return set_buffer_values<T>(
        dd,
        nullptr,
        key,
        info,
        func);
}


template<typename T>
static bool set_buffer_collection_member(
        DynamicData& dd,
        TypeKind::inner_enum kind,
        const T& key,
        py::buffer& values)
{
    auto info = values.request();
    switch (kind) {
    case TypeKind::UINT_8_TYPE:
        return set_buffer_values<uint8_t>(dd, key, info, DDS_DynamicData_set_octet_array);
    case TypeKind::INT_16_TYPE:
        return set_buffer_values<int16_t>(dd, key, info, DDS_DynamicData_set_short_array);
    case TypeKind::UINT_16_TYPE:
        return set_buffer_values<uint16_t>(dd, key, info, DDS_DynamicData_set_ushort_array);
    case TypeKind::INT_32_TYPE:
        return set_buffer_values<int32_t>(dd, key, info, DDS_DynamicData_set_long_array);
    case TypeKind::UINT_32_TYPE:
        return set_buffer_values<uint32_t>(dd, key, info, DDS_DynamicData_set_ulong_array);
    case TypeKind::INT_64_TYPE:
        return set_buffer_values<rti::core::int64>(dd, key, info, DDS_DynamicData_set_longlong_array);
    case TypeKind::UINT_64_TYPE:
        return set_buffer_values<rti::core::uint64>(dd, key, info, DDS_DynamicData_set_ulonglong_array);
    case TypeKind::FLOAT_32_TYPE:
        return set_buffer_values<float>(dd, key, info, DDS_DynamicData_set_float_array);
    case TypeKind::FLOAT_64_TYPE:
        return set_buffer_values<double>(dd, key, info, DDS_DynamicData_set_double_array);
    case TypeKind::CHAR_8_TYPE:
        return set_buffer_values<char>(dd, key, info, DDS_DynamicData_set_char_array);
    default:
        throw dds::core::InvalidArgumentError("Not a valid buffer type");
    }   
    return true;
}


template<typename T>
static void set_collection_member(
        DynamicData& dd,
        TypeKind::inner_enum kind,
        const T& key,
        py::object& values)
{
    kind = resolve_member_type_kind(dd, kind, key);
    if (py::isinstance<py::buffer>(values)) {
        auto buffer = py::cast<py::buffer>(values);
        if (set_buffer_collection_member(dd, kind, key, buffer)) return;
    }
    switch (kind) {
    case TypeKind::BOOLEAN_TYPE: {
        auto v = py::cast<std::vector<bool>>(values);
        std::vector<uint8_t> dd_values;
        dd_values.reserve(v.size());
        for (auto b : v) {
            dd_values.push_back(b);
        }
        dd.set_values<uint8_t>(key, dd_values);
        break;
    }
    case TypeKind::UINT_8_TYPE:
        dd.set_values<uint8_t>(key, py::cast<std::vector<uint8_t>>(values));
        break;
    case TypeKind::INT_16_TYPE:
        dd.set_values<int16_t>(key, py::cast<std::vector<int16_t>>(values));
        break;
    case TypeKind::UINT_16_TYPE:
        dd.set_values<uint16_t>(key, py::cast<std::vector<uint16_t>>(values));
        break;
    case TypeKind::INT_32_TYPE:
        dd.set_values<int32_t>(key, py::cast<std::vector<int32_t>>(values));
        break;
    case TypeKind::ENUMERATION_TYPE: {
        auto iterable = py::cast<py::iterable>(values);
        int32_t index = 1;
        auto loan = dd.loan_value(key);
        DynamicData& enum_collection = loan.get();
        auto& collection_type = static_cast<const CollectionType&>(enum_collection.type());
        auto& enum_type = static_cast<const EnumType&>(collection_type.content_type());
        enum_collection.clear_all_members();
        for (auto& value: iterable) {
            auto enum_member = enum_type.member(
                enum_type.find_member_by_ordinal(
                    int32_t(py::cast<py::int_>(value))
                ));
            enum_collection.value<int32_t>(index++, enum_member.ordinal());
        }
        break;
    }
    case TypeKind::UINT_32_TYPE:
        dd.set_values<uint32_t>(key, py::cast<std::vector<uint32_t>>(values));
        break;
    case TypeKind::INT_64_TYPE:
        dd.set_values<rti::core::int64>(
                key,
                py::cast<std::vector<rti::core::int64>>(values));
        break;
    case TypeKind::UINT_64_TYPE:
        dd.set_values<rti::core::uint64>(
                key,
                py::cast<std::vector<rti::core::uint64>>(values));
        break;
    case TypeKind::FLOAT_32_TYPE:
        dd.set_values<float>(key, py::cast<std::vector<float>>(values));
        break;
    case TypeKind::FLOAT_64_TYPE:
        dd.set_values<double>(key, py::cast<std::vector<double>>(values));
        break;
    case TypeKind::FLOAT_128_TYPE: {
        auto v = py::cast<std::vector<rti::core::LongDouble>>(values);
        DynamicData& member = dd.loan_value(key).get();
        size_t i = 1;
        for (auto& ld : v) {
            member.value<rti::core::LongDouble>(i++, ld);
        }
        break;
    }
    case TypeKind::CHAR_8_TYPE:
        dd.set_values<char>(key, py::cast<std::vector<char>>(values));
        break;
#if rti_connext_version_gte(6, 0, 0, 0)
    case TypeKind::CHAR_16_TYPE: {
        auto s = py::cast<std::vector<wchar_t>>(values);
        std::vector<DDS_Wchar> v;
        for (auto c : s) {
            v.push_back(static_cast<DDS_Wchar>(c));
        }
        dd.set_values<DDS_Wchar>(key, v);
        break;
    }
#endif
    case TypeKind::STRING_TYPE: {
        auto v = py::cast<std::vector<std::string>>(values);
        auto loan = dd.loan_value(key);
        DynamicData& member = loan.get();
        for (int i = 0; i < v.size(); ++i) {
            member.value<std::string>(i + 1, v[i]);
        }
        break;
    }
    case TypeKind::WSTRING_TYPE: {
        auto v = py::cast<std::vector<std::wstring>>(values);
        auto loan = dd.loan_value(key);
        DynamicData& member = loan.get();
        for (int i = 0; i < v.size(); ++i) {
            std::vector<DDS_Wchar> str;
            for (auto c : v[i]) {
                str.push_back(static_cast<DDS_Wchar>(c));
            }
            member.set_values<DDS_Wchar>(i + 1, str);
        }
        break;
    }
    case TypeKind::ARRAY_TYPE:
    case TypeKind::SEQUENCE_TYPE: {
        auto iterable = py::cast<py::iterable>(values);
        rti::core::xtypes::LoanedDynamicData loan = dd.loan_value(key);
        DynamicData& member = loan.get();
        rti::core::xtypes::LoanedDynamicData child = member.loan_value(1);
        auto elem_kind = static_cast<const CollectionType&>(child.get().type())
                                 .content_type()
                                 .kind()
                                 .underlying();
        child.return_loan();
        size_t index = 1;
        for (auto collection : iterable) {
            py::object obj = py::cast<py::object>(collection);
            set_collection_member(member, elem_kind, index++, obj);
        }
        break;
    }
    default: {
        auto v = py::cast<py::iterable>(values);
        set_complex_values(dd, key, v);
    }
    }
}


// Forward declare function to allow use in set_member
void update_dynamicdata_object(DynamicData& dd, py::dict& dict);


template<typename T>
static void set_member(
        DynamicData& dd,
        TypeKind::inner_enum kind,
        const T& key,
        py::object& value)
{
    if (value.is_none()) {
        dd.clear_optional_member(key);
        return;
    }

    kind = resolve_member_type_kind(dd, kind, key);

    switch (kind) {
    case TypeKind::BOOLEAN_TYPE:
        dd.value<bool>(key, py::cast<bool>(value));
        break;
    case TypeKind::UINT_8_TYPE:
        dd.value<uint8_t>(key, py::cast<uint8_t>(value));
        break;
    case TypeKind::INT_16_TYPE:
        dd.value<int16_t>(key, py::cast<int16_t>(value));
        break;
    case TypeKind::UINT_16_TYPE:
        dd.value<uint16_t>(key, py::cast<uint16_t>(value));
        break;
    case TypeKind::INT_32_TYPE:
        dd.value<int32_t>(key, py::cast<int32_t>(value));
        break;
    case TypeKind::ENUMERATION_TYPE: {
        auto member = get_enum_member(dd, key, int32_t(py::cast<py::int_>(value)));
        dd.value<int32_t>(key, member.ordinal());
        break;
    }
    case TypeKind::UINT_32_TYPE:
        dd.value<uint32_t>(key, py::cast<uint32_t>(value));
        break;
    case TypeKind::INT_64_TYPE:
        dd.value<rti::core::int64>(key, py::cast<rti::core::int64>(value));
        break;
    case TypeKind::UINT_64_TYPE:
        dd.value<rti::core::uint64>(key, py::cast<rti::core::uint64>(value));
        break;
    case TypeKind::FLOAT_32_TYPE:
        dd.value<float>(key, py::cast<float>(value));
        break;
    case TypeKind::FLOAT_64_TYPE:
        dd.value<double>(key, py::cast<double>(value));
        break;
    case TypeKind::FLOAT_128_TYPE:
        dd.value<rti::core::LongDouble>(
                key,
                py::cast<rti::core::LongDouble>(value));
        break;
    case TypeKind::CHAR_8_TYPE:
        dd.value<char>(key, py::cast<char>(value));
        break;
#if rti_connext_version_gte(6, 0, 0, 0)
    case TypeKind::CHAR_16_TYPE:
        dd.value<DDS_Wchar>(
                key,
                static_cast<DDS_Wchar>(py::cast<wchar_t>(value)));
        break;
#endif
    case TypeKind::STRING_TYPE:
        dd.value<std::string>(key, py::cast<std::string>(value));
        break;
    case TypeKind::WSTRING_TYPE: {
        std::wstring s = py::cast<std::wstring>(value);
        std::vector<DDS_Wchar> v;
        for (auto c : s) {
            v.push_back(static_cast<DDS_Wchar>(c));
        }
        dd.set_values<DDS_Wchar>(key, v);
    } break;
    case TypeKind::ARRAY_TYPE:
    case TypeKind::SEQUENCE_TYPE: {
        if (py::isinstance<DynamicData>(value)) {
            auto& native_value = py::cast<DynamicData&>(value);
            dd.value<DynamicData>(key, native_value);
        } else {
            auto mi = get_member_info(dd, key);
            auto elem_kind = mi.element_kind().underlying();
            set_collection_member(dd, elem_kind, key, value);
        }
        break;
    }
    default: {
        if (py::isinstance<DynamicData>(value)) {
            auto& native_value = py::cast<DynamicData&>(value);
            dd.value<DynamicData>(key, native_value);
        } else {
            auto dd_dict = py::cast<py::dict>(value);
            rti::core::xtypes::LoanedDynamicData loan = dd.loan_value(key);
            DynamicData native_value(loan.get().type());
            loan.return_loan();
            update_dynamicdata_object(native_value, dd_dict);
            dd.value<DynamicData>(key, native_value);
        }
    }
    }
}


static bool test_index(
        const DynamicData& dd,
        TypeKind::inner_enum kind,
        int index,
        const py::object& obj)
{
    switch (kind) {
    case TypeKind::BOOLEAN_TYPE:
        return dd.value<bool>(index) == py::cast<bool>(obj);
    case TypeKind::UINT_8_TYPE:
        return dd.value<uint8_t>(index) == py::cast<uint8_t>(obj);
    case TypeKind::INT_16_TYPE:
        return dd.value<int16_t>(index) == py::cast<int16_t>(obj);
    case TypeKind::UINT_16_TYPE:
        return dd.value<uint16_t>(index) == py::cast<uint16_t>(obj);
    case TypeKind::INT_32_TYPE:
    case TypeKind::ENUMERATION_TYPE:
        return dd.value<int32_t>(index) == py::cast<int32_t>(obj);
    case TypeKind::UINT_32_TYPE:
        return dd.value<uint32_t>(index) == py::cast<uint32_t>(obj);
    case TypeKind::INT_64_TYPE:
        return dd.value<rti::core::int64>(index)
                == py::cast<rti::core::int64>(obj);
    case TypeKind::UINT_64_TYPE:
        return dd.value<rti::core::uint64>(index)
                == py::cast<rti::core::uint64>(obj);
    case TypeKind::FLOAT_32_TYPE:
        return dd.value<float>(index) == py::cast<float>(obj);
    case TypeKind::FLOAT_64_TYPE:
        return dd.value<double>(index) == py::cast<double>(obj);
    case TypeKind::FLOAT_128_TYPE:
        return dd.value<rti::core::LongDouble>(index)
                == py::cast<rti::core::LongDouble>(obj);
    case TypeKind::CHAR_8_TYPE:
        return dd.value<char>(index) == py::cast<char>(obj);
#if rti_connext_version_gte(6, 0, 0, 0)
    case TypeKind::CHAR_16_TYPE:
        return dd.value<DDS_Wchar>(index)
                == static_cast<DDS_Wchar>(py::cast<wchar_t>(obj));
#endif
    case TypeKind::STRING_TYPE:
        return dd.value<std::string>(index) == py::cast<std::string>(obj);
    case TypeKind::WSTRING_TYPE: {
        std::wstring s1 = py::cast<std::wstring>(obj);
        std::vector<DDS_Wchar> v(dd.get_values<DDS_Wchar>(index));
        std::wstring s2;
        for (auto c : v) {
            s2.push_back(static_cast<wchar_t>(c));
        }
        return s1 == s2;
    }
    default:
        return dd.value<DynamicData>(index) == py::cast<DynamicData>(obj);
    }
}


void update_dynamicdata_object(DynamicData& dd, py::dict& dict)
{
    for (auto kv : dict) {
        std::string key = py::cast<std::string>(kv.first);
        auto mi = get_member_info(dd, key);
        auto obj = py::cast<py::object>(kv.second);
        set_member(dd, mi.member_kind().underlying(), key, obj);
    }
}


template<typename T>
static py::object get_value(DynamicData& dd, const T& key)
{
    auto mi = get_member_info(dd, key);
    return get_member(dd, mi.member_kind().underlying(), key);
}


template<typename T>
static py::object get_value_as_dict(DynamicData& dd, const T& key)
{
    auto mi = get_member_info(dd, key);
    return get_member(dd, mi.member_kind().underlying(), key, true);
}


template<typename T>
static py::object get_values(DynamicData& dd, const T& key)
{
    if (!dd.member_exists(key)) {
        throw dds::core::InvalidArgumentError(
                "DynamicData get_value: member does not exist");
    }
    auto mi = get_member_info(dd, key);
    auto kind = resolve_member_type_kind(
            dd,
            mi.member_kind().underlying(),
            mi.member_index());
    if (kind != TypeKind::ARRAY_TYPE && kind != TypeKind::SEQUENCE_TYPE) {
        throw py::type_error(
                "Cannot get collection from non-collection member.");
    }
    return get_collection_member(dd, mi.element_kind().underlying(), key);
}


template<typename T>
static void set_value(DynamicData& dd, const T& key, py::object& value)
{
    auto mi = get_member_info(dd, key);
    TypeKind::type field_type = mi.member_kind().underlying();
    set_member(dd, field_type, key, value);
}


template<typename T>
static void set_values(DynamicData& dd, const T& key, py::object& values)
{
    auto mi = get_member_info(dd, key);
    TypeKind::type field_type = resolve_member_type_kind(
                                    dd,
                                    mi.member_kind().underlying(),
                                    key);
    if (field_type != TypeKind::ARRAY_TYPE
        && field_type != TypeKind::SEQUENCE_TYPE) {
        throw py::type_error(
                "Cannot set multiple values to non-collection member.");
    }
    auto elem_kind = mi.element_kind().underlying();
    set_collection_member(dd, elem_kind, key, values);
}


class PyDynamicDataFieldsIterator {
public:
    PyDynamicDataFieldsIterator(DynamicData& dd, bool reversed) : _dd(dd)
    {
        if (reversed) {
            this->_index = dd.member_count();
            this->_end = 0;
            this->_step = -1;
        } else {
            this->_index = 1;
            this->_end = dd.member_count() + 1;
            this->_step = 1;
        }
    }

    std::string next()
    {
        if (this->_index == this->_end)
            throw py::stop_iteration();
        auto mi = get_member_info(this->_dd, (uint32_t)this->_index);
        auto retval = mi.member_name();
        this->_index += this->_step;
        return retval;
    }

private:
    DynamicData& _dd;
    int32_t _index;
    int32_t _step;
    int32_t _end;
};


class PyDynamicDataFieldsView {
public:
    PyDynamicDataFieldsView(DynamicData& dd) : _dd(dd)
    {
    }

    bool contains(const std::string& field_name)
    {
        return _dd.member_exists_in_type(field_name);
    }

    PyDynamicDataFieldsIterator iter()
    {
        return PyDynamicDataFieldsIterator(this->_dd, false);
    }

    PyDynamicDataFieldsIterator reversed()
    {
        return PyDynamicDataFieldsIterator(this->_dd, true);
    }

    size_t len()
    {
        return this->_dd.member_count();
    }

private:
    DynamicData& _dd;
};


class PyDynamicDataIndexIterator {
public:
    PyDynamicDataIndexIterator(DynamicData& dd, bool reversed) : _dd(dd)
    {
        if (reversed) {
            this->_index = dd.member_count();
            this->_end = 0;
            this->_step = -1;
        } else {
            this->_index = 1;
            this->_end = dd.member_count() + 1;
            this->_step = 1;
        }
        this->_elem_kind = static_cast<const CollectionType&>(dd.type())
                                   .content_type()
                                   .kind()
                                   .underlying();
    }

    py::object next()
    {
        if (this->_index == this->_end)
            throw py::stop_iteration();
        auto retval = get_member(this->_dd, this->_elem_kind, this->_index);
        this->_index += this->_step;
        return retval;
    }

private:
    DynamicData& _dd;
    TypeKind::inner_enum _elem_kind;
    int32_t _index;
    int32_t _step;
    int32_t _end;
};


class PyDynamicDataItemsIterator {
public:
    PyDynamicDataItemsIterator(DynamicData& dd, bool reversed) : _dd(dd)
    {
        if (reversed) {
            this->_index = dd.member_count();
            this->_end = 0;
            this->_step = -1;
        } else {
            this->_index = 1;
            this->_end = dd.member_count() + 1;
            this->_step = 1;
        }
    }

    std::pair<std::string, py::object> next()
    {
        auto retval = std::pair<std::string, py::object>();

        if (this->_index == this->_end)
            throw py::stop_iteration();

        auto mi = get_member_info(this->_dd, (uint32_t)this->_index);

        retval.first = mi.member_name();
        if (this->_dd.member_exists(this->_index)) {
           retval.second = get_member(
                            this->_dd,
                            mi.member_kind().underlying(),
                            mi.member_name());
        }
        else {
            retval.second = py::none();
        }
        this->_index += this->_step;
        return retval;
    }

private:
    DynamicData& _dd;
    int32_t _index;
    int32_t _step;
    int32_t _end;
};


class PyDynamicDataItemsView {
public:
    PyDynamicDataItemsView(DynamicData& dd) : _dd(dd)
    {
    }

    bool contains(const std::pair<std::string, py::object>& item)
    {
        return _dd.member_exists_in_type(item.first)
                && test_index(
                        this->_dd,
                        get_member_info(this->_dd, item.first)
                                .member_kind()
                                .underlying(),
                        this->_dd.member_index(item.first),
                        item.second);
    }

    PyDynamicDataItemsIterator iter()
    {
        return PyDynamicDataItemsIterator(this->_dd, false);
    }

    PyDynamicDataItemsIterator reversed()
    {
        return PyDynamicDataItemsIterator(this->_dd, true);
    }

    size_t len()
    {
        return this->_dd.member_count();
    }

private:
    DynamicData& _dd;
};


template<>
void init_dds_typed_topic_template(
        py::class_<
            PyTopic<DynamicData>,
            PyITopicDescription<DynamicData>,
            PyIAnyTopic,
            std::unique_ptr<PyTopic<DynamicData>, no_gil_delete<PyTopic<DynamicData>>>>& cls)
{
    init_dds_typed_topic_base_template(cls);
    cls.def(py::init([](PyDomainParticipant& dp,
                        const ::std::string& name,
                        const dds::core::xtypes::DynamicType& type) {
                PyTopic<dds::core::xtypes::DynamicData> t(dp, name, type);
                PyDynamicTypeMap::add(t.type_name(), type);
                return t;
            }),
            py::arg("participant"),
            py::arg("topic_name"),
            py::arg("topic_type"),
            py::call_guard<py::gil_scoped_release>(),
            "Create a Topic with the given type.")
            .def(py::init([](PyDomainParticipant& dp,
                             const ::std::string& name,
                             const dds::core::xtypes::DynamicType& type,
                             const dds::topic::qos::TopicQos& qos,
                             dds::core::optional<PyTopicListener<dds::core::xtypes::DynamicData>*>
                                     l,
                             const dds::core::status::StatusMask& mask) {
                     auto listener = has_value(l) ? get_value(l) : nullptr;
                     PyTopic<dds::core::xtypes::DynamicData>
                             t(dp, name, type, qos, listener, mask);
                     PyDynamicTypeMap::add(t.type_name(), type);
                     return t;
                 }),
                 py::arg("participant"),
                 py::arg("topic_name"),
                 py::arg("topic_type"),
                 py::arg("qos"),
                 py::arg("listener") = py::none(),
                 py::arg_v(
                         "mask",
                         dds::core::status::StatusMask::all(),
                         "StatusMask.ALL"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Create a Topic with the given type.")
            .def(py::init([](const PyDomainParticipant& dp,
                             const std::string& n,
                             const std::string& t,
                             const dds::topic::qos::TopicQos& q,
                             dds::core::optional<PyTopicListener<dds::core::xtypes::DynamicData>*> l,
                             const dds::core::status::StatusMask& m) {
                     auto listener = has_value(l) ? get_value(l) : nullptr;
                     return PyTopic<dds::core::xtypes::DynamicData>(dp, n, t, q, listener, m);
                 }),
                 py::arg("participant"),
                 py::arg("topic_name"),
                 py::arg("type_name"),
                 py::arg("qos"),
                 py::arg("listener") = py::none(),
                 py::arg_v(
                         "mask",
                         dds::core::status::StatusMask::all(),
                         "StatusMask.ALL"),
                 py::call_guard<py::gil_scoped_release>(),
                 "Creates a new Topic.");;
}

template<>
void init_dds_typed_datawriter_template(
        py::class_<
            PyDataWriter<DynamicData>,
            PyIEntity,
            PyIAnyDataWriter,
            std::unique_ptr<PyDataWriter<DynamicData>, no_gil_delete<PyDataWriter<DynamicData>>>>& cls)
{
    init_dds_typed_datawriter_base_template(cls);
    cls.def(
               "write",
               [](PyDataWriter<dds::core::xtypes::DynamicData>& dw,
                  py::dict& dict) {
                   auto dt = PyDynamicTypeMap::get(dw->type_name());
                   dds::core::xtypes::DynamicData sample(dt);
                   update_dynamicdata_object(sample, dict);
                   {
                       py::gil_scoped_release release;
                       dw.write(sample);
                   }
               },
               py::arg("sample_data"),
               "Create a DynamicData object and write it with the given "
               "dictionary containing field names as keys.")
            .def(
                    "write_async",
                    [](PyDataWriter<dds::core::xtypes::DynamicData>& dw,
                       py::dict& dict) {
                        return PyAsyncioExecutor::run<void>(
                                std::function<void()>([&dw, &dict]() {
                                    py::gil_scoped_acquire acquire;
                                    auto dt = PyDynamicTypeMap::get(
                                            dw->type_name());
                                    dds::core::xtypes::DynamicData sample(dt);
                                    update_dynamicdata_object(sample, dict);
                                    {
                                        py::gil_scoped_release release;
                                        dw.write(sample);
                                    }
                                }));
                    },
                    py::arg("sample_data"),
                    py::keep_alive<0, 1>(),
                    py::keep_alive<0, 2>(),
                    "Create a DynamicData object and write it with the given "
                    "dictionary containing field names as keys. This method is "
                    "awaitable and is only for use with asyncio.")
            .def(
                    "create_data",
                    [](PyDataWriter<dds::core::xtypes::DynamicData>& dw) {
                        return dds::core::xtypes::DynamicData(
                                PyDynamicTypeMap::get(dw->type_name()));
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Create data of the writer's associated type and "
                    "initialize it.")
            .def(
                    "key_value",
                    [](PyDataWriter<dds::core::xtypes::DynamicData>& dw,
                       const dds::core::InstanceHandle& handle) {
                        dds::core::xtypes::DynamicData d(
                                PyDynamicTypeMap::get(dw->type_name()));
                        dw.key_value(d, handle);
                        return d;
                    },
                    py::arg("handle"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Retrieve the instance key that corresponds to an instance "
                    "handle.")
            .def(
                    "topic_instance_key_value",
                    [](PyDataWriter<dds::core::xtypes::DynamicData>& dw,
                       const dds::core::InstanceHandle& handle) {
                        dds::core::xtypes::DynamicData d(
                                PyDynamicTypeMap::get(dw->type_name()));
                        dds::topic::TopicInstance<
                                dds::core::xtypes::DynamicData>
                                ti(handle, d);
                        dw.key_value(ti, handle);
                        return ti;
                    },
                    py::arg("handle"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Retrieve the instance key that corresponds to an instance "
                    "handle.");
}

template<>
void init_dds_typed_datareader_template(
        py::class_<
            PyDataReader<DynamicData>,
            PyIDataReader,
            std::unique_ptr<PyDataReader<DynamicData>, no_gil_delete<PyDataReader<DynamicData>>>>& cls)
{
    init_dds_typed_datareader_base_template(cls);
    cls.def(
               "key_value",
               [](PyDataReader<dds::core::xtypes::DynamicData>& dr,
                  const dds::core::InstanceHandle& handle) {
                   dds::core::xtypes::DynamicType dt(
                           PyDynamicTypeMap::get(dr->type_name()));
                   dds::core::xtypes::DynamicData dd(dt);
                   dr.key_value(dd, handle);
                   return dd;
               },
               py::arg("handle"),
               py::call_guard<py::gil_scoped_release>(),
               "Retrieve the instance key that corresponds to an instance "
               "handle.")
            .def(
                    "topic_instance_key_value",
                    [](PyDataReader<dds::core::xtypes::DynamicData>& dr,
                       const dds::core::InstanceHandle& handle) {
                        dds::core::xtypes::DynamicType dt(
                                PyDynamicTypeMap::get(dr->type_name()));
                        dds::core::xtypes::DynamicData dd(dt);
                        dds::topic::TopicInstance<
                                dds::core::xtypes::DynamicData>
                                ti(handle, dd);
                        dr.key_value(ti, handle);
                        return ti;
                    },
                    py::arg("handle"),
                    py::call_guard<py::gil_scoped_release>(),
                    "Retrieve the instance key that corresponds to an instance "
                    "handle.")
            .def(
                    "read_next",
                    [](PyDataReader<dds::core::xtypes::DynamicData>& dr) {
                        dds::core::optional<dds::sub::Sample<
                                dds::core::xtypes::DynamicData>>
                                retval;
                        dds::core::xtypes::DynamicData data(
                                PyDynamicTypeMap::get(dr->type_name()));
                        dds::sub::SampleInfo info;
                        if (dr->read(data, info)) {
                            retval = dds::sub::Sample<
                                    dds::core::xtypes::DynamicData>(data, info);
                        }
                        return retval;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Copy the next not-previously-accessed data value from the "
                    "DataReader via a read operation.")
            .def(
                    "take_next",
                    [](PyDataReader<dds::core::xtypes::DynamicData>& dr) {
                        dds::core::optional<dds::sub::Sample<
                                dds::core::xtypes::DynamicData>>
                                retval;
                        dds::core::xtypes::DynamicData data(
                                PyDynamicTypeMap::get(dr->type_name()));
                        dds::sub::SampleInfo info;
                        if (dr->take(data, info)) {
                            retval = dds::sub::Sample<
                                    dds::core::xtypes::DynamicData>(data, info);
                        }
                        return retval;
                    },
                    py::call_guard<py::gil_scoped_release>(),
                    "Copy the next not-previously-accessed data value from the "
                    "DataReader via a take operation.");
}

template<>
void init_dds_typed_topic_instance_template(
        py::class_<dds::topic::TopicInstance<DynamicData>>& cls)
{
    init_dds_typed_topic_instance_base_template(cls);
}

template<>
void init_dds_typed_sample_template(
        py::class_<dds::sub::Sample<DynamicData>>& cls)
{
    init_dds_typed_sample_base_template(cls);

    cls.def(py::init([](rti::sub::LoanedSample<DynamicData>& loaned_sample) {
                return dds::sub::Sample<DynamicData>(
                        loaned_sample.data(),
                        loaned_sample.info());
            }),
            py::arg("loaned_sample"),
            "Copy constructor.");
}

template<typename T>
void add_field_type(
        py::class_<DynamicData>& cls,
        const std::string& type_str,
        const std::string& type_info)
{
    cls.def(("get_" + type_str).c_str(),
            (T(DynamicData::*)(const std::string&) const)
                    & DynamicData::value<T>,
            py::arg("name"),
            ("Get a " + type_info + " value by field name.").c_str())
            .def(("get_" + type_str).c_str(),
                 [](const DynamicData& dd, uint32_t index) {
                     if (dd.type_kind().underlying() != TypeKind::UNION_TYPE) {
                         index += 1;
                     }
                     return dd.value<T>(index);
                 },
                 py::arg("name"),
                 ("Get a " + type_info + " value by field name.").c_str())
            .def(("set_" + type_str).c_str(),
                 (void (DynamicData::*)(const std::string&, const T&))
                         & DynamicData::value<T>,
                 py::arg("name"),
                 py::arg("value"),
                 ("Set a " + type_info + " value by name.").c_str())
            .def(("set_" + type_str).c_str(),
                 [](DynamicData& dd, uint32_t index, const T& v) {
                     if (dd.type_kind().underlying() != TypeKind::UNION_TYPE) {
                         index += 1;
                     }
                     dd.value<T>(index, v);
                 },
                 py::arg("index"),
                 py::arg("value"),
                 ("Set a " + type_info + " value by index.").c_str());
}


template<typename T>
void add_field_type_collection(
        py::class_<DynamicData>& cls,
        const TypeKind::inner_enum kind,
        const std::string& type_str,
        const std::string& type_info)
{
    cls.def(("get_" + type_str + "_values").c_str(),
            [](DynamicData& dd, const std::string& key) {
                std::vector<T> values;
                DynamicDataNestedIndex id;
                DynamicData& parent = resolve_nested_member(dd, key, id);
                if (id.index_type == DynamicDataNestedIndex::INT) 
                {
                    auto mi = get_member_info(parent, id.int_index);
                    resize_no_init(values, mi.element_count());
                    parent.get_values<T>(id.int_index, values);
                }
                else {
                    auto mi = get_member_info(parent, id.string_index);
                    resize_no_init(values, mi.element_count());
                    parent.get_values<T>(id.string_index, values);
                }
                return values;
            },
            py::arg("name"),
            ("Get multiple " + type_info + " values by field name.").c_str())
            .def(("get_" + type_str + "_values").c_str(),
                 [](const DynamicData& dd, uint32_t index) {
                     if (dd.type_kind().underlying() != TypeKind::UNION_TYPE) {
                         index += 1;
                     }
                     std::vector<T> values;
                     auto mi = get_member_info(dd, index);
                     resize_no_init(values, mi.element_count());
                     dd.get_values<T>(index, values);
                     return values;
                 },
                 py::arg("index"),
                 ("Get multiple " + type_info + " values by field name.")
                         .c_str())
            .def(("set_" + type_str + "_values").c_str(),
                 [kind](DynamicData& dd, const std::string& key, py::object& v) {
                     DynamicDataNestedIndex id;
                     DynamicData& parent = resolve_nested_member(dd, key, id);
                     if (id.index_type == DynamicDataNestedIndex::INT) {
                        set_collection_member(parent, kind, id.int_index, v);
                     }
                     else {
                        set_collection_member(parent, kind, id.string_index, v);
                     }
                 },
                 py::arg("name"),
                 py::arg("values"),
                 ("Get multiple " + type_info + " values by field name.")
                         .c_str())
            .def(("set_" + type_str + "_values").c_str(),
                 [kind](DynamicData& dd, uint32_t index, py::object& v) {
                     if (dd.type_kind().underlying() != TypeKind::UNION_TYPE) {
                         index += 1;
                     }
                     set_collection_member(dd, kind, index, v);
                 },
                 py::arg("index"),
                 py::arg("values"),
                 ("Get multiple " + type_info + " values by field name.")
                         .c_str());
}

template<>
void init_class_defs(py::class_<DynamicData>& dd_class)
{
    dd_class.def(py::init<const DynamicType&>(), 
                py::arg("dynamic_type"),
                "Create a DynamicData object from a DynamicType.")
            .def(py::init<
                    const DynamicType&,
                    const rti::core::xtypes::DynamicDataProperty&>(),
                py::arg("dynamic_type"),
                py::arg("property"),
                "Create a DynamicData object from a DynamicType with "
                "properties.")
            .def(py::init([](const DynamicType& dt, py::dict& data) {
                    DynamicData dd(dt);
                    update_dynamicdata_object(dd, data);
                    return dd;
                }),
                py::arg("dynamic_type"),
                py::arg("data"),
                "Create a DynamicData object from a DynamicType with "
                "a dict to initialize the fields")
            .def(py::init<DynamicData&>(),
                py::arg("other"),
                "Create a copy of a DynamicData object.")
            .def(py::self == py::self, "Test for equality.")
            .def(
                    "__eq__",
                    [](DynamicData& dd, py::dict& dict) {
                        try {
                            DynamicData other(dd.type());
                            update_dynamicdata_object(other, dict);
                            return dd == other;
                        } catch (dds::core::InvalidArgumentError&) {
                            return false;
                        }
                    },
                    py::is_operator())
            .def(py::self != py::self, "Test for inequality.")
            .def(
                    "__ne__",
                    [](DynamicData& dd, py::dict& dict) {
                        try {
                            DynamicData other(dd.type());
                            update_dynamicdata_object(other, dict);
                            return dd != other;
                        } catch (dds::core::InvalidArgumentError&) {
                            return false;
                        }
                    },
                    py::is_operator());

    py::class_<PyDynamicDataFieldsView> fields_view(dd_class, "FieldsView");
    py::class_<PyDynamicDataFieldsIterator> fields_iterator(
            dd_class,
            "FieldsIterator");
    py::class_<PyDynamicDataIndexIterator> index_iterator(
            dd_class,
            "IndexIterator");
    py::class_<PyDynamicDataItemsView> items_view(dd_class, "ItemsView");
    py::class_<PyDynamicDataItemsIterator> items_iterator(
            dd_class,
            "ItemsIterator");

    add_field_type<char>(dd_class, "int8", "8-bit signed int");
    add_field_type<uint8_t>(dd_class, "uint8", "8-bit unsigned int");
    add_field_type<uint8_t>(dd_class, "octet", "8-bit unsigned int");
    add_field_type<int16_t>(dd_class, "int16", "16-bit signed int");
    add_field_type<int16_t>(dd_class, "short", "16-bit signed int");
    add_field_type<uint16_t>(dd_class, "uint16", "16-bit unsigned int");
    add_field_type<uint16_t>(dd_class, "ushort", "16-bit unsigned int");
    add_field_type<int32_t>(dd_class, "int32", "32-bit signed int");
    add_field_type<int32_t>(dd_class, "long", "32-bit signed int");
    add_field_type<uint32_t>(dd_class, "uint32", "32-bit unsigned int");
    add_field_type<uint32_t>(dd_class, "ulong", "32-bit unsigned int");
    add_field_type<int>(dd_class, "int", "int (signed)");
    add_field_type<unsigned int>(dd_class, "uint", "32-bit unsigned int");
    add_field_type<rti::core::int64>(dd_class, "int64", "64-bit signed int");
    add_field_type<rti::core::uint64>(
            dd_class,
            "uint64",
            "64-bit unsigned int");
    add_field_type<long long>(dd_class, "longlong", "64-bit signed int");
    add_field_type<unsigned long long>(
            dd_class,
            "ulonglong",
            "64-bit unsigned int");
    add_field_type<float>(dd_class, "float32", "32-bit floating point");
    add_field_type<float>(dd_class, "float", "32-bit floating point");
    add_field_type<double>(dd_class, "float64", "64-bit floating point");
    add_field_type<double>(dd_class, "double", "64-bit floating point");
    add_field_type<rti::core::LongDouble>(
            dd_class,
            "float128",
            "128-bit floating point");
    add_field_type<rti::core::LongDouble>(
            dd_class,
            "longdouble",
            "128-bit floating point");
    add_field_type<bool>(dd_class, "boolean", "boolean");
    add_field_type<char>(dd_class, "char", "character");
    // DYNAMIC_DATA_GET_SET_DECL(dd_class, wchar_t, "wchar", "wide character");
    add_field_type<std::string>(dd_class, "string", "string");
    // DYNAMIC_DATA_GET_SET_DECL(dd_class, std::wstring, "wstring", "wide
    // string");
    add_field_type<DynamicData>(dd_class, "complex", "complex data type");

    add_field_type_collection<char>(
            dd_class,
            TypeKind::CHAR_8_TYPE,
            "int8",
            "8-bit signed int");
    add_field_type_collection<uint8_t>(
            dd_class,
            TypeKind::UINT_8_TYPE,
            "uint8",
            "8-bit unsigned int");
    add_field_type_collection<uint8_t>(
            dd_class,
            TypeKind::UINT_8_TYPE,
            "octet",
            "8-bit unsigned int");
    add_field_type_collection<int16_t>(
            dd_class,
            TypeKind::INT_16_TYPE,
            "int16",
            "16-bit signed int");
    add_field_type_collection<int16_t>(
            dd_class,
            TypeKind::INT_16_TYPE,
            "short",
            "16-bit signed int");
    add_field_type_collection<uint16_t>(
            dd_class,
            TypeKind::UINT_16_TYPE,
            "uint16",
            "16-bit unsigned int");
    add_field_type_collection<uint16_t>(
            dd_class,
            TypeKind::UINT_16_TYPE,
            "ushort",
            "16-bit unsigned int");
    add_field_type_collection<DDS_Long>(
            dd_class,
            TypeKind::INT_32_TYPE,
            "int32",
            "32-bit signed int");
    add_field_type_collection<DDS_Long>(
            dd_class,
            TypeKind::INT_32_TYPE,
            "long",
            "32-bit signed int");
    add_field_type_collection<DDS_UnsignedLong>(
            dd_class,
            TypeKind::UINT_32_TYPE,
            "uint32",
            "32-bit unsigned int");
    add_field_type_collection<DDS_UnsignedLong>(
            dd_class,
            TypeKind::UINT_32_TYPE,
            "ulong",
            "32-bit unsigned int");
    add_field_type_collection<DDS_LongLong>(
            dd_class,
            TypeKind::INT_64_TYPE,
            "int64",
            "64-bit signed int");
    add_field_type_collection<DDS_LongLong>(
            dd_class,
            TypeKind::INT_64_TYPE,
            "longlong",
            "64-bit signed int");
    add_field_type_collection<DDS_UnsignedLongLong>(
            dd_class,
            TypeKind::UINT_64_TYPE,
            "uint64",
            "64-bit unsigned int");
    add_field_type_collection<DDS_UnsignedLongLong>(
            dd_class,
            TypeKind::UINT_64_TYPE,
            "ulonglong",
            "64-bit unsigned int");
    add_field_type_collection<float>(
            dd_class,
            TypeKind::FLOAT_32_TYPE,
            "float32",
            "32-bit floating point");
    add_field_type_collection<float>(
            dd_class,
            TypeKind::FLOAT_32_TYPE,
            "float",
            "32-bit floating point");
    add_field_type_collection<double>(
            dd_class,
            TypeKind::FLOAT_64_TYPE,
            "float64",
            "64-bit floating point");
    add_field_type_collection<double>(
            dd_class,
            TypeKind::FLOAT_64_TYPE,
            "double",
            "64-bit floating point");
    add_field_type_collection<char>(
            dd_class,
            TypeKind::CHAR_8_TYPE,
            "char",
            "character");

    dd_class.def(
                    "get_wchar",
                    [](const DynamicData& data, const std::string& name) {
                        return static_cast<wchar_t>(data.value<DDS_Wchar>(name));
                    },
                    py::arg("name"),
                    "Get a wchar value by field name.")
            .def(
                    "get_wchar",
                    [](const DynamicData& data, uint32_t index) {
                        if (data.type_kind().underlying()
                            != TypeKind::UNION_TYPE) {
                            index += 1;
                        }
                        return static_cast<wchar_t>(
                                data.value<DDS_Wchar>(index));
                    },
                    py::arg("index"),
                    "Get a wchar value by field index.")
            .def(
                    "set_wchar",
                    [](DynamicData& data, const std::string& name, wchar_t c) {
                        DDS_Wchar value = static_cast<DDS_Wchar>(c);
                        data.value<DDS_Wchar>(name, value);
                    },
                    py::arg("name"),
                    py::arg("value"),
                    "Get a wchar value by field name.")
            .def(
                    "set_wchar",
                    [](DynamicData& data, uint32_t index, wchar_t c) {
                        DDS_Wchar value = static_cast<DDS_Wchar>(c);
                        if (data.type_kind().underlying()
                            != TypeKind::UNION_TYPE) {
                            index += 1;
                        }
                        data.value<DDS_Wchar>(index, value);
                    },
                    py::arg("index"),
                    py::arg("value"),
                    "Get a wchar value by field index.")
            .def(
                    "get_wstring",
                    [](const DynamicData& data, const std::string& name) {
                        std::vector<DDS_Wchar> v(
                                data.get_values<DDS_Wchar>(name));
                        std::wstring s;
                        for (auto c : v) {
                            s.push_back(static_cast<wchar_t>(c));
                        }
                        return s;
                    },
                    py::arg("name"),
                    "Get a wstring value by field name.")
            .def(
                    "get_wstring",
                    [](const DynamicData& data, uint32_t index) {
                        if (data.type_kind().underlying()
                            != TypeKind::UNION_TYPE) {
                            index += 1;
                        }
                        std::vector<DDS_Wchar> v(
                                data.get_values<DDS_Wchar>(index));
                        std::wstring s;
                        for (auto c : v) {
                            s.push_back(static_cast<wchar_t>(c));
                        }
                        return s;
                    },
                    py::arg("index"),
                    "Get a wstring value by field index.")
            .def(
                    "set_wstring",
                    [](DynamicData& data,
                       const std::string& name,
                       const std::wstring& s) {
                        std::vector<DDS_Wchar> v;
                        for (auto c : s) {
                            v.push_back(static_cast<DDS_Wchar>(c));
                        }
                        data.set_values<DDS_Wchar>(name, v);
                    },
                    py::arg("name"),
                    py::arg("value"),
                    "Set a wstring by field name.")
            .def(
                    "set_wstring",
                    [](DynamicData& data,
                       uint32_t index,
                       const std::wstring& s) {
                        std::vector<DDS_Wchar> v;
                        for (auto c : s) {
                            v.push_back(static_cast<DDS_Wchar>(c));
                        }
                        if (data.type_kind().underlying()
                            != TypeKind::UNION_TYPE) {
                            index += 1;
                        }
                        data.set_values<DDS_Wchar>(index, v);
                    },
                    py::arg("index"),
                    py::arg("value"),
                    "Get a wchar value by field index.")
            .def(
                    "get_complex_values",
                    [](DynamicData& data, const std::string& name) {
                        DynamicDataNestedIndex id;
                        DynamicData& parent = resolve_nested_member(data, name, id);
                        if (id.index_type == DynamicDataNestedIndex::INT) {
                            return get_complex_values(parent, id.int_index);
                        }
                        else {
                            return get_complex_values(parent, id.string_index);
                        }
                    },
                    py::arg("name"),
                    "Get a list of complex values by field name.")
            .def(
                    "get_complex_values",
                    [](DynamicData& data, uint32_t index) {
                        if (data.type_kind().underlying()
                            != TypeKind::UNION_TYPE) {
                            index += 1;
                        }
                        return get_complex_values(data, index);
                    },
                    py::arg("index"),
                    "Get a list of complex fields by index.")
            .def(
                    "set_complex_values",
                    [](DynamicData& data,
                            const std::string& name,
                            py::iterable& values) {
                        DynamicDataNestedIndex id;
                        DynamicData& parent = resolve_nested_member(data, name, id);
                        if (id.index_type == DynamicDataNestedIndex::INT) {
                            set_complex_values(parent, id.int_index, values);
                        }
                        else {
                            set_complex_values(parent, id.string_index, values);
                        }
                    },
                    py::arg("name"),
                    py::arg("values"),
                    "Set a list of complex values by field name.")
            .def(
                    "set_complex_values",
                    [](DynamicData& data,
                       uint32_t index,
                       py::iterable& values) {
                        if (data.type_kind().underlying()
                            != TypeKind::UNION_TYPE) {
                            index += 1;
                        }
                        set_complex_values(data, index, values);
                    },
                    py::arg("index"),
                    py::arg("values"),
                    "Set a list of complex values by index.")
            .def(
                    "loan_value",
                    [](DynamicData& data, uint32_t index) {
                        if (data.type_kind().underlying()
                            != TypeKind::UNION_TYPE) {
                            index += 1;
                        }
                        return data.loan_value(index);
                    },
                    py::arg("index"),
                    py::keep_alive<0, 1>(),
                    "Gets a view of a complex member.")
            .def(
                    "loan_value",
                    [](DynamicData& data,
                       rti::core::xtypes::LoanedDynamicData& loan,
                       uint32_t index)
                            -> rti::core::xtypes::LoanedDynamicData& {
                        if (data.type_kind().underlying()
                            != TypeKind::UNION_TYPE) {
                            index += 1;
                        }
                        return data.loan_value(loan, index);
                    },
                    py::arg("data"),
                    py::arg("index"),
                    py::keep_alive<0, 1>(),
                    "Gets a view of a complex member.")
            .def_property_readonly(
                    "discriminator_value",
                    &DynamicData::discriminator_value,
                    "Obtains the value of the union discriminator (valid for "
                    "UnionType only).")
            .def("clear_all_members",
                 &DynamicData::clear_all_members,
                 "Clear the contents of all data members of this object, "
                 "including key members.")
            .def("clear_optional_member",
                 (void (DynamicData::*)(const std::string&))
                         & DynamicData::clear_optional_member,
                 py::arg("name"),
                 "Clear the contents of a single optional data member of this "
                 "object.")
            .def(
                    "clear_optional_member",
                    [](DynamicData& data, uint32_t index) {
                        if (data.type_kind().underlying()
                            != TypeKind::UNION_TYPE) {
                            index += 1;
                        }
                        data.clear_optional_member(index);
                    },
                    py::arg("index"),
                    "Clear the contents of a single optional data member of "
                    "this object.")
#if rti_connext_version_gte(6, 0, 0, 0)
            .def("clear_member",
                 (void (DynamicData::*)(const std::string&))
                         & DynamicData::clear_member,
                 py::arg("name"),
                 "Clear the contents of a single data member of this object.")
            .def(
                    "clear_member",
                    [](DynamicData& data, uint32_t index) {
                        data.clear_member(index + 1);
                    },
                    py::arg("index"),
                    "Clear the contents of a single data member of this "
                    "object.")
#endif
#if rti_connext_version_lt(6, 1, 0, 0)
            .def("set_buffer",
                 &DynamicData::set_buffer,
                 py::arg("storage"),
                 py::arg("size"),
                 "Associate a buffer with this dynamic data object.")
            .def_property_readonly(
                    "estimated_max_buffer_size",
                    &DynamicData::estimated_max_buffer_size,
                    "Get the estimated maximum buffer size for a DynamicData "
                    "object.")
#endif
            .def_property_readonly(
                    "type",
                    [](const DynamicData& dd) {
                        auto dt = dd.type();
                        return py_cast_type(dt);
                    },
                    "Gets the data type of this DynamicData.")
            .def_property_readonly(
                    "type_kind",
                    &DynamicData::type_kind,
                    "Gets this data type kind of this DynamicData.")
            .def_property_readonly(
                    "member_count",
                    &DynamicData::member_count,
                    "Get the number of members in this sample.")
            .def("member_exists",
                 (bool (DynamicData::*)(const std::string&) const)
                         & DynamicData::member_exists,
                 py::arg("name"),
                 "Determine if an optional member is set by member name.")
            .def(
                    "member_exists",
                    [](const DynamicData& data, uint32_t index) {
                        if (data.type_kind().underlying()
                            != TypeKind::UNION_TYPE) {
                            index += 1;
                        }
                        return data.member_exists(index);
                    },
                    py::arg("index"),
                    "Determine if an optional member is set by member index.")
            .def("member_exists_in_type",
                 (bool (DynamicData::*)(const std::string&) const)
                         & DynamicData::member_exists_in_type,
                 py::arg("name"),
                 "Determine if a member with a particular name exists in the "
                 "type.")
            .def("member_exists_in_type",
                 (bool (DynamicData::*)(uint32_t) const)
                         & DynamicData::member_exists_in_type,
                 py::arg("index"),
                 "Determine if a member with a particular index exists in the "
                 "type.")
            .def_property_readonly(
                    "info",
                    &DynamicData::info,
                    "Returns info about this sample")
            .def("member_info",
                 [](DynamicData& dd, const std::string& key) {
                    DynamicDataNestedIndex id;
                    DynamicData& parent = resolve_nested_member(dd, key, id);
                    rti::core::xtypes::DynamicDataMemberInfo mi;

                    if (id.index_type == DynamicDataNestedIndex::INT) {
                        mi = get_member_info(parent, id.int_index);
                    }
                    else {
                        mi = get_member_info(parent, id.string_index);
                    }
                    if (parent.type_kind().underlying() != TypeKind::UNION_TYPE) {
                        mi.native().member_id -= 1;
                    }
                    return mi;
                 },
                 py::arg("name"),
                 "Returns info about a member.")
            .def(
                    "member_info",
                    [](const DynamicData& data, uint32_t index) {
                        if (data.type_kind().underlying()
                            != TypeKind::UNION_TYPE) {
                            index += 1;
                        }
                        auto mi = get_member_info(data, index);
                        if (data.type_kind().underlying() != TypeKind::UNION_TYPE) {
                            mi.native().member_id -= 1;
                        }
                        return mi;
                    },
                    py::arg("index"),
                    "Returns info about a member.")
            .def("is_member_key",
                 (bool (DynamicData::*)(const std::string&) const)
                         & DynamicData::is_member_key,
                 py::arg("name"),
                 "Returns whether a member is a key.")
            .def(
                    "is_member_key",
                    [](const DynamicData& data, uint32_t index) {
                        return data.is_member_key(index + 1);
                    },
                    py::arg("index"),
                    "Returns whether a member is a key.")
            .def(
                    "__len__",
                    &DynamicData::member_count,
                    "Number of members in the DynamicData object")
            .def(
                    "__str__",
                    [](const DynamicData& sample) {
                        return rti::core::xtypes::to_string(sample);
                    },
                    "DynamicData value to string.")
            .def(
                    "to_string",
                    [](const DynamicData& sample, const rti::topic::PrintFormatProperty& prop) {
                        return rti::topic::to_string(sample, prop);
                    },
                    py::arg_v(
                        "format",
                        rti::topic::PrintFormatProperty::Default(),
                        "PrintFormatProperty.default"),
                    "Convert DynamicData object to string with print format.")
            .def(
                    "to_cdr_buffer",
                    [](const dds::core::xtypes::DynamicData& sample) {
                        std::vector<char> output;
                        rti::core::xtypes::to_cdr_buffer(output, sample);
                        return output;
                    },
                    "Serializes a DynamicData sample to CDR format")
            .def(
                    "from_cdr_buffer",
                    [](dds::core::xtypes::DynamicData& sample,
                       const std::vector<char>& buffer) {
                        return rti::core::xtypes::from_cdr_buffer(
                                sample,
                                buffer);
                    },
                    py::arg("buffer"),
                    "Populates a DynamicData sample by deserializing a CDR "
                    "buffer.")
            .def("fields",
                 [](DynamicData& dd) {
                     if (dd.type_kind().underlying()
                         != TypeKind::STRUCTURE_TYPE) {
                         throw py::type_error(
                                 "Can't get fields of a non-structure type");
                     }
                     return PyDynamicDataFieldsView(dd);
                 })
            .def("items",
                 [](DynamicData& dd) {
                     if (dd.type_kind().underlying()
                         != TypeKind::STRUCTURE_TYPE) {
                         throw py::type_error(
                                 "Can't get items of a non-structure type");
                     }
                     return PyDynamicDataItemsView(dd);
                 })
            .def(
                    "append",
                    [](DynamicData& dd, py::object& value) {
                        auto type = dd.type();
                        auto kind = type.kind().underlying();
                        if (kind != TypeKind::SEQUENCE_TYPE) {
                            throw py::type_error(
                                    "Cannot append to non sequence type.");
                        }
                        auto& collection_type =
                                static_cast<const CollectionType&>(type);
                        set_member(
                                dd,
                                collection_type.content_type()
                                        .kind()
                                        .underlying(),
                                dd.member_count() + 1,
                                value);
                    },
                    py::arg("value"),
                    "Append a value to a sequence.")
            .def(
                    "extend",
                    [](DynamicData& dd, py::iterable& values) {
                        auto type = dd.type();
                        auto kind = type.kind().underlying();
                        if (kind != TypeKind::SEQUENCE_TYPE) {
                            throw py::type_error(
                                    "Cannot extend non sequence type.");
                        }
                        auto& collection_type =
                                static_cast<const CollectionType&>(type);
                        auto elem_kind = collection_type.content_type()
                                                 .kind()
                                                 .underlying();
                        for (auto value : values) {
                            auto obj = py::cast<py::object>(value);
                            set_member(
                                    dd,
                                    elem_kind,
                                    dd.member_count() + 1,
                                    obj);
                        }
                    },
                    py::arg("values"),
                    "Extend a sequence.")
            .def("count",
                 [](const DynamicData& dd, const py::object& obj) {
                     if (dd.type_kind().underlying() != TypeKind::ARRAY_TYPE
                         && dd.type_kind().underlying()
                                 != TypeKind::SEQUENCE_TYPE) {
                         throw py::type_error(
                                 "This DynamicData type does not support this "
                                 "operation");
                     }
                     auto kind = static_cast<const CollectionType&>(dd.type())
                                         .content_type()
                                         .kind()
                                         .underlying();
                     int count = 0;
                     for (uint32_t i = 1; i <= dd.member_count(); ++i) {
                         if (test_index(dd, kind, i, obj))
                             count += 1;
                     }
                     return count;
                 })
            .def("update",
                 [](DynamicData& dd, py::dict& dict) {
                     update_dynamicdata_object(dd, dict);
                 })
            .def(
                "set_value",
                [](DynamicData& dd, std::string& key, py::object& value) {
                    DynamicDataNestedIndex id;
                    DynamicData& parent = resolve_nested_member(dd, key, id);
                    if (id.index_type == DynamicDataNestedIndex::INT) set_value(parent, id.int_index, value);
                    else set_value(parent, id.string_index, value);
                },
                py::arg("field_path"),
                py::arg("value"),
                "Automatically resolve type and set value for a field.")
            .def(
                "set_values",
                [](DynamicData& dd, std::string& key, py::object& values) {
                    DynamicDataNestedIndex id;
                    DynamicData& parent = resolve_nested_member(dd, key, id);
                    if (id.index_type == DynamicDataNestedIndex::INT) set_values(parent, id.int_index, values);
                    else set_values(parent, id.string_index, values);
                },
                py::arg("field_path"),
                py::arg("values"),
                "Automatically resolve type and set collection for a field."
            )
            .def(
                "__setitem__",
                [](DynamicData& dd, std::string& key, py::object& value) {
                    DynamicDataNestedIndex id;
                    DynamicData& parent = resolve_nested_member(dd, key, id);
                    if (id.index_type == DynamicDataNestedIndex::INT) set_value(parent, id.int_index, value);
                    else set_value(parent, id.string_index, value);
                })
        .def(
            "get_value",
            [](DynamicData& dd, std::string& key) {
                DynamicDataNestedIndex id;
                DynamicData& parent = resolve_nested_member(dd, key, id);
                if (id.index_type == DynamicDataNestedIndex::INT) return get_value(parent, id.int_index);
                else return get_value(parent, id.string_index);
            },
            py::arg("field_path"),
            "Automatically resolve type and return value for a field."
        )
        .def(
            "get_values",
            [](DynamicData& dd, std::string& key) {
                DynamicDataNestedIndex id;
                DynamicData& parent = resolve_nested_member(dd, key, id);
                if (id.index_type == DynamicDataNestedIndex::INT) return get_values(parent, id.int_index);
                else return get_values(parent, id.string_index);
            },
            py::arg("field_path"),
            "Automatically resolve type and return collection for a field."
        )
        .def(
            "member_exists",
            [](DynamicData& dd, std::string& key) {
                DynamicDataNestedIndex id;
                DynamicData& parent = resolve_nested_member(dd, key, id);
                if (id.index_type == DynamicDataNestedIndex::INT) return parent.member_exists(id.int_index);
                else return parent.member_exists(id.string_index);
            },
            py::arg("name"),
            "Determine if an optional member is set by member name."
        )
        .def(
            "member_exists_in_type",
            [](DynamicData& dd, std::string& key) {
                DynamicDataNestedIndex id;
                DynamicData& parent = resolve_nested_member(dd, key, id);
                if (id.index_type == DynamicDataNestedIndex::INT) return parent.member_exists_in_type(id.int_index);
                else return parent.member_exists_in_type(id.string_index);
            },
            py::arg("name"),
            "Determine if an optional member is set by member name."
        )
        .def(
            "loan_value",
            [](DynamicData& dd, std::string& key) {
                DynamicDataNestedIndex id;
                DynamicData& parent = resolve_nested_member(dd, key, id);
                if (id.index_type == DynamicDataNestedIndex::INT) return parent.loan_value(id.int_index);
                else return parent.loan_value(id.string_index);
            },
            py::arg("name"),
            py::keep_alive<0, 1>(),
            "Gets a view of a complex member."
        )
        .def(
            "loan_value",
            [](DynamicData& dd, rti::core::xtypes::LoanedDynamicData& data, std::string& key) -> rti::core::xtypes::LoanedDynamicData& {
                DynamicDataNestedIndex id;
                DynamicData& parent = resolve_nested_member(dd, key, id);
                if (id.index_type == DynamicDataNestedIndex::INT) return parent.loan_value(data, id.int_index);
                else return parent.loan_value(data, id.string_index);
            },
            py::arg("data"),
            py::arg("name"),
            py::keep_alive<0, 1>(),
            "Gets a view of a complex member."
        )
        .def(
            "member_index",
            [](DynamicData& data, std::string& key) {
                DynamicDataNestedIndex id;
                DynamicData& parent = resolve_nested_member(data, key, id);
                if (id.index_type == DynamicDataNestedIndex::INT) {
                    return static_cast<unsigned int>(id.int_index - 1);
                }
                else {
                    auto kind = resolve_type_kind(parent);
                    if (kind != TypeKind::UNION_TYPE) {
                        return parent.member_index(id.string_index) - 1;
                    }
                    else return parent.member_index(id.string_index);
                }
            },
            py::arg("name"),
            "Translates from member name to member index."
        )
            .def(
                "__getitem__",
                [](DynamicData& dd, std::string& key) {
                    DynamicDataNestedIndex id;
                    DynamicData& parent = resolve_nested_member(dd, key, id);
                    if (id.index_type == DynamicDataNestedIndex::INT) return get_value(parent, id.int_index);
                    else return get_value_as_dict(parent, id.string_index);
                }
            )
            .def(
                    "get_value",
                    [](DynamicData& dd, size_t index) {
                        auto kind = resolve_type_kind(dd);
                        if (kind != TypeKind::UNION_TYPE)
                            index += 1;
                        return get_value(dd, index);
                    },
                    py::arg("field_path"),
                    "Automatically resolve type and return value for a field.")
            .def(
                    "get_values",
                    [](DynamicData& dd, size_t index) {
                        auto kind = resolve_type_kind(dd);
                        if (kind != TypeKind::UNION_TYPE)
                            index += 1;
                        return get_values(dd, index);
                    },
                    py::arg("field_path"),
                    "Automatically resolve type and return collection for a "
                    "field.")
            .def(
                    "set_value",
                    [](DynamicData& dd, size_t index, py::object& value) {
                        auto kind = resolve_type_kind(dd);
                        if (kind != TypeKind::UNION_TYPE)
                            index += 1;
                        else
                            set_value(dd, index, value);
                    },
                    py::arg("field_path"),
                    py::arg("value"),
                    "Automatically resolve type and set value for a field.")
            .def(
                    "set_values",
                    [](DynamicData& dd, size_t index, py::object& values) {
                        auto kind = resolve_type_kind(dd);
                        if (kind != TypeKind::UNION_TYPE)
                            index += 1;
                        else
                            set_values(dd, index, values);
                    },
                    py::arg("field_path"),
                    py::arg("values"),
                    "Automatically resolve type and set collection for a "
                    "field.")
            .def("__getitem__",
                 [](DynamicData& dd, size_t index) -> py::object {
                     auto kind = resolve_type_kind(dd);
                     if (kind != TypeKind::UNION_TYPE) {
                         index += 1;  // Python index starts at 0 by language
                                      // convention
                     }
                     switch (kind) {
                     case TypeKind::STRUCTURE_TYPE:
                     case TypeKind::UNION_TYPE: {
                         return get_value_as_dict(dd, index);
                     }
                     case TypeKind::ARRAY_TYPE:
                     case TypeKind::SEQUENCE_TYPE: {
                         auto& collection_type =
                                 static_cast<const CollectionType&>(dd.type());
                         return get_member(
                                 dd,
                                 collection_type.content_type()
                                         .kind()
                                         .underlying(),
                                 index,
                                 true);
                     }
                     default:
                         throw py::type_error(
                                 "This DynamicData type does not support index "
                                 "access.");
                     }
                 })
            .def("__setitem__",
                 [](DynamicData& dd, size_t index, py::object& value) {
                     auto kind = resolve_type_kind(dd);
                     if (kind != TypeKind::UNION_TYPE) {
                         index += 1;  // Python index starts at 0 by language
                                      // convention
                     }
                     switch (kind) {
                     case TypeKind::STRUCTURE_TYPE:
                     case TypeKind::UNION_TYPE: {
                         set_value(dd, index, value);
                         break;
                     }
                     case TypeKind::ARRAY_TYPE:
                     case TypeKind::SEQUENCE_TYPE: {
                         auto& collection_type =
                                 static_cast<const CollectionType&>(dd.type());
                         set_member(
                                 dd,
                                 collection_type.content_type()
                                         .kind()
                                         .underlying(),
                                 index,
                                 value);
                         break;
                     }
                     default:
                         throw py::type_error(
                                 "This DynamicData type does not support index "
                                 "access.");
                     }
                 })
            .def("__getitem__",
                 [](DynamicData& dd, py::tuple index) -> py::object {
                     auto& type = rti::core::xtypes::resolve_alias(dd.type());
                     auto vec = py::cast<std::vector<int>>(index);
                     int offset =
                             calculate_multidimensional_offset(type, vec);
                     auto& collection_type =
                             static_cast<const CollectionType&>(type);
                     return get_member(
                             dd,
                             collection_type.content_type().kind().underlying(),
                             offset + 1,
                             true);
                 })
            .def("__setitem__",
                 [](DynamicData& dd, py::tuple index, py::object& value) {
                     // Python index starts at 0 by language
                     // convention
                     auto& type = rti::core::xtypes::resolve_alias(dd.type());
                     auto vec = py::cast<std::vector<int>>(index);
                     int offset =
                             calculate_multidimensional_offset(type, vec);
                     auto& collection_type =
                             static_cast<const CollectionType&>(type);
                     set_member(
                             dd,
                             collection_type.content_type().kind().underlying(),
                             offset + 1,
                             value);
                 })
            .def("__getitem__",
                 [](DynamicData& dd, py::slice slice) {
                     if (dd.type_kind().underlying() != TypeKind::ARRAY_TYPE
                         && dd.type_kind().underlying()
                                 != TypeKind::SEQUENCE_TYPE) {
                         throw py::type_error(
                                 "This DynamicData type does not support this "
                                 "operation");
                     }

                     size_t start, stop, step, slicelength;
                     if (!slice.compute(
                                 dd.member_count(),
                                 &start,
                                 &stop,
                                 &step,
                                 &slicelength))
                         throw py::error_already_set();

                     py::list seq;
                     auto type = dd.type();
                     auto& collection_type =
                             static_cast<const CollectionType&>(type);
                     auto elem_kind =
                             collection_type.content_type().kind().underlying();

                     for (size_t i = 0; i < slicelength; ++i) {
                         seq.append(get_member(dd, elem_kind, start + 1, true));
                         start += step;
                     }
                     return seq;
                 })
            .def("__setitem__",
                 [](DynamicData& dd, py::slice slice, py::iterable values) {
                     if (dd.type_kind().underlying() != TypeKind::ARRAY_TYPE
                         && dd.type_kind().underlying()
                                 != TypeKind::SEQUENCE_TYPE) {
                         throw py::type_error(
                                 "This DynamicData type does not support this "
                                 "operation");
                     }

                     size_t start, stop, step, slicelength;
                     if (!slice.compute(
                                 dd.member_count(),
                                 &start,
                                 &stop,
                                 &step,
                                 &slicelength))
                         throw py::error_already_set();

                     auto type = dd.type();
                     auto& collection_type =
                             static_cast<const CollectionType&>(type);
                     auto elem_kind =
                             collection_type.content_type().kind().underlying();

                     for (auto value : values) {
                         auto obj = py::cast<py::object>(value);
                         set_member(dd, elem_kind, start + 1, obj);
                         start += step;
                     }
                 })
            .def("__contains__",
                 [](const DynamicData& dd, const std::string& field_name) {
                     return dd.member_exists(field_name);
                 })
            .def("__contains__",
                 [](const DynamicData& dd, const py::object& obj) {
                     if (dd.type_kind().underlying() != TypeKind::ARRAY_TYPE
                         && dd.type_kind().underlying()
                                 != TypeKind::SEQUENCE_TYPE) {
                         throw py::type_error(
                                 "This DynamicData type does not support this "
                                 "operation");
                     }
                     auto kind = static_cast<const CollectionType&>(dd.type())
                                         .content_type()
                                         .kind()
                                         .underlying();
                     for (uint32_t i = 1; i <= dd.member_count(); ++i) {
                         if (test_index(dd, kind, i, obj))
                             return true;
                     }
                     return false;
                 })
            .def(
                    "__iter__",
                    [](DynamicData& dd) {
                        auto type = dd.type();
                        auto kind = type.kind().underlying();
                        switch (kind) {
                        case TypeKind::STRUCTURE_TYPE: {
                            return py::cast(
                                    PyDynamicDataFieldsIterator(dd, false));
                        }
                        case TypeKind::ARRAY_TYPE:
                        case TypeKind::SEQUENCE_TYPE: {
                            return py::cast(
                                    PyDynamicDataIndexIterator(dd, false));
                        }
                        default:
                            throw py::type_error(
                                    "This DynamicData type does not support "
                                    "iteration");
                        }
                    },
                    py::keep_alive<0, 1>())
            .def("__reverse__", [](DynamicData& dd) {
                auto type = dd.type();
                auto kind = type.kind().underlying();
                switch (kind) {
                case TypeKind::STRUCTURE_TYPE: {
                    return py::cast(PyDynamicDataFieldsIterator(dd, true));
                }
                case TypeKind::ARRAY_TYPE:
                case TypeKind::SEQUENCE_TYPE: {
                    return py::cast(PyDynamicDataIndexIterator(dd, true));
                }
                default:
                    throw py::type_error(
                            "This DynamicData type does not support iteration");
                }
            });

    py::class_<topic_type_support<DynamicData>>(dd_class, "TopicTypeSupport")
            .def_static(
                    "register_type",
                    [](PyDomainParticipant& dp, const std::string& type_name) {
                        topic_type_support<DynamicData>::register_type(
                                dp,
                                type_name);
                    },
                    py::arg("participant"),
                    py::arg("type_name"),
                    "Register a type with a participant.")
            .def_static(
                    "initialize_sample",
                    &topic_type_support<DynamicData>::initialize_sample)
            .def_static(
                    "to_cdr_buffer",
                    &topic_type_support<DynamicData>::to_cdr_buffer,
                    py::arg("buffer"),
                    py::arg("sample"),
#if rti_connext_version_gte(6, 0, 0, 0)
                    py::arg("representation_id") =
                            dds::core::policy::DataRepresentation::auto_id(),
#endif
                    "Serialize sample to a CDR buffer.")
            .def_static(
                    "from_cdr_buffer",
                    &topic_type_support<DynamicData>::from_cdr_buffer,
                    py::arg("sample"),
                    py::arg("buffer"),
                    "Deserialize a sample from a CDR buffer.");

    fields_view.def(py::init<DynamicData&>())
            .def("__iter__",
                 &PyDynamicDataFieldsView::iter,
                 py::keep_alive<0, 1>())
            .def("__reverse__", &PyDynamicDataFieldsView::reversed)
            .def("__contains__", &PyDynamicDataFieldsView::contains)
            .def("__len__", &PyDynamicDataFieldsView::len);

    fields_iterator.def(py::init<DynamicData&, bool>())
            .def("__next__", &PyDynamicDataFieldsIterator::next);

    index_iterator.def(py::init<DynamicData&, bool>())
            .def("__next__", &PyDynamicDataIndexIterator::next);

    items_view.def(py::init<DynamicData&>())
            .def("__iter__",
                 &PyDynamicDataItemsView::iter,
                 py::keep_alive<0, 1>())
            .def("__reverse__", &PyDynamicDataItemsView::reversed)
            .def("__contains__", &PyDynamicDataItemsView::contains)
            .def("__len__", &PyDynamicDataItemsView::len);

    items_iterator.def(py::init<DynamicData&, bool>())
            .def("__next__", &PyDynamicDataItemsIterator::next);
}

template<>
void process_inits<DynamicData>(py::module& m, ClassInitList& l)
{
    l.push_back([m, &l]() mutable {
        return init_type_class<DynamicData>(m, l, "DynamicData");
    });
}

}  // namespace pyrti
