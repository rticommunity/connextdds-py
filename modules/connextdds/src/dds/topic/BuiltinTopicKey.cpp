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
#include <dds/topic/BuiltinTopicKey.hpp>

using namespace dds::topic;

namespace pyrti {

template<>
void init_class_defs(py::class_<BuiltinTopicKey>& cls)
{
    cls.def(py::init<>(), "Creates a key whose value is all zeros.")
            .def_property_readonly(
                    "value",
                    &BuiltinTopicKey::value,
                    "Returns a copy of the four integers that represent the "
                    "key.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<BuiltinTopicKey>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<BuiltinTopicKey>(m, "BuiltinTopicKey");
    });
}

}  // namespace pyrti
