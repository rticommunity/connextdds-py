#include "PyConnext.hpp"
#include <dds/core/BuiltinTopicTypes.hpp>
#include <PyInitType.hpp>
#include "PyInitOpaqueTypeContainers.hpp"


INIT_OPAQUE_TYPE_CONTAINERS(dds::core::BytesTopicType);


template<>
void pyrti::init_class_defs(py::class_<dds::core::BytesTopicType>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates a sample with an empty array of bytes."
        )
        .def(
            py::init<
                const std::vector<uint8_t>&
            >(),
            py::arg("data"),
            "Creates a sample from the provided byte sequence."
        )
        .def_property(
            "data",
            (std::vector<uint8_t> (dds::core::BytesTopicType::*)() const) &dds::core::BytesTopicType::data,
            (void (dds::core::BytesTopicType::*)(const std::vector<uint8_t>&)) &dds::core::BytesTopicType::data,
            "The byte sequence."
            "\n\n"
            "This property uses value semantics."
        )
        .def(
            "length",
            &dds::core::BytesTopicType::length,
            "Get the number of bytes."
        )
        .def(
            "__getitem__",
            [](const dds::core::BytesTopicType& b, uint32_t index) {
                return b[index];
            }
        )
        .def(
            "__setitem__",
            [](dds::core::BytesTopicType& b, uint32_t index, uint8_t value) {
                b[index] = value;
            }
        )
        .def(
            "__len__",
            &dds::core::BytesTopicType::length
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );;

    py::implicitly_convertible<std::vector<uint8_t>, dds::core::BytesTopicType>();
    pyrti::init_type<dds::core::BytesTopicType>(cls);
}


template<>
void pyrti::process_inits<dds::core::BytesTopicType>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<dds::core::BytesTopicType>(m, "BytesTopicType");
        }
    );
}