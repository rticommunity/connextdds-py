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
void init_class_defs(py::class_<Lifespan>& cls)
{
    cls.def(py::init<>(),
            "Creates the default policy with an infinite lifespan.")
            .def(py::init<const dds::core::Duration&>(),
                 py::arg("duration"),
                 "Creates a policy with the specified lifespan duration.")
            .def_property(
                    "duration",
                    (const dds::core::Duration (Lifespan::*)() const)
                            & Lifespan::duration,
                    (Lifespan & (Lifespan::*) (const dds::core::Duration&) )
                            & Lifespan::duration,
                    "Sets the maximum duration for which the data is valid."
                    "\n\n"
                    "This property's getter returns a deep copy.")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<Lifespan>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable { return init_class<Lifespan>(m, "Lifespan"); });
}

}  // namespace pyrti
