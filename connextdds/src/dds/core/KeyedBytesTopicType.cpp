#include "PyConnext.hpp"
#include "PyInitOpaqueTypeContainers.hpp"
#include "PyInitType.hpp"
#include "PySeq.hpp"
#include <dds/core/BuiltinTopicTypes.hpp>

INIT_OPAQUE_TYPE_CONTAINERS(dds::core::KeyedBytesTopicType);

namespace pyrti {

template <>
void init_class_defs(py::class_<dds::core::KeyedBytesTopicType> &cls) {
  cls.def(py::init<>(), "Creates a sample with an empty array of bytes.")
      .def(py::init<const std::string &, const std::vector<uint8_t> &>(),
           py::arg("key"), py::arg("value"),
           "Creates a sample from the provided key and value.")
      .def(py::init([](const std::pair<std::string, std::vector<uint8_t>> &p) {
             return dds::core::KeyedBytesTopicType(p.first, p.second);
           }),
           py::arg("pair"), "Creates a sample from the provided key and value.")
      .def_property(
          "key",
          [](const dds::core::KeyedBytesTopicType &s) {
            return s.key().to_std_string();
          },
          [](dds::core::KeyedBytesTopicType &s, const std::string &value) {
            s.key(dds::core::string(value));
          },
          "The instance key.")
      .def_property(
          "value",
          (std::vector<uint8_t>(dds::core::KeyedBytesTopicType::*)() const) &
              dds::core::KeyedBytesTopicType::value,
          (void (dds::core::KeyedBytesTopicType::*)(
              const std::vector<uint8_t> &)) &
              dds::core::KeyedBytesTopicType::value,
          "The byte sequence."
          "\n\n"
          "This property's getter returns a deep copy.")
      .def("length", &dds::core::KeyedBytesTopicType::length,
           "Get the number of bytes.")
      .def("__getitem__", [](const dds::core::KeyedBytesTopicType &b,
                             uint32_t index) { return b[index]; })
      .def("__setitem__", [](dds::core::KeyedBytesTopicType &b, uint32_t index,
                             uint8_t value) { b[index] = value; })
      .def("__len__", &dds::core::KeyedBytesTopicType::length)
      .def(py::self == py::self, "Test for equality.")
      .def(py::self != py::self, "Test for inequality.");

  py::implicitly_convertible<std::pair<std::string, std::vector<uint8_t>>,
                             dds::core::KeyedBytesTopicType>();
}

template <>
void process_inits<dds::core::KeyedBytesTopicType>(py::module &m,
                                                   ClassInitList &l) {
  l.push_back([m, &l]() mutable {
    return init_type_class<dds::core::KeyedBytesTopicType>(
        m, l, "KeyedBytesTopicType");
  });
}

} // namespace pyrti
