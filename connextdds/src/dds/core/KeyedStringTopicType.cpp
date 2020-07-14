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
    cls.def(py::init<>(), "Initialize sample with an two empty strings.")
            .def(py::init<const std::string&, const std::string&>(),
                 py::arg("key"),
                 py::arg("value"),
                 "Initialize sample with provided key and value.")
            .def(py::init([](const std::pair<std::string, std::string>& p) {
                     return dds::core::KeyedStringTopicType(p.first, p.second);
                 }),
                 py::arg("pair"),
                 "Initialize sample with provided key/value pair.")
            .def_property(
                    "key",
                    [](const dds::core::KeyedStringTopicType& s) {
                        return s.key().to_std_string();
                    },
                    [](dds::core::KeyedStringTopicType& s,
                       const std::string& value) {
                        s.key(dds::core::string(value));
                    },
                    "The instance's key string.")
            .def_property(
                    "value",
                    [](const dds::core::KeyedStringTopicType& s) {
                        return s.value().to_std_string();
                    },
                    [](dds::core::KeyedStringTopicType& s,
                       const std::string& value) {
                        s.value(dds::core::string(value));
                    },
                    "The sample's value string.")
            .def("__str__",
                 [](const dds::core::KeyedStringTopicType& s) {
                     return s.key().to_std_string() + " => "
                             + s.value().to_std_string();
                 })
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");

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
