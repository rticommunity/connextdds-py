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
#include "PyAnyDataWriterListener.hpp"

namespace pyrti {

template<>
void init_class_defs(py::class_<
                     PyAnyDataWriterListener,
                     PyAnyDataWriterListenerTrampoline<>>& cls)
{
    cls.def(py::init<>())
            .def("on_offered_deadline_missed",
                 (void (PyAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const dds::core::status::OfferedDeadlineMissedStatus&))
                         & PyAnyDataWriterListener::on_offered_deadline_missed,
                 "Offered deadline missed callback.")
            .def("on_offered_incompatible_qos",
                 (void (PyAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const dds::core::status::
                                 OfferedIncompatibleQosStatus&))
                         & PyAnyDataWriterListener::on_offered_incompatible_qos,
                 "Offered incompatible QoS callback.")
            .def("on_liveliness_lost",
                 (void (PyAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const dds::core::status::LivelinessLostStatus&))
                         & PyAnyDataWriterListener::on_liveliness_lost,
                 "Liveliness lost callback.")
            .def("on_publication_matched",
                 (void (PyAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const dds::core::status::PublicationMatchedStatus&))
                         & PyAnyDataWriterListener::on_publication_matched,
                 "Publication matched callback.")
            .def("on_reliable_writer_cache_changed",
                 (void (PyAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const rti::core::status::
                                 ReliableWriterCacheChangedStatus&))
                         & PyAnyDataWriterListener::
                                 on_reliable_writer_cache_changed,
                 "Reliable writer cache changed callback.")
            .def("on_reliable_reader_activity_changed",
                 (void (PyAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const rti::core::status::
                                 ReliableReaderActivityChangedStatus&))
                         & PyAnyDataWriterListener::
                                 on_reliable_reader_activity_changed,
                 "Reliable reader activity changed callback.")
            .def("on_instance_replaced",
                 (void (PyAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const dds::core::InstanceHandle&))
                         & PyAnyDataWriterListener::on_instance_replaced,
                 "On instance replaced callback.")
            .def("on_application_acknowledgment",
                 (void (PyAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const rti::pub::AcknowledgmentInfo&))
                         & PyAnyDataWriterListener::
                                 on_application_acknowledgment,
                 "On application acknowledgment callback")
            .def("on_service_request_accepted",
                 (void (PyAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const rti::core::status::
                                 ServiceRequestAcceptedStatus&))
                         & PyAnyDataWriterListener::on_service_request_accepted,
                 "On service request accepted callback.");
}

template<>
void init_class_defs(py::class_<
                     PyNoOpAnyDataWriterListener,
                     PyAnyDataWriterListener,
                     PyNoOpAnyDataWriterListenerTrampoline<>>& cls)
{
    cls.def(py::init<>())
            .def("on_offered_deadline_missed",
                 (void (PyNoOpAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const dds::core::status::OfferedDeadlineMissedStatus&))
                         & PyNoOpAnyDataWriterListener::
                                 on_offered_deadline_missed,
                 "Offered deadline missed callback.")
            .def("on_offered_incompatible_qos",
                 (void (PyNoOpAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const dds::core::status::
                                 OfferedIncompatibleQosStatus&))
                         & PyNoOpAnyDataWriterListener::
                                 on_offered_incompatible_qos,
                 "Offered incompatible QoS callback.")
            .def("on_liveliness_lost",
                 (void (PyNoOpAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const dds::core::status::LivelinessLostStatus&))
                         & PyNoOpAnyDataWriterListener::on_liveliness_lost,
                 "Liveliness lost callback.")
            .def("on_publication_matched",
                 (void (PyNoOpAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const dds::core::status::PublicationMatchedStatus&))
                         & PyNoOpAnyDataWriterListener::on_publication_matched,
                 "Publication matched callback.")
            .def("on_reliable_writer_cache_changed",
                 (void (PyNoOpAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const rti::core::status::
                                 ReliableWriterCacheChangedStatus&))
                         & PyNoOpAnyDataWriterListener::
                                 on_reliable_writer_cache_changed,
                 "Reliable writer cache changed callback.")
            .def("on_reliable_reader_activity_changed",
                 (void (PyNoOpAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const rti::core::status::
                                 ReliableReaderActivityChangedStatus&))
                         & PyNoOpAnyDataWriterListener::
                                 on_reliable_reader_activity_changed,
                 "Reliable reader activity changed callback.")
            .def("on_instance_replaced",
                 (void (PyNoOpAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const dds::core::InstanceHandle&))
                         & PyNoOpAnyDataWriterListener::on_instance_replaced,
                 "On instance replaced callback.")
            .def("on_application_acknowledgment",
                 (void (PyNoOpAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const rti::pub::AcknowledgmentInfo&))
                         & PyNoOpAnyDataWriterListener::
                                 on_application_acknowledgment,
                 "On application acknowledgment callback")
            .def("on_service_request_accepted",
                 (void (PyNoOpAnyDataWriterListener::*)(
                         PyAnyDataWriter&,
                         const rti::core::status::
                                 ServiceRequestAcceptedStatus&))
                         & PyNoOpAnyDataWriterListener::
                                 on_service_request_accepted,
                 "On service request accepted callback.");
}

template<>
void process_inits<dds::pub::AnyDataWriterListener>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<
                PyAnyDataWriterListener,
                PyAnyDataWriterListenerTrampoline<>>(
                m,
                "AnyDataWriterListener");
    });

    l.push_back([m]() mutable {
        return init_class<
                PyNoOpAnyDataWriterListener,
                PyAnyDataWriterListener,
                PyNoOpAnyDataWriterListenerTrampoline<>>(
                m,
                "NoOpAnyDataWriterListener");
    });
}

}  // namespace pyrti
