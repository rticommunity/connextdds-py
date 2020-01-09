#include "PyConnext.hpp"
#include <dds/sub/qos/SubscriberQos.hpp>
#include "PyQos.hpp"

using namespace dds::sub::qos;
using namespace dds::core::policy;
using namespace rti::core::policy;

template<>
void pyrti::init_class_defs(py::class_<SubscriberQos>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a SubscriberQos with the default value for each policy."
        );
    pyrti::add_qos_property<SubscriberQos, Presentation>(cls, "presentation", "Presentation");
    pyrti::add_qos_property<SubscriberQos, Partition>(cls, "partition", "Partition");
    pyrti::add_qos_property<SubscriberQos, GroupData>(cls, "group_data", "GroupData");
    pyrti::add_qos_property<SubscriberQos, EntityFactory>(cls, "entity_factory", "EntityFactory");
    pyrti::add_qos_property<SubscriberQos, ExclusiveArea>(cls, "exclusive_area", "ExclusiveArea");
    pyrti::add_qos_property<SubscriberQos, EntityName>(cls, "entity_name", "EntityName");
}

template<>
void pyrti::process_inits<SubscriberQos>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<SubscriberQos>(m, "SubscriberQos");
        }
    ); 
}
