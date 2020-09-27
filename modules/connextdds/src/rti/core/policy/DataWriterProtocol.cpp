#include "PyConnext.hpp"
#include <rti/core/policy/CorePolicy.hpp>

using namespace rti::core::policy;

namespace pyrti {

template<>
void init_class_defs(py::class_<DataWriterProtocol>& cls)
{
    cls.def(py::init<>(), "Create a default DataWriterProtocol policy.")
            .def_property(
                    "virtual_guid",
                    (rti::core::Guid(DataWriterProtocol::*)() const)
                            & DataWriterProtocol::virtual_guid,
                    (DataWriterProtocol
                     & (DataWriterProtocol::*) (const rti::core::Guid&) )
                            & DataWriterProtocol::virtual_guid,
                    "The virtual GUID of the DataWriter. This property uses "
                    "value "
                    "semantics.")
            .def_property(
                    "rtps_object_id",
                    (uint32_t(DataWriterProtocol::*)() const)
                            & DataWriterProtocol::rtps_object_id,
                    (DataWriterProtocol & (DataWriterProtocol::*) (uint32_t))
                            & DataWriterProtocol::rtps_object_id,
                    "The RTPS object ID of the DataWriter.")
            .def_property(
                    "push_on_write",
                    (bool (DataWriterProtocol::*)() const)
                            & DataWriterProtocol::push_on_write,
                    (DataWriterProtocol & (DataWriterProtocol::*) (bool) )
                            & DataWriterProtocol::push_on_write,
                    "Determines whether a sample is pushed on a call to write.")
            .def_property(
                    "disable_positive_acks",
                    (bool (DataWriterProtocol::*)() const)
                            & DataWriterProtocol::disable_positive_acks,
                    (DataWriterProtocol & (DataWriterProtocol::*) (bool) )
                            & DataWriterProtocol::disable_positive_acks,
                    "Controls whether the DataWriter expects positive "
                    "acknowledgments from matched DataReaders.")
            .def_property(
                    "disable_inline_keyhash",
                    (bool (DataWriterProtocol::*)() const)
                            & DataWriterProtocol::disable_inline_keyhash,
                    (DataWriterProtocol & (DataWriterProtocol::*) (bool) )
                            & DataWriterProtocol::disable_inline_keyhash,
                    "Controls whether a keyhash is propagated on the wire with "
                    "each "
                    "sample.")
            .def_property(
                    "serialize_key_with_dispose",
                    (bool (DataWriterProtocol::*)() const)
                            & DataWriterProtocol::serialize_key_with_dispose,
                    (DataWriterProtocol & (DataWriterProtocol::*) (bool) )
                            & DataWriterProtocol::serialize_key_with_dispose,
                    "Controls whether or not the serialized key is propagated "
                    "on the "
                    "wire with dispose samples.")
            .def_property(
                    "propagate_app_ack_with_no_response",
                    (bool (DataWriterProtocol::*)() const)
                            & DataWriterProtocol::
                                    propagate_app_ack_with_no_response,
                    (DataWriterProtocol & (DataWriterProtocol::*) (bool) )
                            & DataWriterProtocol::
                                    propagate_app_ack_with_no_response,
                    "Controls whether or not a DataWriter receives "
                    "on_application_acknowledgment notifications with an empty "
                    "or "
                    "invalid response.")
            .def_property(
                    "rtps_reliable_writer",
                    (RtpsReliableWriterProtocol & (DataWriterProtocol::*) ())
                            & DataWriterProtocol::rtps_reliable_writer,
                    (DataWriterProtocol
                     & (DataWriterProtocol::*) (const RtpsReliableWriterProtocol&) )
                            & DataWriterProtocol::rtps_reliable_writer,
                    "RTPS protocol-related configuration settings for the RTPS "
                    "reliable writer associated to a DataWriter. This "
                    "parameter only "
                    "has effect if both the writer and the matching reader are "
                    "configured with ReliabilityKind.RELIABLE.")
            .def(py::self == py::self)
            .def(py::self != py::self);
}

template<>
void process_inits<DataWriterProtocol>(py::module& m, ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<DataWriterProtocol>(m, "DataWriterProtocol");
    });
}

}  // namespace pyrti
