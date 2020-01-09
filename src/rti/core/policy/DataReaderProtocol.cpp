#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core;
using namespace rti::core::policy;

template<>
void pyrti::init_class_defs(py::class_<DataReaderProtocol>& cls) {
    cls
        .def(
            py::init<>(),
            "Create a default DataReaderProtocol policy."
        )
        .def_property(
            "virtual_guid",
            (Guid (DataReaderProtocol::*)() const) &DataReaderProtocol::virtual_guid,
            (DataReaderProtocol& (DataReaderProtocol::*)(const Guid&)) &DataReaderProtocol::virtual_guid,
            "Get/set a copy of the virtual GUID."
        )
        .def_property(
            "rtps_object_id",
            (uint32_t (DataReaderProtocol::*)() const) &DataReaderProtocol::rtps_object_id,
            (DataReaderProtocol& (DataReaderProtocol::*)(uint32_t)) &DataReaderProtocol::rtps_object_id,
            "Get/set the RTPS object ID."
        )
        .def_property(
            "expects_inline_qos",
            (bool (DataReaderProtocol::*)() const) &DataReaderProtocol::expects_inline_qos,
            (DataReaderProtocol& (DataReaderProtocol::*)(bool)) &DataReaderProtocol::expects_inline_qos,
            "Get/set the boolean for whether the reader will expect inline "
            "QoS with each sample."
        )
        .def_property(
            "disable_positive_acks",
            (bool (DataReaderProtocol::*)() const) &DataReaderProtocol::disable_positive_acks,
            (DataReaderProtocol& (DataReaderProtocol::*)(bool)) &DataReaderProtocol::disable_positive_acks,
            "Get/set the boolean for whether the reader will use send "
            "positive acknowledgments."
        )
        .def_property(
            "propagate_dispose_of_unregistered_instances",
            (bool (DataReaderProtocol::*)() const) &DataReaderProtocol::propagate_dispose_of_unregistered_instances,
            (DataReaderProtocol& (DataReaderProtocol::*)(bool)) &DataReaderProtocol::propagate_dispose_of_unregistered_instances,
            "Get/set the boolean for whether an instance can move to the "
            "not_alive_disposed state without being in the alive state."
        )
        .def_property(
            "propagate_unregister_of_disposed_instances",
            (bool (DataReaderProtocol::*)() const) &DataReaderProtocol::propagate_unregister_of_disposed_instances,
            (DataReaderProtocol& (DataReaderProtocol::*)(bool)) &DataReaderProtocol::propagate_unregister_of_disposed_instances,
            "Get/set the boolean for whether an instance can move to the "
            "not_alive_no_writers state without being in the alive state."
        )
        .def_property(
            "rtps_reliable_reader",
            (RtpsReliableReaderProtocol& (DataReaderProtocol::*)()) &DataReaderProtocol::rtps_reliable_reader,
            (DataReaderProtocol& (DataReaderProtocol::*)(const RtpsReliableReaderProtocol&)) &DataReaderProtocol::rtps_reliable_reader,
            "Get/set the reliable reader protocol settings."
        )
        .def(
            py::self == py::self,
            "Compare for equality."
        )
        .def(
            py::self != py::self,
            "Compare for inequality."
        );
}

template<>
void pyrti::process_inits<DataReaderProtocol>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<DataReaderProtocol>(m, "DataReaderProtocol");
        }
    );
}
