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
void init_class_defs(py::class_<OwnershipStrength>& cls)
{
    cls.def(py::init<>(), "Creates an instance with the default strength (0).")
            .def(py::init<int32_t>(),
                 py::arg("strength"),
                 "Creates an instance with the specified strength value.")
            .def_property(
                    "value",
                    (int32_t(OwnershipStrength::*)() const)
                            & OwnershipStrength::value,
                    (OwnershipStrength & (OwnershipStrength::*) (int32_t))
                            & OwnershipStrength::value,
                    "The ownership kind.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<OwnershipStrength>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<OwnershipStrength>(m, "OwnershipStrength");
    });
}

}  // namespace pyrti
