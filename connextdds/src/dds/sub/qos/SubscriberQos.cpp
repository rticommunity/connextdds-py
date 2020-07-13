#include "PyConnext.hpp"
#include <dds/sub/qos/SubscriberQos.hpp>
#include "PyQos.hpp"

using namespace dds::sub::qos;
using namespace dds::core::policy;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<SubscriberQos>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a SubscriberQos with the default value for each policy."
        )
        .def(
            py::init(
                [](const PySubscriber& sub) {
                    return sub.qos();
                }
            ),
            py::arg("subscriber"),
            "Create a SubscriberQos with settings equivalent to those "
            "of the provided Subscriber object."
        )
        .def(
            py::self == py::self,
            "Test for equality"
        )
        .def (
            py::self != py::self,
            "Test for inequality."
        );

    add_qos_property<SubscriberQos, Presentation>(cls, "presentation", "Presentation");
    add_qos_property<SubscriberQos, Partition>(cls, "partition", "Partition");
    add_qos_property<SubscriberQos, GroupData>(cls, "group_data", "GroupData");
    add_qos_property<SubscriberQos, EntityFactory>(cls, "entity_factory", "EntityFactory");
    add_qos_property<SubscriberQos, ExclusiveArea>(cls, "exclusive_area", "ExclusiveArea");
    add_qos_property<SubscriberQos, EntityName>(cls, "entity_name", "EntityName");
}

template<>
void process_inits<SubscriberQos>(py::module& m, ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return init_class<SubscriberQos>(m, "SubscriberQos");
        }
    ); 
}

}
