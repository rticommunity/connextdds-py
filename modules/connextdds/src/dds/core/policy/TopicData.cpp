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

using namespace dds::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<TopicData>& cls)
{
    cls.def(py::init<>(), "Create an empty TopicData QoS policy.")
            .def(py::init<const std::vector<uint8_t>>(),
                 py::arg("data"),
                 "Create a TopicData object from a data sequence.")
            .def_property(
                    "value",
                    (const std::vector<uint8_t> (TopicData::*)() const)
                            & TopicData::value,
                    [](TopicData& td, std::vector<uint8_t> v) {
                        return td.value(v.begin(), v.end());
                    },
                    "Get/set a copy of the TopicData value.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<TopicData>(py::module& m, ClassInitList& l)
{
    l.push_back(
            [m]() mutable { return init_class<TopicData>(m, "TopicData"); });
}

}  // namespace pyrti