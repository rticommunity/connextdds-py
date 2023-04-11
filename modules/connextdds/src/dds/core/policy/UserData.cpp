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
void init_class_defs(py::class_<UserData>& cls)
{
    cls.def(py::init<>(),
               "Creates an instance with an empty sequence of bytes.")
            .def(py::init<const std::vector<uint8_t>&>(),
                    py::arg("data"),
                    "Creates an instance with a sequence of bytes.")
            .def_property(
                    "value",
                    (const std::vector<uint8_t> (UserData::*)() const)
                            & UserData::value,
                    [](UserData& ud, const std::vector<uint8_t>& v) {
                       return ud.value(v.begin(), v.end());
                    },
                    "The user data.")
            .def(
                    "__iter__",
                    [](UserData& ud) {
                        return py::make_iterator(ud.begin(), ud.end());
                    },
                    py::keep_alive<0, 1>())
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<UserData>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable { return init_class<UserData>(m, "UserData"); });
}

}  // namespace pyrti
