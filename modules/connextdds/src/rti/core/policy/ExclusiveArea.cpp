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
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<ExclusiveArea>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<bool>(),
                 py::arg("use_shared_exclusive_area"),
                 "Creates an instance specifying the use of shared or "
                 "exclusive "
                 "area.")
            .def_property(
                    "use_shared_exclusive_area",
                    (bool (ExclusiveArea::*)() const)
                            & ExclusiveArea::use_shared_exclusive_area,
                    (ExclusiveArea & (ExclusiveArea::*) (bool) )
                            & ExclusiveArea::use_shared_exclusive_area,
                    "Whether the Entity is protected by its own exclusive area "
                    "or the "
                    "shared one.")
            .def_property_readonly_static(
                    "shared_ea",
                    [](py::object&) {
                        return ExclusiveArea::SharedEA();
                    },
                    "Returns ExclusiveArea(true)")
            .def_property_readonly_static(
                    "exclusive_ea",
                    [](py::object&) {
                        return ExclusiveArea::ExclusiveEA();
                    },
                    "Returns ExclusiveArea(false)")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<ExclusiveArea>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<ExclusiveArea>(m, "ExclusiveArea");
    });
}

}  // namespace pyrti
