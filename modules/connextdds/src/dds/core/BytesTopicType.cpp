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

INIT_OPAQUE_TYPE_CONTAINERS(dds::core::BytesTopicType);

namespace pyrti {

template<>
void init_class_defs(py::class_<dds::core::BytesTopicType>& cls)
{
    cls.def(py::init([]() {
                emit_deprecation_warning(
                        "rti.connextdds.BytesTopicType",
                        "rti.types.builtin.Bytes");
                return dds::core::BytesTopicType();
            }),
            "(Deprecated) Creates a sample with an empty array of bytes.");
    cls.def(py::init([](const std::vector<uint8_t>& bytes) {
                emit_deprecation_warning(
                        "rti.connextdds.BytesTopicType",
                        "rti.types.builtin.Bytes");
                return dds::core::BytesTopicType(bytes);
            }),
            py::arg("data"),
            "(Deprecated) Creates a sample from the provided byte sequence.");
    cls.def_property(
            "data",
            (std::vector<uint8_t>(dds::core::BytesTopicType::*)() const)
                    & dds::core::BytesTopicType::data,
            (void(dds::core::BytesTopicType::*)(const std::vector<uint8_t>&))
                    & dds::core::BytesTopicType::data,
            "The byte sequence."
            "\n\n"
            "This property's getter returns a deep copy.");
    cls.def("length",
            &dds::core::BytesTopicType::length,
            "Get the number of bytes.");
    cls.def("__getitem__",
            [](const dds::core::BytesTopicType& b, uint32_t index) {
                return b[index];
            });
    cls.def("__setitem__",
            [](dds::core::BytesTopicType& b, uint32_t index, uint8_t value) {
                b[index] = value;
            });
    cls.def("__len__", &dds::core::BytesTopicType::length);
    cls.def(py::self == py::self, "Test for equality.");
    cls.def(py::self != py::self, "Test for inequality.");

    py::implicitly_convertible<
            std::vector<uint8_t>,
            dds::core::BytesTopicType>();
}


template<>
void process_inits<dds::core::BytesTopicType>(py::module& m, ClassInitList& l)
{
    l.push_back([m, &l]() mutable {
        return init_type_class<dds::core::BytesTopicType>(
                m,
                l,
                "BytesTopicType");
    });
}

}  // namespace pyrti
