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
#include <dds/core/BuiltinTopicTypes.hpp>
#include "PyInitType.hpp"
#include "PyInitOpaqueTypeContainers.hpp"

INIT_OPAQUE_TYPE_CONTAINERS(dds::core::KeyedBytesTopicType);

namespace pyrti {

template<>
void init_class_defs(py::class_<dds::core::KeyedBytesTopicType>& cls)
{
    cls.def(py::init([]() {
                emit_deprecation_warning(
                        "rti.connextdds.KeyedBytesTopicType",
                        "rti.types.builtin.KeyedBytes");
                return dds::core::KeyedBytesTopicType();
            }),
            "(Deprecated) Creates a sample with an empty array of bytes.");
    cls.def(py::init([](const std::string& key, const std::vector<uint8_t>& value) {
                emit_deprecation_warning(
                        "rti.connextdds.KeyedBytesTopicType",
                        "rti.types.builtin.KeyedBytes");
                return dds::core::KeyedBytesTopicType(key, value);
            }),
            py::arg("key"),
            py::arg("value"),
            "(Deprecated) Creates a sample from the provided key and value.");
    cls.def(py::init([](const std::pair<std::string, std::vector<uint8_t>>& p) {
                emit_deprecation_warning(
                        "rti.connextdds.KeyedBytesTopicType",
                        "rti.types.builtin.KeyedBytes");
                return dds::core::KeyedBytesTopicType(p.first, p.second);
            }),
            py::arg("pair"),
            "(Deprecated) Creates a sample from the provided key and value.");
    cls.def_property(
            "key",
            [](const dds::core::KeyedBytesTopicType& s) {
                return s.key().to_std_string();
            },
            [](dds::core::KeyedBytesTopicType& s, const std::string& value) {
                s.key(dds::core::string(value));
            },
            "The instance key.");
    cls.def_property(
            "value",
            (std::vector<uint8_t>(dds::core::KeyedBytesTopicType::*)() const)
                    & dds::core::KeyedBytesTopicType::value,
            (void(dds::core::KeyedBytesTopicType::*)(
                    const std::vector<uint8_t>&))
                    & dds::core::KeyedBytesTopicType::value,
            "The byte sequence."
            "\n\n"
            "This property's getter returns a deep copy.");
    cls.def("length",
            &dds::core::KeyedBytesTopicType::length,
            "Get the number of bytes.");
    cls.def("__getitem__",
            [](const dds::core::KeyedBytesTopicType& b, uint32_t index) {
                return b[index];
            });
    cls.def("__setitem__",
            [](dds::core::KeyedBytesTopicType& b,
                    uint32_t index,
                    uint8_t value) { b[index] = value; });
    cls.def("__len__", &dds::core::KeyedBytesTopicType::length);
    cls.def(py::self == py::self, "Test for equality.");
    cls.def(py::self != py::self, "Test for inequality.");

    py::implicitly_convertible<
            std::pair<std::string, std::vector<uint8_t>>,
            dds::core::KeyedBytesTopicType>();
}


template<>
void process_inits<dds::core::KeyedBytesTopicType>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m, &l]() mutable {
        return init_type_class<dds::core::KeyedBytesTopicType>(
                m,
                l,
                "KeyedBytesTopicType");
    });
}

}  // namespace pyrti
