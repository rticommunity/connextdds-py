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
#include <dds/sub/qos/SubscriberQos.hpp>
#include "PyQos.hpp"

using namespace dds::sub::qos;
using namespace dds::core::policy;
using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<SubscriberQos>& cls)
{
    cls.def(py::init<>(),
            "Create a SubscriberQos with the default value for each policy.")
            .def(py::init([](const PySubscriber& sub) { return sub.qos(); }),
                 py::arg("subscriber"),
                 "Create a SubscriberQos with settings equivalent to those "
                 "of the provided Subscriber object.")
            .def(py::self == py::self, "Test for equality")
            .def(py::self != py::self, "Test for inequality.");

    add_qos_property<SubscriberQos, Presentation>(
            cls,
            "presentation",
            "Presentation");
    add_qos_property<SubscriberQos, Partition>(cls, "partition", "Partition");
    add_qos_property<SubscriberQos, GroupData>(cls, "group_data", "GroupData");
    add_qos_property<SubscriberQos, EntityFactory>(
            cls,
            "entity_factory",
            "EntityFactory");
    add_qos_property<SubscriberQos, ExclusiveArea>(
            cls,
            "exclusive_area",
            "ExclusiveArea");
    add_qos_property<SubscriberQos, EntityName>(
            cls,
            "entity_name",
            "EntityName");
#if rti_connext_version_gte(6, 1, 0)
    add_qos_string_conversions(cls);
#endif
}

template<>
void process_inits<SubscriberQos>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<SubscriberQos>(m, "SubscriberQos");
    });
}

}  // namespace pyrti
