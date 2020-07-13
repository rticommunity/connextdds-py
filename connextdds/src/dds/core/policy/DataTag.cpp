#include "PyConnext.hpp"
#include <dds/core/policy/CorePolicy.hpp>

#if rti_connext_version_gte(6, 0, 0)

namespace py = pybind11;

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<DataTag>& cls) {
    cls
        .def(
            py::init<>(),
            "Creates a policy with an empty sequence of tags."
        )
        .def(
            py::init([](std::vector<std::pair<std::string, std::string>>& entries) {
                return DataTag(entries.begin(), entries.end());
            }),
            py::arg("entries"),
            "Adds tags from a list."
        )
        .def(
            py::init([](std::map<std::string, std::string>& entries) {
                return DataTag(entries.begin(), entries.end());
            }),
            py::arg("entries"),
            "Adds tags from a dictionary."
        )
        .def(
            py::init([](py::dict& d) {
                DataTag dt;
                for (auto it : d) {
                    dt.set(std::pair<std::string, std::string>(
                        py::cast<std::string>(it.first),
                        py::cast<std::string>(it.second)
                    ));
                }
                return dt;
            }),
            py::arg("entries"),
            "Adds tags from a dictionary."
        )
        .def(
            "set",
            [](DataTag& dt, std::vector<std::pair<std::string, std::string>>* entries) {
                dt.set(entries->begin(), entries->end());
            },
            py::arg("entry_list"),
            "Adds or assigns tags from a list of string pairs."
        )
        .def(
            "set",
            [](DataTag& dt, py::dict& entries) {
                for (auto kv : entries) {
                    std::string key = py::cast<std::string>(kv.first);
                    std::string value = py::cast<std::string>(kv.second);
                    dt.set(std::pair<std::string, std::string>(key, value));
                }
            },
            py::arg("entry_map"),
            "Adds or assigns tags from a dictionary."
        )
        .def(
            "set",
            [](DataTag& dt, std::pair<std::string, std::string> entry) {
                dt.set(entry);
            },
            py::arg("entry"),
            "Adds or assigns a tag from a pair of strings."
        )
        .def(
            "set",
            [](DataTag& dt, const std::string& key, const std::string& value) {
                dt.set(std::pair<std::string, std::string>(key, value));
            },
            "Adds or assigns a tag from a key string and a value string."
        )
        .def(
            "exists",
            &DataTag::exists,
            py::arg("key"),
            "Returns true if a tag exists."
        )
        .def(
            "get",
            &DataTag::get,
            py::arg("key"),
            "Returns the value of a tag identified by a key if it exists."
        )
        .def(
            "try_get",
            &DataTag::try_get,
            py::arg("key"),
            "Returns the value of a tag identified by a key if it exists."
        )
        .def(
            "remove",
            &DataTag::remove,
            py::arg("key"),
            "Removes the tag identified by a key."
        )
        .def(
            "size",
            &DataTag::size,
            "Returns the number of tags."
        )
        .def(
            "get_all",
            &DataTag::get_all,
            "Retrieves a copy of all the entries."
        )
        .def(
            "__len__",
            &DataTag::size
        )
        .def(
            "__contains__",
            [](const DataTag& dt, std::string& key) {
                return dt.try_get(key).is_set();
            }
        )
        .def(
            "__getitem__",
            &DataTag::try_get
        )
        .def(
            "__setitem__",
            [](DataTag& dt, const std::string& key, const std::string& value) {
                dt.set(std::pair<std::string, std::string>(key, value));
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
}

template<>
void process_inits<DataTag>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<DataTag>(
                m, "DataTag");
        }
    );
}

}

#endif
