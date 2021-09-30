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

#include <pybind11/pybind11.h>
#include "PyConnext.hpp"
#include <dds/domain/qos/DomainParticipantFactoryQos.hpp>
#include "PyQos.hpp"

using namespace dds::domain::qos;
using namespace dds::core::policy;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<DomainParticipantFactoryQos>& cls)
{
    cls.def(py::init<>(),
            "Create a DomainParticipantFactoryQos with the default value for "
            "each policy.")
            .def(py::self == py::self, "Test for equality")
            .def(py::self != py::self, "Test for inequality.");
    add_qos_property<DomainParticipantFactoryQos, EntityFactory>(
            cls,
            "entity_factory",
            "EntityFactory");
    add_qos_property<DomainParticipantFactoryQos, SystemResourceLimits>(
            cls,
            "system_resource_limits",
            "SystemResourceLimits");
#if rti_connext_version_gte(6, 1, 0, 0)
    add_qos_string_conversions(cls);
#endif
}

template<>
void process_inits<DomainParticipantFactoryQos>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DomainParticipantFactoryQos>(
                m,
                "DomainParticipantFactoryQos");
    });
}

}  // namespace pyrti
