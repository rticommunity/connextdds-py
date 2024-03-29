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
#include <dds/core/policy/CorePolicy.hpp>

namespace py = pybind11;

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<DataTag>& cls)
{
    cls.def(py::init<>(), "Creates a policy with an empty sequence of tags.")
            .def(py::init([](std::vector<std::pair<std::string, std::string>>&
                                     entries) {
                     return DataTag(entries.begin(), entries.end());
                 }),
                 py::arg("entries"),
                 "Adds tags from a list.")
            .def(py::init([](py::dict& d) {
                     DataTag dt;
                     for (auto it : d) {
                         dt.set({py::cast<std::string>(it.first),
                                 py::cast<std::string>(it.second)});
                     }
                     return dt;
                 }),
                 py::arg("entries"),
                 "Adds tags from a dictionary.")
            .def(
                    "set",
                    [](DataTag& dt,
                       std::vector<std::pair<std::string, std::string>>*
                               entries) {
                        dt.set(entries->begin(), entries->end());
                    },
                    py::arg("entry_list"),
                    "Adds or assigns tags from a list of string pairs.")
            .def(
                    "set",
                    [](DataTag& dt, py::dict& entries) {
                        for (auto kv : entries) {
                            std::string key = py::cast<std::string>(kv.first);
                            std::string value =
                                    py::cast<std::string>(kv.second);
                            dt.set({key, value});
                        }
                    },
                    py::arg("entry_map"),
                    "Adds or assigns tags from a dictionary.")
            .def(
                    "set",
                    [](DataTag& dt, std::pair<std::string, std::string> entry) {
                        dt.set(entry);
                    },
                    py::arg("entry"),
                    "Adds or assigns a tag from a pair of strings.")
            .def(
                    "set",
                    [](DataTag& dt,
                       const std::string& key,
                       const std::string& value) {
                        dt.set({key, value});
                    },
                    "Adds or assigns a tag from a key string and a value "
                    "string.")
            .def("exists",
                 &DataTag::exists,
                 py::arg("key"),
                 "Returns true if a tag exists.")
            .def("get",
                 &DataTag::get,
                 py::arg("key"),
                 "Returns the value of a tag identified by a key if it exists.")
            .def("try_get",
                 &DataTag::try_get,
                 py::arg("key"),
                 "Returns the value of a tag identified by a key if it exists.")
            .def("remove",
                 &DataTag::remove,
                 py::arg("key"),
                 "Removes the tag identified by a key.")
            .def("size", &DataTag::size, "Returns the number of tags.")
            .def("get_all",
                 &DataTag::get_all,
                 "Retrieves a copy of all the entries.")
            .def("__len__", &DataTag::size)
            .def("__contains__",
                 [](const DataTag& dt, std::string& key) {
                     return dt.try_get(key).is_set();
                 })
            .def("__getitem__", &DataTag::try_get)
            .def("__setitem__",
                 [](DataTag& dt,
                    const std::string& key,
                    const std::string& value) {
                     dt.set({key, value});
                 })
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<DataTag>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable { return init_class<DataTag>(m, "DataTag"); });
}

}  // namespace pyrti