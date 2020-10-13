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
#include <dds/core/xtypes/PrimitiveTypes.hpp>

using namespace dds::core::xtypes;

namespace pyrti {

template<typename T>
void init_dds_dynamic_primitive_defs(
        py::class_<PrimitiveType<T>, DynamicType>& cls,
        const std::string& name)
{
    cls.def(py::init([]() { return primitive_type<T>(); }),
            ("Get the singleton for " + name).c_str())
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<typename T>
void init_dds_dynamic_primitive(
        py::module& m,
        const std::string& name,
        ClassInitList& l,
        const char* alias = nullptr)
{
    l.push_back([m, name, alias]() mutable {
        py::class_<PrimitiveType<T>, DynamicType> cls(m, name.c_str());
        if (nullptr != alias) {
            m.attr(alias) = cls;
        }
        return ([cls, name]() mutable {
            init_dds_dynamic_primitive_defs<T>(cls, name);
        });
    });
}

template<>
void process_inits<PyPrimitiveType>(py::module& m, ClassInitList& l)
{
    init_dds_dynamic_primitive<char>(m, "Int8Type", l, "CharType");
    init_dds_dynamic_primitive<bool>(m, "BoolType", l);
    init_dds_dynamic_primitive<uint8_t>(m, "Uint8Type", l, "OctetType");
    init_dds_dynamic_primitive<int16_t>(m, "Int16Type", l, "ShortType");
    init_dds_dynamic_primitive<uint16_t>(m, "Uint16Type", l, "UShortType");
    init_dds_dynamic_primitive<int32_t>(m, "Int32Type", l, "LongType");
    init_dds_dynamic_primitive<uint32_t>(m, "Uint32Type", l, "ULongType");
    init_dds_dynamic_primitive<rti::core::int64>(m, "Int64Type", l, "LongLongType");
    init_dds_dynamic_primitive<rti::core::uint64>(m, "Uint64Type", l, "ULongLongType");
    init_dds_dynamic_primitive<float>(m, "Float32Type", l, "FloatType");
    init_dds_dynamic_primitive<double>(m, "Float64Type", l, "DoubleType");
    init_dds_dynamic_primitive<rti::core::LongDouble>(m, "Float128Type", l, "LongDoubleType");
    init_dds_dynamic_primitive<wchar_t>(m, "WcharType", l);
}

}  // namespace pyrti
