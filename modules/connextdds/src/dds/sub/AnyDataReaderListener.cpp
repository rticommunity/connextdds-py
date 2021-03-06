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
#include "PyAnyDataReaderListener.hpp"

namespace pyrti {

template<>
void init_class_defs(py::class_<
                     PyAnyDataReaderListener,
                     PyAnyDataReaderListenerTrampoline<>,
                     std::shared_ptr<PyAnyDataReaderListener>>& cls)
{
    cls.def(py::init<>())
            .def("on_requested_deadline_missed",
                 (void (PyAnyDataReaderListener::*)(
                         PyAnyDataReader&,
                         const dds::core::status::
                                 RequestedDeadlineMissedStatus&))
                         & PyAnyDataReaderListener::
                                 on_requested_deadline_missed,
                 "Requested deadline missed callback.")
            .def("on_requested_incompatible_qos",
                 (void (PyAnyDataReaderListener::*)(
                         PyAnyDataReader&,
                         const dds::core::status::
                                 RequestedIncompatibleQosStatus&))
                         & PyAnyDataReaderListener::
                                 on_requested_incompatible_qos,
                 "Requested incompatible QoS callback.")
            .def("on_sample_rejected",
                 (void (PyAnyDataReaderListener::*)(
                         PyAnyDataReader&,
                         const dds::core::status::SampleRejectedStatus&))
                         & PyAnyDataReaderListener::on_sample_rejected,
                 "Sample rejected callback.")
            .def("on_liveliness_changed",
                 (void (PyAnyDataReaderListener::*)(
                         PyAnyDataReader&,
                         const dds::core::status::LivelinessChangedStatus&))
                         & PyAnyDataReaderListener::on_liveliness_changed,
                 "Liveliness changed callback.")
            .def("on_data_available",
                 (void (PyAnyDataReaderListener::*)(PyAnyDataReader&))
                         & PyAnyDataReaderListener::on_data_available,
                 "Data available callback.")
            .def("on_subscription_matched",
                 (void (PyAnyDataReaderListener::*)(
                         PyAnyDataReader&,
                         const dds::core::status::SubscriptionMatchedStatus&))
                         & PyAnyDataReaderListener::on_subscription_matched,
                 "Subscription matched callback.")
            .def("on_sample_lost",
                 (void (PyAnyDataReaderListener::*)(
                         PyAnyDataReader&,
                         const dds::core::status::SampleLostStatus&))
                         & PyAnyDataReaderListener::on_sample_lost,
                 "Sample lost callback.");
}

template<>
void init_class_defs(py::class_<
                     PyNoOpAnyDataReaderListener,
                     PyAnyDataReaderListener,
                     PyNoOpAnyDataReaderListenerTrampoline<>,
                     std::shared_ptr<PyNoOpAnyDataReaderListener>>& cls)
{
    cls.def(py::init<>())
            .def("on_requested_deadline_missed",
                 (void (PyNoOpAnyDataReaderListener::*)(
                         PyAnyDataReader&,
                         const dds::core::status::
                                 RequestedDeadlineMissedStatus&))
                         & PyNoOpAnyDataReaderListener::
                                 on_requested_deadline_missed,
                 "Requested deadline missed callback.")
            .def("on_requested_incompatible_qos",
                 (void (PyNoOpAnyDataReaderListener::*)(
                         PyAnyDataReader&,
                         const dds::core::status::
                                 RequestedIncompatibleQosStatus&))
                         & PyNoOpAnyDataReaderListener::
                                 on_requested_incompatible_qos,
                 "Requested incompatible QoS callback.")
            .def("on_sample_rejected",
                 (void (PyNoOpAnyDataReaderListener::*)(
                         PyAnyDataReader&,
                         const dds::core::status::SampleRejectedStatus&))
                         & PyNoOpAnyDataReaderListener::on_sample_rejected,
                 "Sample rejected callback.")
            .def("on_liveliness_changed",
                 (void (PyNoOpAnyDataReaderListener::*)(
                         PyAnyDataReader&,
                         const dds::core::status::LivelinessChangedStatus&))
                         & PyNoOpAnyDataReaderListener::on_liveliness_changed,
                 "Liveliness changed callback.")
            .def("on_data_available",
                 (void (PyNoOpAnyDataReaderListener::*)(PyAnyDataReader&))
                         & PyNoOpAnyDataReaderListener::on_data_available,
                 "Data available callback.")
            .def("on_subscription_matched",
                 (void (PyNoOpAnyDataReaderListener::*)(
                         PyAnyDataReader&,
                         const dds::core::status::SubscriptionMatchedStatus&))
                         & PyNoOpAnyDataReaderListener::on_subscription_matched,
                 "Subscription matched callback.")
            .def("on_sample_lost",
                 (void (PyNoOpAnyDataReaderListener::*)(
                         PyAnyDataReader&,
                         const dds::core::status::SampleLostStatus&))
                         & PyNoOpAnyDataReaderListener::on_sample_lost,
                 "Sample lost callback.");
}

template<>
void process_inits<dds::sub::AnyDataReaderListener>(
        py::module& m,
        ClassInitList& l)
{
    l.push_back([m]() mutable {
        return init_class<
                PyAnyDataReaderListener,
                PyAnyDataReaderListenerTrampoline<>,
                std::shared_ptr<PyAnyDataReaderListener>>(
                        m,
                        "AnyDataReaderListener");
    });

    l.push_back([m]() mutable {
        return init_class<
                PyNoOpAnyDataReaderListener,
                PyAnyDataReaderListener,
                PyNoOpAnyDataReaderListenerTrampoline<>,
                std::shared_ptr<PyNoOpAnyDataReaderListener>>(
                        m,
                        "NoOpAnyDataReaderListener");
    });
}

}  // namespace pyrti
