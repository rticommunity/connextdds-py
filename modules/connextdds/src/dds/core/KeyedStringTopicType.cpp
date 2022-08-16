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

INIT_OPAQUE_TYPE_CONTAINERS(dds::core::KeyedStringTopicType);

namespace pyrti {

template<>
void init_class_defs(py::class_<dds::core::KeyedStringTopicType>& cls)
{
    cls.def(py::init([]() {
                emit_deprecation_warning(
                        "rti.connextdds.KeyedStringTopicType",
                        "rti.types.builtin.KeyedString");
                return dds::core::KeyedStringTopicType();
            }),
            "(Deprecated) Initialize sample with an two empty strings.");

    cls.def(py::init([](const std::string& key, const std::string& value) {
                emit_deprecation_warning(
                        "rti.connextdds.KeyedStringTopicType",
                        "rti.types.builtin.KeyedString");
                return dds::core::KeyedStringTopicType(key, value);
            }),
            py::arg("key"),
            py::arg("value"),
            "(Deprecated) Initialize sample with provided key and value.");
    cls.def(py::init([](const std::pair<std::string, std::string>& p) {
                emit_deprecation_warning(
                        "rti.connextdds.KeyedStringTopicType",
                        "rti.types.builtin.KeyedString");
                return dds::core::KeyedStringTopicType(p.first, p.second);
            }),
            py::arg("pair"),
            "(Deprecated) Initialize sample with provided key/value pair.");
    cls.def_property(
            "key",
            [](const dds::core::KeyedStringTopicType& s) {
                return s.key().to_std_string();
            },
            [](dds::core::KeyedStringTopicType& s, const std::string& value) {
                s.key(dds::core::string(value));
            },
            "The instance's key string.");
    cls.def_property(
            "value",
            [](const dds::core::KeyedStringTopicType& s) {
                return s.value().to_std_string();
            },
            [](dds::core::KeyedStringTopicType& s, const std::string& value) {
                s.value(dds::core::string(value));
            },
            "The sample's value string.");
    cls.def("__str__", [](const dds::core::KeyedStringTopicType& s) {
        return s.key().to_std_string() + " => " + s.value().to_std_string();
    });
    cls.def(py::self == py::self, "Test for equality.");
    cls.def(py::self != py::self, "Test for inequality.");

    py::implicitly_convertible<
            std::pair<std::string, std::string>,
            dds::core::KeyedStringTopicType>();
}


template<>
void process_inits<dds::core::KeyedStringTopicType>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m, &l]() mutable {
        return init_type_class<dds::core::KeyedStringTopicType>(
                m,
                l,
                "KeyedStringTopicType");
    });
}

}  // namespace pyrti
