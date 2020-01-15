#include <pybind11/pybind11.h>
#include "PyConnext.hpp"
#include <dds/domain/qos/DomainParticipantFactoryQos.hpp>
#include "PyQos.hpp"

using namespace dds::domain::qos;
using namespace dds::core::policy;
using namespace rti::core::policy;

template<>
void pyrti::init_class_defs(py::class_<DomainParticipantFactoryQos>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a DomainParticipantFactoryQos with the default value for "
            "each policy."
        );
    pyrti::add_qos_property<DomainParticipantFactoryQos, EntityFactory>(cls, "entity_factory", "EntityFactory");
    pyrti::add_qos_property<DomainParticipantFactoryQos, SystemResourceLimits>(cls, "system_resource_limits", "SystemResourceLimits");
}

template<>
void pyrti::process_inits<DomainParticipantFactoryQos>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<DomainParticipantFactoryQos>(m, "DomainParticipantFactoryQos");
        }
    ); 
}

