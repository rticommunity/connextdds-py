#include "PyConnext.hpp"
#include <dds/core/xtypes/PrimitiveTypes.hpp>

using namespace dds::core::xtypes;

template<typename T>
void init_dds_dynamic_primitive_defs(py::module& m, const std::string& name, const std::string& singleton_func_name) {
    m.
        def(
            singleton_func_name.c_str(),
            []() {
                return primitive_type<T>();
            },
            ("Get the singleton for " + name).c_str()
        );
}

template<typename T>
void init_dds_dynamic_primitive(py::module& m, const std::string& name, const std::string& singleton_func_name, pyrti::ClassInitList& l) {
    l.push_back(
        [m, name, singleton_func_name]() mutable {
            py::class_<PrimitiveType<T>, DynamicType> cls(m, name.c_str());
            return (
                [m, name, singleton_func_name]() mutable {
                    init_dds_dynamic_primitive_defs<T>(m, name, singleton_func_name);
                }     
            );
        }
    );  
}

template<>
void pyrti::process_inits<pyrti::PyPrimitiveType>(py::module& m, pyrti::ClassInitList& l) {
    init_dds_dynamic_primitive<char>(m, "CharType", "char_type", l);
    init_dds_dynamic_primitive<bool>(m, "BoolType", "bool_type", l);
    init_dds_dynamic_primitive<uint8_t>(m, "Uint8Type", "uint8_type", l);
    init_dds_dynamic_primitive<int16_t>(m, "Int16Type", "int16_type", l);
    init_dds_dynamic_primitive<uint16_t>(m, "Uint16Type", "uint16_type", l);
    init_dds_dynamic_primitive<int32_t>(m, "Int32Type", "int32_type", l);
    init_dds_dynamic_primitive<uint32_t>(m, "Uint32Type", "int32_type", l);
    init_dds_dynamic_primitive<int64_t>(m, "Int64Type", "int64_type", l);
    init_dds_dynamic_primitive<uint64_t>(m, "Uint64Type", "uint64_type", l);
    init_dds_dynamic_primitive<float>(m, "Float", "float_type", l);
    init_dds_dynamic_primitive<double>(m, "Double", "double_type", l);
    init_dds_dynamic_primitive<rti::core::LongDouble>(m, "LongDoubleType", "longdouble_type", l);
    init_dds_dynamic_primitive<wchar_t>(m, "WcharType", "wchar_type", l);
}
