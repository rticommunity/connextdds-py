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
    cls.def(py::init<>(), "Creates a sample with an empty array of bytes.")
            .def(py::init<const std::vector<uint8_t>&>(),
                 py::arg("data"),
                 "Creates a sample from the provided byte sequence.")
            .def_property(
                    "data",
                    (std::vector<uint8_t>(dds::core::BytesTopicType::*)() const)
                            & dds::core::BytesTopicType::data,
                    (void (dds::core::BytesTopicType::*)(
                            const std::vector<uint8_t>&))
                            & dds::core::BytesTopicType::data,
                    "The byte sequence."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def("length",
                 &dds::core::BytesTopicType::length,
                 "Get the number of bytes.")
            .def("__getitem__",
                 [](const dds::core::BytesTopicType& b, uint32_t index) {
                     return b[index];
                 })
            .def("__setitem__",
                 [](dds::core::BytesTopicType& b,
                    uint32_t index,
                    uint8_t value) { b[index] = value; })
            .def("__len__", &dds::core::BytesTopicType::length)
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
    ;

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
