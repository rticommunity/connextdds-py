#include "PyConnext.hpp"
#include <dds/core/xtypes/PrimitiveTypes.hpp>

using namespace dds::core::xtypes;

namespace pyrti {

template<typename T>
void init_dds_dynamic_primitive_defs(py::class_<PrimitiveType<T>, DynamicType>& cls, const std::string& name) {
    cls.
        def(
            py::init(
                []() {
                    return primitive_type<T>();
                }
            ),
            ("Get the singleton for " + name).c_str()
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );
}

template<typename T>
void init_dds_dynamic_primitive(py::module& m, const std::string& name, ClassInitList& l) {
    l.push_back(
        [m, name]() mutable {
            py::class_<PrimitiveType<T>, DynamicType> cls(m, name.c_str());
            return (
                [cls, name]() mutable {
                    init_dds_dynamic_primitive_defs<T>(cls, name);
                }     
            );
        }
    );  
}

template<>
void process_inits<PyPrimitiveType>(py::module& m, ClassInitList& l) {
    init_dds_dynamic_primitive<char>(m, "CharType", l);
    init_dds_dynamic_primitive<bool>(m, "BoolType", l);
    init_dds_dynamic_primitive<uint8_t>(m, "OctetType", l);
    init_dds_dynamic_primitive<int16_t>(m, "Int16Type", l);
    init_dds_dynamic_primitive<uint16_t>(m, "Uint16Type", l);
    init_dds_dynamic_primitive<int32_t>(m, "Int32Type", l);
    init_dds_dynamic_primitive<uint32_t>(m, "Uint32Type", l);
    init_dds_dynamic_primitive<rti::core::int64>(m, "Int64Type", l);
    init_dds_dynamic_primitive<rti::core::uint64>(m, "Uint64Type", l);
    init_dds_dynamic_primitive<float>(m, "Float32Type", l);
    init_dds_dynamic_primitive<double>(m, "Float64Type", l);
    init_dds_dynamic_primitive<rti::core::LongDouble>(m, "Float128Type", l);
    init_dds_dynamic_primitive<wchar_t>(m, "WcharType", l);
}

}
