#include "PyConnext.hpp"
#include "PyAnyDataReaderListener.hpp"

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyAnyDataReaderListener, pyrti::PyAnyDataReaderListenerTrampoline<>>& cls) {
    cls
        .def(
            py::init<>()
        )
        .def(
            "on_requested_deadline_missed",
            (void (pyrti::PyAnyDataReaderListener::*)(pyrti::PyAnyDataReader&, const dds::core::status::RequestedDeadlineMissedStatus&)) &pyrti::PyAnyDataReaderListener::on_requested_deadline_missed,
            "Requested deadline missed callback."
        )
        .def(
            "on_requested_incompatible_qos",
            (void (pyrti::PyAnyDataReaderListener::*)(pyrti::PyAnyDataReader&, const dds::core::status::RequestedIncompatibleQosStatus&)) &pyrti::PyAnyDataReaderListener::on_requested_incompatible_qos,
            "Requested incompatible QoS callback."
        )
        .def(
            "on_sample_rejected",
            (void (pyrti::PyAnyDataReaderListener::*)(pyrti::PyAnyDataReader&, const dds::core::status::SampleRejectedStatus&)) &pyrti::PyAnyDataReaderListener::on_sample_rejected,
            "Sample rejected callback."
        )
        .def(
            "on_liveliness_changed",
            (void (pyrti::PyAnyDataReaderListener::*)(pyrti::PyAnyDataReader&, const dds::core::status::LivelinessChangedStatus&)) &pyrti::PyAnyDataReaderListener::on_liveliness_changed,
            "Liveliness changed callback."
        )
        .def(
            "on_data_available",
            (void (pyrti::PyAnyDataReaderListener::*)(pyrti::PyAnyDataReader&)) &pyrti::PyAnyDataReaderListener::on_data_available,
            "Data available callback."
        )
        .def(
            "on_subscription_matched",
            (void (pyrti::PyAnyDataReaderListener::*)(pyrti::PyAnyDataReader&, const dds::core::status::SubscriptionMatchedStatus&)) &pyrti::PyAnyDataReaderListener::on_subscription_matched,
            "Subscription matched callback."
        )
        .def(
            "on_sample_lost",
            (void (pyrti::PyAnyDataReaderListener::*)(pyrti::PyAnyDataReader&, const dds::core::status::SampleLostStatus&)) &pyrti::PyAnyDataReaderListener::on_sample_lost,
            "Sample lost callback."
        );
}

template<>
void pyrti::init_class_defs(py::class_<pyrti::PyNoOpAnyDataReaderListener, pyrti::PyAnyDataReaderListener, pyrti::PyNoOpAnyDataReaderListenerTrampoline<>>& cls) {
    cls
        .def(
            py::init<>()
        )
        .def(
            "on_requested_deadline_missed",
            (void (pyrti::PyNoOpAnyDataReaderListener::*)(pyrti::PyAnyDataReader&, const dds::core::status::RequestedDeadlineMissedStatus&)) &pyrti::PyNoOpAnyDataReaderListener::on_requested_deadline_missed,
            "Requested deadline missed callback."
        )
        .def(
            "on_requested_incompatible_qos",
            (void (pyrti::PyNoOpAnyDataReaderListener::*)(pyrti::PyAnyDataReader&, const dds::core::status::RequestedIncompatibleQosStatus&)) &pyrti::PyNoOpAnyDataReaderListener::on_requested_incompatible_qos,
            "Requested incompatible QoS callback."
        )
        .def(
            "on_sample_rejected",
            (void (pyrti::PyNoOpAnyDataReaderListener::*)(pyrti::PyAnyDataReader&, const dds::core::status::SampleRejectedStatus&)) &pyrti::PyNoOpAnyDataReaderListener::on_sample_rejected,
            "Sample rejected callback."
        )
        .def(
            "on_liveliness_changed",
            (void (pyrti::PyNoOpAnyDataReaderListener::*)(pyrti::PyAnyDataReader&, const dds::core::status::LivelinessChangedStatus&)) &pyrti::PyNoOpAnyDataReaderListener::on_liveliness_changed,
            "Liveliness changed callback."
        )
        .def(
            "on_data_available",
            (void (pyrti::PyNoOpAnyDataReaderListener::*)(pyrti::PyAnyDataReader&)) &pyrti::PyNoOpAnyDataReaderListener::on_data_available,
            "Data available callback."
        )
        .def(
            "on_subscription_matched",
            (void (pyrti::PyNoOpAnyDataReaderListener::*)(pyrti::PyAnyDataReader&, const dds::core::status::SubscriptionMatchedStatus&)) &pyrti::PyNoOpAnyDataReaderListener::on_subscription_matched,
            "Subscription matched callback."
        )
        .def(
            "on_sample_lost",
            (void (pyrti::PyNoOpAnyDataReaderListener::*)(pyrti::PyAnyDataReader&, const dds::core::status::SampleLostStatus&)) &pyrti::PyNoOpAnyDataReaderListener::on_sample_lost,
            "Sample lost callback."
        );
}

template<>
void pyrti::process_inits<dds::sub::AnyDataReaderListener>(py::module& m, pyrti::ClassInitList& l) {
    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyAnyDataReaderListener, pyrti::PyAnyDataReaderListenerTrampoline<>>(m, "AnyDataReaderListener");
        }
    );

    l.push_back(
        [m]() mutable {
            return pyrti::init_class<pyrti::PyNoOpAnyDataReaderListener, pyrti::PyAnyDataReaderListener, pyrti::PyNoOpAnyDataReaderListenerTrampoline<>>(m, "NoOpAnyDataReaderListener");
        }
    );
}
