#include "PyConnext.hpp"
#include <dds/core/BuiltinTopicTypes.hpp>
#include <PyInitType.hpp>
#include "PyInitOpaqueTypeContainers.hpp"

INIT_OPAQUE_TYPE_CONTAINERS(dds::core::BytesTopicType);
INIT_OPAQUE_TYPE_CONTAINERS(dds::core::KeyedBytesTopicType);
INIT_OPAQUE_TYPE_CONTAINERS(dds::core::StringTopicType);
INIT_OPAQUE_TYPE_CONTAINERS(dds::core::KeyedStringTopicType);

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
void pyrti::init_class_defs(py::class_<dds::core::KeyedBytesTopicType>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates a sample with an empty array of bytes."
        )
        .def(
            py::init<
                const std::string&,
                const std::vector<uint8_t>&
            >(),
            py::arg("key"),
            py::arg("value"),
            "Creates a sample from the provided key and value."
        )
        .def(
            py::init(
                [](const std::pair<std::string, std::vector<uint8_t>>& p) {
                    return dds::core::KeyedBytesTopicType(p.first, p.second);
                }
            ),
            py::arg("pair"),
            "Creates a sample from the provided key and value."
        )
        .def_property(
            "key",
            [](const dds::core::KeyedBytesTopicType& s) {
                return s.key().to_std_string();
            },
            [](dds::core::KeyedBytesTopicType& s, const std::string& value) {
                s.key(dds::core::string(value));
            },
            "The instance key."
        )
        .def_property(
            "value",
            (std::vector<uint8_t> (dds::core::KeyedBytesTopicType::*)() const) &dds::core::KeyedBytesTopicType::value,
            (void (dds::core::KeyedBytesTopicType::*)(const std::vector<uint8_t>&)) &dds::core::KeyedBytesTopicType::value,
            "The byte sequence."
            "\n\n"
            "This property uses value semantics."
        )
        .def(
            "length",
            &dds::core::KeyedBytesTopicType::length,
            "Get the number of bytes."
        )
        .def(
            "__getitem__",
            [](const dds::core::KeyedBytesTopicType& b, uint32_t index) {
                return b[index];
            }
        )
        .def(
            "__setitem__",
            [](dds::core::KeyedBytesTopicType& b, uint32_t index, uint8_t value) {
                b[index] = value;
            }
        )
        .def(
            "__len__",
            &dds::core::KeyedBytesTopicType::length
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );

    py::implicitly_convertible<std::pair<std::string, std::vector<uint8_t>>, dds::core::KeyedBytesTopicType>();
    pyrti::init_type<dds::core::KeyedBytesTopicType>(cls);
}

template<>
void pyrti::init_class_defs(py::class_<dds::core::StringTopicType>& cls) {
    cls
        .def(
            py::init<>(),
            "Initialize sample with an empty string"
        )
        .def(
            py::init<
                const std::string&
            >(),
            py::arg("data"),
            "Initialize sample with provided string."
        )
        .def_property(
            "data",
            [](const dds::core::StringTopicType& s) {
                return s.data().to_std_string();
            },
            [](dds::core::StringTopicType& s, const std::string& value) {
                s.data(dds::core::string(value));
            },
            "The sample's string data."
        )
        .def(
            "__str__",
            [](const dds::core::StringTopicType& s) {
                return s.data().to_std_string();
            }
        )
        .def(
            "__len__",
            [](const dds::core::StringTopicType& s) {
                return s.data().size();
            }
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );

    py::implicitly_convertible<std::string, dds::core::StringTopicType>();
    pyrti::init_type<dds::core::StringTopicType>(cls);
}

template<>
void pyrti::init_class_defs(py::class_<dds::core::KeyedStringTopicType>& cls) {
    cls
        .def(
            py::init<>(),
            "Initialize sample with an two empty strings."
        )
        .def(
            py::init<
                const std::string&,
                const std::string&
            >(),
            py::arg("key"),
            py::arg("value"),
            "Initialize sample with provided key and value."
        )
        .def(
            py::init([](const std::pair<std::string, std::string>& p){
                return dds::core::KeyedStringTopicType(p.first, p.second);
            }),
            py::arg("pair"),
            "Initialize sample with provided key/value pair."
        )
        .def_property(
            "key",
            [](const dds::core::KeyedStringTopicType& s) {
                return s.key().to_std_string();
            },
            [](dds::core::KeyedStringTopicType& s, const std::string& value) {
                s.key(dds::core::string(value));
            },
            "The instance's key string."
        )
        .def_property(
            "value",
            [](const dds::core::KeyedStringTopicType& s) {
                return s.value().to_std_string();
            },
            [](dds::core::KeyedStringTopicType& s, const std::string& value) {
                s.value(dds::core::string(value));
            },
            "The sample's value string."
        )
        .def(
            "__str__",
            [](const dds::core::KeyedStringTopicType& s) {
                return s.key().to_std_string() + " => " + s.value().to_std_string();
            }
        )
        .def(
            py::self == py::self,
            "Test for equality."
        )
        .def(
            py::self != py::self,
            "Test for inequality."
        );

    py::implicitly_convertible<std::pair<std::string, std::string>, dds::core::KeyedStringTopicType>();
    pyrti::init_type<dds::core::KeyedStringTopicType>(cls);
}

template<>
void pyrti::process_inits<pyrti::PyBuiltinTopicTypes>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<dds::core::BytesTopicType>(m, "BytesTopicType");
        }
    );

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<dds::core::KeyedBytesTopicType>(m, "KeyedBytesTopicType");
        }
    );

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<dds::core::StringTopicType>(m, "StringTopicType");
        }
    );

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<dds::core::KeyedStringTopicType>(m, "KeyedStringTopicType");
        }
    );
}
