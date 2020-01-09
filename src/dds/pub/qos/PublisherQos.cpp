#include "PyConnext.hpp"
#include <dds/pub/qos/PublisherQos.hpp>
#include "PyQos.hpp"

using namespace dds::pub::qos;
using namespace dds::core::policy;
using namespace rti::core::policy;

template<>
void pyrti::init_class_defs(py::class_<PublisherQos>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a PublisherQos with the default value for each policy."
        );
    pyrti::add_qos_property<PublisherQos, Presentation>(cls, "presentation", "Presentation");
    pyrti::add_qos_property<PublisherQos, Partition>(cls, "partition", "Partition");
    pyrti::add_qos_property<PublisherQos, GroupData>(cls, "group_data", "GroupData");
    pyrti::add_qos_property<PublisherQos, EntityFactory>(cls, "entity_factory", "EntityFactory");
    pyrti::add_qos_property<PublisherQos, AsynchronousPublisher>(cls, "asynchronous_publisher", "AsynchronousPublisher");
    pyrti::add_qos_property<PublisherQos, ExclusiveArea>(cls, "exclusive_area", "ExclusiveArea");
    pyrti::add_qos_property<PublisherQos, EntityName>(cls, "entity_name", "EntityName");
}

template<>
void pyrti::process_inits<PublisherQos>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<PublisherQos>(m, "PublisherQos");
        }
    ); 
}
