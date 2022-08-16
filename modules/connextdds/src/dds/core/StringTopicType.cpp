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

INIT_OPAQUE_TYPE_CONTAINERS(dds::core::StringTopicType);

namespace pyrti {

template<>
void init_class_defs(py::class_<dds::core::StringTopicType>& cls)
{
    cls.def(py::init([]() {
                emit_deprecation_warning(
                        "rti.connextdds.StringTopicType",
                        "rti.types.builtin.String");
                return dds::core::StringTopicType();
            }),
            "(Deprecated) Initialize sample with an empty string");

    cls.def(py::init([](const std::string& str) {
                emit_deprecation_warning(
                        "rti.connextdds.StringTopicType",
                        "rti.types.builtin.String");
                return dds::core::StringTopicType(str);
            }),
            py::arg("data"),
            "(Deprecated) Initialize sample with provided string.");

    cls.def_property(
            "data",
            [](const dds::core::StringTopicType& s) {
                return s.data().to_std_string();
            },
            [](dds::core::StringTopicType& s, const std::string& value) {
                s.data(dds::core::string(value));
            },
            "The sample's string data.");
    cls.def("__str__", [](const dds::core::StringTopicType& s) {
        return s.data().to_std_string();
    });
    cls.def("__len__", [](const dds::core::StringTopicType& s) {
        return s.data().size();
    });
    cls.def(py::self == py::self, "Test for equality.");
    cls.def(py::self != py::self, "Test for inequality.");

    py::implicitly_convertible<std::string, dds::core::StringTopicType>();
}


template<>
void process_inits<dds::core::StringTopicType>(py::module& m, ClassInitList& l)
{
    l.push_back([m, &l]() mutable {
        return init_type_class<dds::core::StringTopicType>(
                m,
                l,
                "StringTopicType");
    });
}

}  // namespace pyrti
