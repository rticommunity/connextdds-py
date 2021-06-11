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
void init_class_defs(py::class_<EntityFactory>& cls)
{
    cls.def(py::init<>(), "Creates the default policy.")
            .def(py::init<bool>(),
                 py::arg("auto_enable"),
                 "Specifies whether the entity acting as a factory "
                 "automatically "
                 "enables the instances it creates.")
            .def_property(
                    "autoenable_created_entities",
                    (bool (EntityFactory::*)() const)
                            & EntityFactory::autoenable_created_entities,
                    (EntityFactory & (EntityFactory::*) (bool) )
                            & EntityFactory::autoenable_created_entities,
                    "Whether the entity acting as a factory automatically "
                    "enables the "
                    "instances it creates.")
            .def_property_readonly_static(
                    "auto_enable",
                    [](py::object&) {
                        return EntityFactory::AutoEnable();
                    },
                    "Creates EntityFactory(true)")
            .def_property_readonly_static(
                    "manually_enable",
                    [](py::object&) {
                        return EntityFactory::ManuallyEnable();
                    },
                    "Creates EntityFactory(false)")
            .def(py::self == py::self, "Test for equality.")
            .def(py::self != py::self, "Test for inequality.");
}

template<>
void process_inits<EntityFactory>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<EntityFactory>(m, "EntityFactory");
    });
}

}  // namespace pyrti
