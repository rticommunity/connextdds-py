#include "PyConnext.hpp"
#include "PyInitOpaqueTypeContainers.hpp"
#include "PyInitType.hpp"
#include "PySeq.hpp"
#include <dds/core/BuiltinTopicTypes.hpp>

INIT_OPAQUE_TYPE_CONTAINERS(dds::core::StringTopicType);

namespace pyrti {

template<>
void init_class_defs(py::class_<dds::core::StringTopicType>& cls)
{
    cls.def(py::init<>(), "Initialize sample with an empty string")
            .def(py::init<const std::string&>(),
                 py::arg("data"),
                 "Initialize sample with provided string.")
            .def_property(
                    "data",
                    [](const dds::core::StringTopicType& s) {
                        return s.data().to_std_string();
                    },
                    [](dds::core::StringTopicType& s,
                       const std::string& value) {
                        s.data(dds::core::string(value));
                    },
                    "The sample's string data.")
            .def("__str__",
                 [](const dds::core::StringTopicType& s) {
                     return s.data().to_std_string();
                 })
            .def("__len__",
                 [](const dds::core::StringTopicType& s) {
                     return s.data().size();
                 })
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");

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
